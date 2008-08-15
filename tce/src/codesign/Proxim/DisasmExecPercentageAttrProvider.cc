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
 * @file DisasmExecPercentageAttrProvider.cc
 *
 * Implementation of DisasmExecPercentageAttrProvider class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/grid.h>
#include <math.h>
#include "DisasmExecPercentageAttrProvider.hh"
#include "TracedSimulatorFrontend.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "ExecutableInstruction.hh"
#include "Move.hh"

/**
 * The Constructor.
 *
 * @param simulator Simulator frontend for accessing instructions and execution
 *                  counts.
 */
DisasmExecPercentageAttrProvider::DisasmExecPercentageAttrProvider(
    const TracedSimulatorFrontend& simulator):
    ProximDisasmAttrProvider(), simulator_(simulator) {
}


/**
 * The Destructor.
 */
DisasmExecPercentageAttrProvider::~DisasmExecPercentageAttrProvider() {
}


/**
 * Returns grid cell attributes for cell with given move.
 *
 * @param address Address of the cell's instruction.
 */
wxGridCellAttr*
DisasmExecPercentageAttrProvider::moveCellAttr(
    InstructionAddress address, int move) {
    
    assert(simulator_.isProgramLoaded());

    wxGridCellAttr* attr = new wxGridCellAttr();

    const ExecutableInstruction& ins =
        simulator_.executableInstructionAt(address);

    const TTAProgram::Program& program = simulator_.program();

    const TTAMachine::Machine::BusNavigator& nav =
        program.targetProcessor().busNavigator();

    assert(move >= 0);

    if (move >= nav.count()) {
        // No highlight for immediate slots.
        return attr;
    }

    const TTAMachine::Bus* moveBus = nav.item(move);
    const TTAProgram::Instruction& instruction =
        program.instructionAt(address);

    // Search for the correct move index in the instruction.
    // TTAProgram::Instruction doesn't contain nops and the move indexing
    // differs from the busNavigator and disassembly grid indexing.
    int index = 0;
    for (; index < instruction.moveCount(); index++) {
        if (moveBus == &instruction.move(index).bus()) {
            break;
        }
    }

    ClockCycleCount executions = 0;
    if (index < instruction.moveCount()) {
        executions = ins.moveExecutionCount(index);
    }

    if (executions > 0) {
        ClockCycleCount cycles = simulator_.cycleCount();
        int colour = static_cast<int>(
            5 * 255 * sin((executions / cycles) * 0.5 * 3.1415926));
        if (colour > 255) colour = 255;
        attr->SetBackgroundColour(
            wxColour(255, 255 - colour, 255 - colour));
    } else {
        // Gray background colour for moves that are not executed at all.
        attr->SetBackgroundColour(wxColour(220, 220, 220));
    }
    return attr;
}
