/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ExecutionTrace.cc
 *
 * Definition of ExecutionTrace class.
 *
 * @author Pekka Jääskeläinen 2004 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "boost/format.hpp"

#include "Application.hh"
#include "ExecutionTrace.hh"
#include "Conversion.hh"
#include "InstructionExecution.hh"
#include "FileSystem.hh"
#include "SQLite.hh"
#include "SimValue.hh"
#include "RelationalDBQueryResult.hh"
#include "DataObject.hh"

/// database table creation queries (CQ)

const std::string CQ_DATABASE_INFO =
"CREATE TABLE database_info (" 
"       version INTEGER PRIMARY KEY," 
"       tpef_filename TEXT);";

const std::string CQ_INSTRUCTION_EXECUTION =
"CREATE TABLE instruction_execution (" 
"       cycle INTEGER PRIMARY KEY," 
"       address INTEGER NOT NULL);";

const std::string CQ_PROCEDURE_TRANSFER =
"CREATE TABLE procedure_transfer (" 
"       cycle INTEGER PRIMARY KEY," 
"       address INTEGER NOT NULL, "
"       type INTEGER NOT NULL);";

const std::string CQ_INSTRUCTION_EXECUTION_COUNT =
"CREATE TABLE instruction_execution_count ("
"       address INTEGER PRIMARY KEY, "
"       count INTEGER);"; 

const std::string CQ_PROCEDURE_ADDRESS_RANGE =
"CREATE TABLE procedure_address_range ("
"       first_address INTEGER UNIQUE, "
"       last_address INTEGER UNIQUE, "
"       procedure_name TEXT NOT NULL);";

const std::string CQ_BUS_ACTIVITY = 
"CREATE TABLE bus_activity ("
"       cycle INTEGER NOT NULL,"
"       bus TEXT NOT NULL,"
"       segment TEXT NOT NULL,"
"       squash BOOLEAN DEFAULT false,"
"       data_as_int INTEGER,"
"       data_as_double REAL);";

const std::string CQ_CONCURRENT_REGISTER_FILE_ACCESS = 
"CREATE TABLE concurrent_register_file_access ("
"       register_file TEXT NOT NULL,"
"       reads INTEGER NOT NULL,"
"       writes INTEGER NOT NULL,"
"       count INTEGER NOT NULL);";

const std::string CQ_REGISTER_ACCESS = 
"CREATE TABLE register_access ("
"       register_file TEXT NOT NULL,"
"       register_index INTEGER NOT NULL,"
"       reads INTEGER NOT NULL,"
"       writes INTEGER NOT NULL);";

const std::string CQ_FU_OPERATION_TRIGGERS = 
"CREATE TABLE fu_operation_triggers ("
"       function_unit TEXT NOT NULL,"
"       operation TEXT NOT NULL,"
"       count INTEGER NOT NULL);";

const std::string CQ_BUS_WRITE_COUNTS = 
"CREATE TABLE bus_write_counts ("
"       bus TEXT NOT NULL,"
"       writes INTEGER NOT NULL);";

const std::string CQ_SOCKET_WRITE_COUNTS = 
"CREATE TABLE socket_write_counts ("
"       socket TEXT NOT NULL,"
"       writes INTEGER NOT NULL);";

const std::string CQ_TOTALS = 
"CREATE TABLE totals ("
"       value_name TEXT NOT NULL,"
"       integer_value INTEGER);";

/// the version number of the database schema
const int DB_VERSION = 1;

/** 
 * Creates a new execution trace database.
 *
 * If the database file cannot be found, creates a new database file and 
 * initializes it.
 *
 * @param fileName Full path to the database to be opened.
 * @return A pointer to opened execution trace database instance. Instance
 *         is owned by the client and should be deleted after use.
 * @exception IOException If there was a problem opening the database,
 *                        for example, if the file cannot be found and a new 
 *                        file cannot be created.
 */
ExecutionTrace*
ExecutionTrace::open(const std::string& fileName) 
    throw (IOException) {
    
    ExecutionTrace* traceDB = 
        new ExecutionTrace(
            fileName, FileSystem::fileExists(fileName) && 
            !FileSystem::fileIsWritable(fileName));
    try {
        bool newDatabase = !FileSystem::fileExists(fileName);
        traceDB->open();
        if (newDatabase) {
            traceDB->initialize();	
        } else {
            // tests that the file is really a trace DB by querying the
            // instruction_execution table
            traceDB->instructionExecutions();
        }
    } catch (const RelationalDBException& e) {
        delete traceDB;
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }

    return traceDB;
}

