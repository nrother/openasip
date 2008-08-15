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
 * @file UniversalMachine.hh
 *
 * Declaration of UniversalMachine class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_UNIVERSAL_MACHINE_HH
#define TTA_UNIVERSAL_MACHINE_HH

#include "Machine.hh"
#include "OperationPool.hh"

class UniversalFunctionUnit;
class UnboundedRegisterFile;

// the size of the data address space in bytes (MAUs) is 4GB.
#define DATA_MEMORY_SIZE (static_cast<unsigned int>(4096)*1024*1024)

/**
 * UniversalMachine represents a TTA processor which can execute any kind of
 * sequential code.
 *
 * One universal machine should be created per sequential program. This is 
 * because some of the resources are created on-demand, thus depend on the 
 * sequential program used.
 */
class UniversalMachine : public TTAMachine::Machine {
public:
    UniversalMachine();
    UniversalMachine(OperationPool&);
    virtual ~UniversalMachine();

    UniversalFunctionUnit& universalFunctionUnit() const;
    TTAMachine::RegisterFile& booleanRegisterFile() const;
    UnboundedRegisterFile& integerRegisterFile() const;
    UnboundedRegisterFile& doubleRegisterFile() const;
    TTAMachine::RegisterFile& specialRegisterFile() const;
    TTAMachine::AddressSpace& instructionAddressSpace() const;
    TTAMachine::AddressSpace& dataAddressSpace() const;
    TTAMachine::Bus& universalBus() const;

    virtual void addBus(TTAMachine::Bus& bus)
        throw (ComponentAlreadyExists);
    virtual void addSocket(TTAMachine::Socket& socket)
        throw (ComponentAlreadyExists);
    virtual void addFunctionUnit(TTAMachine::FunctionUnit& unit)
        throw (ComponentAlreadyExists);
    virtual void addImmediateUnit(TTAMachine::ImmediateUnit& unit)
        throw (ComponentAlreadyExists);
    virtual void addRegisterFile(TTAMachine::RegisterFile& unit)
        throw (ComponentAlreadyExists);
    virtual void addAddressSpace(TTAMachine::AddressSpace& as)
        throw (ComponentAlreadyExists);
    virtual void addBridge(TTAMachine::Bridge& bridge)
        throw (ComponentAlreadyExists);
    virtual void addInstructionTemplate(
        TTAMachine::InstructionTemplate& iTemp)
        throw (ComponentAlreadyExists);
    virtual void setGlobalControl(TTAMachine::ControlUnit& unit)
        throw (ComponentAlreadyExists);

    virtual void removeBus(TTAMachine::Bus& bus)
        throw (InstanceNotFound);
    virtual void removeSocket(TTAMachine::Socket& socket)
        throw (InstanceNotFound);
    virtual void removeFunctionUnit(TTAMachine::FunctionUnit& unit)
        throw (InstanceNotFound);
    virtual void removeRegisterFile(TTAMachine::RegisterFile& unit)
        throw (InstanceNotFound);
    virtual void deleteAddressSpace(TTAMachine::AddressSpace& as)
        throw (InstanceNotFound);
    virtual void unsetGlobalControl();

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    void construct();

    /// Indicates whether the UniversalMachine is built completely.
    bool isBuilt_;

    /// The operation pool instance to use for finding operations.
    OperationPool opPool;
};

/// Machine component names reserved for the universal machine.
#define UM_BUS_NAME "universal_bus"
#define UM_SEGMENT_NAME "universal_segment"
#define UM_INPUT_SOCKET_NAME "universal_input_socket"
#define UM_OUTPUT_SOCKET_NAME "universal_output_socket"
#define UM_BOOLEAN_RF_NAME "universal_boolean_rf"
#define UM_BOOLEAN_RF_WRITE_PORT "universal_boolean_write_port"
#define UM_BOOLEAN_RF_READ_PORT "universal_boolean_read_port"
#define UM_INTEGER_URF_NAME "universal_integer_rf"
#define UM_INTEGER_URF_WRITE_PORT "universal_rf_write_port"
#define UM_INTEGER_URF_READ_PORT "universal_rf_read_port"
#define UM_DOUBLE_URF_NAME "universal_double_rf"
#define UM_DOUBLE_URF_WRITE_PORT "universal_drf_write"
#define UM_DOUBLE_URF_READ_PORT "universal_drf_read"
#define UM_SPECIAL_RF_NAME "universal_special_rf_return_value"
#define UM_SPECIAL_RF_WRITE_PORT "universal_special_rf_write"
#define UM_SPECIAL_RF_READ_PORT "universal_special_rf_read"
#define UM_UNIVERSAL_FU_NAME "universal_fu"
#define UM_IMEM_NAME "universal_instructions"
#define UM_DMEM_NAME "universal_data"
#define UM_GCU_NAME "universal_gcu"

#endif
