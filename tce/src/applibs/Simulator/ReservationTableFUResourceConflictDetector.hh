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
 * @file ReservationTableFUResourceConflictDetector.hh
 *
 * Declaration of ReservationTableFUResourceConflictDetector class.
 *
 * @author Pekka Jääskeläinen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CRT_FU_RESOURCE_CONFLICT_DETECTOR_HH
#define TTA_CRT_FU_RESOURCE_CONFLICT_DETECTOR_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "FunctionUnit.hh"
#include "FUResourceConflictDetector.hh"
#include "FUReservationTableIndex.hh"
#include "ReservationTable.hh"

class Operation;
class TCEString;

namespace TTAMachine {
    class FunctionUnit;
}

/**
 * A Conventional Reservation Table implementation of a FU resource conflict 
 * detector.
 */
class ReservationTableFUResourceConflictDetector : 
    public FUResourceConflictDetector {
public:

    ReservationTableFUResourceConflictDetector(
        const TTAMachine::FunctionUnit& fu)
        throw (InvalidData);
    virtual ~ReservationTableFUResourceConflictDetector();

    virtual bool issueOperation(OperationID id);
    virtual bool advanceCycle();
    virtual void reset();

    bool issueOperationInline(OperationID id);
    bool advanceCycleInline();

    virtual OperationID operationID(const TCEString& operationName) const;

private:
    /// The reservation tables of operations.
    FUReservationTableIndex reservationTables_;
    /// The global reservation table.
    ReservationTable globalReservationTable_;
    /// The modeled FU.
    const TTAMachine::FunctionUnit& fu_;
};

#include "ReservationTableFUResourceConflictDetector.icc"

#endif