/**
 * Opens a new connection to the data base.
 *
 * Starts a new transaction. Transaction is committed in destructor. This
 * consumes lots of memory but should reduce I/O to the minimum.
 */
void
ExecutionTrace::open() 
    throw (RelationalDBException) {
    dbConnection_ = &db_->connect(fileName_);
    dbConnection_->beginTransaction();    
}

/**
 * Constructor.
 *
 * @param fileName Filename used for accessing the database.
 * @param readOnly Is the database read-only?
 */
ExecutionTrace::ExecutionTrace(const std::string& fileName, bool readOnly) : 
    fileName_(fileName), readOnly_(readOnly), db_(new SQLite()), 
    dbConnection_(NULL), instructionExecution_(NULL)  {
}

/**
 * Destructor.
 *
 * Closes the database and frees the resources connected to it. Commits the
 * transaction so updates are written to disk.
 * 
 */
ExecutionTrace::~ExecutionTrace() {

    try {
        if (instructionExecution_ != NULL) {
            delete instructionExecution_;
            instructionExecution_ = NULL;
        }

        if (dbConnection_ != NULL) {
            dbConnection_->commit();
            try {
                assert(db_ != NULL);
                db_->close(*dbConnection_);
                dbConnection_ = NULL;
            } catch (const RelationalDBException& e) {
                Application::writeToErrorLog(
                    __FILE__, __LINE__, __func__, e.errorMessage());
            }
        }

        if (db_ != NULL) {
            delete db_;
            db_ = NULL;
        }
    
    } catch (const Exception& e) {
        debugLog(
            "Exception almost leaked from ~ExecutionTrace! Message: " +
            e.errorMessage());
    }
}

/**
 * Initializes a new trace database.
 *
 * @exception IOException If an I/O error occured.
 */
void 
ExecutionTrace::initialize() throw (IOException) {

    assert(dbConnection_ != NULL);
    
    try {
        dbConnection_->DDLQuery(CQ_DATABASE_INFO);
        dbConnection_->DDLQuery(CQ_INSTRUCTION_EXECUTION);
        dbConnection_->DDLQuery(CQ_PROCEDURE_TRANSFER);
        dbConnection_->DDLQuery(CQ_INSTRUCTION_EXECUTION_COUNT);
        dbConnection_->DDLQuery(CQ_PROCEDURE_ADDRESS_RANGE);
        dbConnection_->DDLQuery(CQ_BUS_ACTIVITY);
        dbConnection_->DDLQuery(CQ_REGISTER_ACCESS);
        dbConnection_->DDLQuery(CQ_CONCURRENT_REGISTER_FILE_ACCESS);
        dbConnection_->DDLQuery(CQ_TOTALS);
        dbConnection_->DDLQuery(CQ_FU_OPERATION_TRIGGERS);
        dbConnection_->DDLQuery(CQ_BUS_WRITE_COUNTS);
        dbConnection_->DDLQuery(CQ_SOCKET_WRITE_COUNTS);

        dbConnection_->updateQuery(
            (boost::format(
                "INSERT INTO database_info(version) VALUES (%d);") %
             DB_VERSION).str());

    } catch (const RelationalDBException& e) {
        debugLog(
            std::string("") + "Error while initializing TraceDB. " +
            e.errorMessage());
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}

/**
 * Adds a new instruction execution record to the database.
 *
 * @param cycle The clock cycle on which the instruction execution happened.
 * @param address The address of the executed instruction.
 * @exception IOException In case an error in adding the data happened.
 */
void
ExecutionTrace::addInstructionExecution(
    ClockCycleCount cycle, 
    InstructionAddress address) 
    throw (IOException) {
    
    const std::string query =
      (boost::format(
        "INSERT INTO instruction_execution(cycle, address) VALUES(" 
	"%.0f, %d);") % cycle % address).str();

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}

/**
 * Adds a new instruction execution count record to the database.
 *
 * @param address The address of the executed instruction.
 * @param count The count of clock cycles during which execution of the 
 *              instruction happened.
 * @exception IOException In case an error in adding the data happened.
 */
void
ExecutionTrace::addInstructionExecutionCount(
    InstructionAddress address,
    ClockCycleCount count) 
    throw (IOException) {
    
    const std::string query =
        "INSERT INTO instruction_execution_count(address, count) VALUES(" + 
        Conversion::toString(address) + ", " + 
        Conversion::toString(count) + ");";

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}

/**
 * Adds a procedure address range entry to the database.
 *
 * An entry in this table tells the range of instruction addresses belonging
 * to a procedure.
 *
 * @param firstAddress The starting address of the procedure.
 * @param lastAddress The ending address of the procedure.
 * @param name The name of the procedure.
 * @exception IOException In case an error in adding the data happened.
 */
void
ExecutionTrace::addProcedureAddressRange(
    InstructionAddress firstAddress,
    InstructionAddress lastAddress,
    const std::string& procedureName)
    throw (IOException) {
    
    const std::string query =
        "INSERT INTO procedure_address_range(first_address, last_address, "
        "procedure_name) VALUES(" + 
        Conversion::toString(firstAddress) + ", " + 
        Conversion::toString(lastAddress) + ", '" + 
        procedureName + "');";

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}



/**
 * Queries database for instruction execution entries. 
 *
 * @return A handle object which can be used to traverse through the results. 
 *         The handle points to the first instruction execution in the list of 
 *         executions, sorted by the cycle.
 * @exception IOException If an I/O error occurs.
 */
InstructionExecution&
ExecutionTrace::instructionExecutions() throw (IOException) {

    if (instructionExecution_ != NULL) {
        delete instructionExecution_;
        instructionExecution_ = NULL;
    }

    try {
        instructionExecution_ = new InstructionExecution(
            dbConnection_->query(
                "SELECT * FROM instruction_execution ORDER BY cycle"));
    } catch (const Exception& e) {
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    } 

    return *instructionExecution_;
}

/**
 * Adds a new bus activity record to the database.
 *
 * @param cycle The clock cycle in which the bus activity happened.
 * @param busId The name of the bus.
 * @param segmentId The name of the segment in the bus.
 * @param squash Whether the transfer was squashed or not.
 * @param data The data transferred.
 * @exception IOException In case an error in adding the data happened.
 */
void 
ExecutionTrace::addBusActivity(
    ClockCycleCount cycle, 
    const BusID& busId,
    const SegmentID& segmentId,
    bool squash,
    const SimValue& data) 
    throw (IOException) {
    
    std::string query = 
        "INSERT INTO bus_activity(cycle, bus, segment, squash, "
        "data_as_int, data_as_double) VALUES(" + 
        Conversion::toString(cycle) + ", '" + busId + "', '" + segmentId + 
        "', ";

    if (squash) 
        query += "'TRUE'";
    else
        query += "'FALSE'";

    std::string doubleString = Conversion::toString(data.doubleWordValue());
    if (doubleString == "nan")
        doubleString = "NULL";

    if (!squash && &data != &NullSimValue::instance())
        query += ", " + Conversion::toString(data.uIntWordValue()) + ", " + 
            doubleString + ")";        
    else
        query += ", 0, 0.0)";
 

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        debugLog(std::string("query: ") + query);
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}

/**
 * Adds a concurrent register file access statistics to the database.
 *
 * Statistics tells how many times a type of read/write count combination
 * access happened. That is, how many times the given register file was
 * read and written the given times simultaneously.
 *
 * @param registerFile The name of the register file.
 * @param reads Count of simultaneous reads.
 * @param writes Count of simultaneous writes.
 * @param count The count of this type of accesses.
 * @exception IOException In case an error in adding the data happened.
 */
void 
ExecutionTrace::addConcurrentRegisterFileAccessCount(
    RegisterFileID registerFile,    
    RegisterAccessCount reads,
    RegisterAccessCount writes,
    ClockCycleCount count)
    throw (IOException) {

    const std::string query =
        std::string("") +
        "INSERT INTO concurrent_register_file_access("
        "                register_file, reads, writes, count) "
        "VALUES('" + registerFile + "', " +
        Conversion::toString(reads) + ", " +
        Conversion::toString(writes) + ", " +
        Conversion::toString(count) + ")";

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        debugLog(query + " failed!");
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}

/**
 * Adds a register access statistics to the database.
 *
 * Statistics tells how many times a register was read and written during
 * simulation.
 *
 * @param registerFile The name of the register file.
 * @param registerIndex The index of the register.
 * @param reads Count of reads.
 * @param writes Count of writes.
 * @exception IOException In case an error in adding the data happened.
 */
void 
ExecutionTrace::addRegisterAccessCount(
    RegisterFileID registerFile,
    RegisterID registerIndex,
    ClockCycleCount reads,
    ClockCycleCount writes)
    throw (IOException) {
    
    const std::string query =
        (boost::format(
            "INSERT INTO register_access("
            "                register_file, register_index, reads, writes) "
            "VALUES('%s', %d, %.0f, %.0f)") 
         % registerFile % registerIndex % reads % writes).str();

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        debugLog(query + " failed!");
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}


/**
 * Returns the list of different concurrent register file access combinations
 * and counts how many times those were encountered while simulating the
 * program.
 *
 * @param registerFile The register file for which the stats are needed.
 * @return A list of accesses. Must be deleted by the client after use.
 * @exception IOException If an I/O error occurs.
 */
ExecutionTrace::ConcurrentRFAccessCountList* 
ExecutionTrace::registerFileAccessCounts(RegisterFileID registerFile) const
    throw (IOException) {

    ConcurrentRFAccessCountList* accesses = NULL;
    try {
        assert(dbConnection_ != NULL);

        RelationalDBQueryResult* result =
            dbConnection_->query(
                std::string("") +
                "SELECT * FROM concurrent_register_file_access " +
                "WHERE register_file LIKE('" + registerFile + "')");
        accesses = new ConcurrentRFAccessCountList();
        while (result->hasNext()) {
            result->next();
            accesses->push_back(
                boost::make_tuple(
                    (result->data("reads")).integerValue(),
                    (result->data("writes")).integerValue(),
                    (result->data("count")).integerValue()));
        }
        delete result;
        result = NULL;
    } catch (const Exception& e) {
        delete accesses;
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    } 

    return accesses;    
}

/**
 * Adds a function unit operation execution statistics to the database.
 *
 * Statistics tells how many times an operation was executed in an function
 * unit.
 *
 * @param functionUnit The name of the function unit.
 * @param operation The name of the operation.
 * @param count The count of executions.
 * @exception IOException In case an error in adding the data happened.
 */
void 
ExecutionTrace::addFunctionUnitOperationTriggerCount(
    FunctionUnitID functionUnit,
    OperationID operation,
    OperationTriggerCount count)
    throw (IOException) {

    const std::string query =
        std::string("") +
        "INSERT INTO fu_operation_triggers("
        "                function_unit, operation, count) "
        "VALUES('" + functionUnit + "', '" + operation + "', " +
        Conversion::toString(count) + ")";

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        debugLog(query + " failed!");
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}

/**
 * Adds a procedure transfer tracking data entry.
 *
 * @param cycle The clock cycle in which the transfer occured.
 * @param address The instruction address to which the execution changed.
 * @param type Type of the transfer.
 */
void 
ExecutionTrace::addProcedureTransfer(
    ClockCycleCount cycle,
    InstructionAddress address,
    ProcedureEntryType type)
    throw (IOException) {

    const std::string query =
        (boost::format(
            "INSERT INTO procedure_transfer(cycle, address, type) "
            "VALUES(%.0f, %d, %d)") % cycle % address % type).str();

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        debugLog(query + " failed!");
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}

/**
 * Returns the list of operation access counts for wanted function unit.
 *
 * @param functionUnit The function unit for which the stats are needed.
 * @return A list of access counts. Must be deleted by the client after use.
 * @exception IOException If an I/O error occurs.
 */
ExecutionTrace::FUOperationTriggerCountList* 
ExecutionTrace::functionUnitOperationTriggerCounts(
    FunctionUnitID functionUnit) const
    throw (IOException) {

    FUOperationTriggerCountList* accesses = NULL;
    try {
        assert(dbConnection_ != NULL);

        RelationalDBQueryResult* result =
            dbConnection_->query(
                std::string("") +
                "SELECT * FROM fu_operation_triggers WHERE function_unit "
                "LIKE('" + functionUnit + "')");
        accesses = new FUOperationTriggerCountList();
        while (result->hasNext()) {
            result->next();
            accesses->push_back(
                boost::make_tuple(
                    (result->data("operation")).stringValue(),
                    (result->data("count")).integerValue()));
        }
        delete result;
        result = NULL;
    } catch (const Exception& e) {
        delete accesses;
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    } 

    return accesses;    
}

/**
 * Adds a socket write count statistics to the database.
 *
 * This stats tells in how many clock cycles was a socket used (transported
 * data through).
 *
 * @param socket The name of the socket.
 * @param count The count of writes/uses.
 * @exception IOException In case an error in adding the data happened.
 */
void
ExecutionTrace::addSocketWriteCount(SocketID socket, ClockCycleCount count)
    throw (IOException) {

    const std::string query =
        std::string("") +
        "INSERT INTO socket_write_counts(socket, writes) "
        "VALUES('" + socket + "', " + Conversion::toString(count) + ")";

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        debugLog(query + " failed!");
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }

}

/**
 * Returns the count of clock cycles in which a socket was written to.
 *
 * In case no data was found for the given socket in DB, returns 0.
 *
 * @param socket The name of the socket.
 * @return The count of writes/accesses.
 */
ClockCycleCount
ExecutionTrace::socketWriteCount(SocketID socket) const {

    RelationalDBQueryResult* result = NULL;
    ClockCycleCount count = 0;
    try {
        result = dbConnection_->query(
            (boost::format(
                "SELECT writes FROM socket_write_counts "
                "WHERE socket LIKE('%s')") % socket).str());

        if (result->hasNext()) {
            result->next();
            count = (result->data("writes")).integerValue();
        }
    } catch (const Exception&) {
    } 
    delete result;
    result = NULL;
    return count;
}

/**
 * Adds a bus write count statistics to the database.
 *
 * This stats tells in how many clock cycles was a bus used (transported
 * data through).
 *
 * @param socket The name of the bus.
 * @param count The count of writes/uses.
 * @exception IOException In case an error in adding the data happened.
 */
void
ExecutionTrace::addBusWriteCount(BusID bus, ClockCycleCount count)
    throw (IOException) {

    const std::string query =
        std::string("") +
        "INSERT INTO bus_write_counts(bus, writes) "
        "VALUES('" + bus + "', " + Conversion::toString(count) + ")";

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        debugLog(query + " failed!");
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }

}

/**
 * Returns the count of clock cycles in which a bus was written to.
 *
 * In case no data was found for the given bus in DB, returns 0.
 *
 * @param bus The name of the bus.
 * @return The count of writes/accesses.
 */
ClockCycleCount
ExecutionTrace::busWriteCount(BusID bus) const {

    RelationalDBQueryResult* result = NULL;
    ClockCycleCount count = 0;
    try {
        result = dbConnection_->query(
            (boost::format(
                "SELECT writes FROM bus_write_counts "
                "WHERE bus LIKE('%s')") % bus).str());

        if (result->hasNext()) {
            result->next();
            count = (result->data("writes")).integerValue();
        }
    } catch (const Exception&) {
    } 
    delete result;
    result = NULL;
    return count;
}

/**
 * Sets the total count of simulated clock cycles.
 *
 * @param count The count of cycles.
 * @exception IOException In case an error in adding the data happened.
 */
void
ExecutionTrace::setSimulatedCycleCount(ClockCycleCount count)
    throw (IOException) {

    const std::string query =
        std::string("") +
        "INSERT INTO totals(value_name, integer_value) "
        "VALUES('cycle_count', " + Conversion::toString(count) + ")";

    assert(dbConnection_ != NULL);

    try {
        dbConnection_->updateQuery(query);
    } catch (const RelationalDBException& e) {
        debugLog(query + " failed!");
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    }
}

/**
 * Gets the total count of simulated clock cycles.
 *
 * @return The count of cycles.
 * @exception IOException In case an error in getting the data happened.
 */
ClockCycleCount
ExecutionTrace::simulatedCycleCount() const
    throw (IOException) {

    RelationalDBQueryResult* result = NULL;
    try {
        result = dbConnection_->query(
            "SELECT integer_value FROM totals "
            "WHERE value_name LIKE('cycle_count')");
        if (!result->hasNext()) {
            delete result;
            result = NULL;
            throw IOException(
                __FILE__, __LINE__, __func__, 
                "No 'cycle_count' entry in 'totals' table.");
        }
        result->next();
        ClockCycleCount count = (result->data("integer_value")).integerValue();
        delete result;
        result = NULL;
        return count;
    } catch (const Exception& e) {
        delete result;
        result = NULL;
        throw IOException(__FILE__, __LINE__, __func__, e.errorMessage());
    } 
}

