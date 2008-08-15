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
 * @file ProximDisassemblyGridTable.cc
 *
 * Implementation of ProximDisassemblyGridTable class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <math.h>
#include "ProximDisassemblyGridTable.hh"
#include "StopPointManager.hh"
#include "Breakpoint.hh"
#include "WxConversion.hh"
#include "ProximToolbox.hh"
#include "TracedSimulatorFrontend.hh"
#include "InstructionMemory.hh"
#include "ExecutableInstruction.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "ProximDisasmAttrProvider.hh"

/**
 * The Constructor.
 */
ProximDisassemblyGridTable::ProximDisassemblyGridTable():
    DisassemblyGridTable(), bpManager_(NULL),
    attrProvider_(NULL) {
        
}


/**
 * The Destructor.
 */
ProximDisassemblyGridTable::~ProximDisassemblyGridTable() {
    if (attrProvider_ != NULL) {
        delete attrProvider_;
    }
}

/**
 * Sets the breakpoint manager containing program breakpoints to display.
 *
 * @param bpManager Breakpoint manager of the program.
 */
void
ProximDisassemblyGridTable::setStopPointManager(
    StopPointManager& bpManager) {

    bpManager_ = &bpManager;
}


/**
 * Returns text value of a cell.
 *
 * @param row Row of the cell.
 * @param col Column of the cell.
 * @return Value of the cell as wxString.
 */
wxString
ProximDisassemblyGridTable::GetValue(int row, int col) {

    if (!ProximToolbox::frontend()->isProgramLoaded()) {
        return _T("");
    }

    wxString value;

    if (col == 0 && bpManager_ != NULL) {
        for (unsigned i = 0; i < bpManager_->stopPointCount(); i++) {
            unsigned handle = bpManager_->stopPointHandle(i);

            const Breakpoint* bp =
                dynamic_cast<const Breakpoint*>(
                      &bpManager_->stopPointWithHandleConst(handle));

            if (bp != NULL) {
                Word address = bp->address();
                if (address == (Word)row) {
                    value.Append(WxConversion::toWxString(handle));
                    value.Append(_T(" "));
                }
            }
        }
    }
    
    if (col == 0 && showPCArrow_ && (Word)row == currentInstruction_) {
        value.Append(_T("next>"));
    }

    if (col > 0) {
        value = DisassemblyGridTable::GetValue(row, col - 1);
    }
    return value;
}

/**
 * Returns number of columns in the grid.
 *
 * @return Number of columns.
 */
int
ProximDisassemblyGridTable::GetNumberCols() {

    if (!ProximToolbox::frontend()->isProgramLoaded()) {
        return 0;
    }

    return DisassemblyGridTable::GetNumberCols() + 1;
}


/**
 * Returns number of rows in the grid.
 *
 * @return Number of rows.
 */
int
ProximDisassemblyGridTable::GetNumberRows() {

    if (!ProximToolbox::frontend()->isProgramLoaded()) {
        return 0;
    }

    return DisassemblyGridTable::GetNumberRows();
}

/**
 * Returns label for a column.
 *
 * @param col Column index.
 * @return Column label.
 */
wxString
ProximDisassemblyGridTable::GetColLabelValue(int col) {
    if (col > 1) {
        return DisassemblyGridTable::GetColLabelValue(col - 1);
    } else {
        return _T("");
    }
}

/**
 * Turns on showing of the arrow displaying the current instruction.
 */
void
ProximDisassemblyGridTable::showPCArrow() {
    showPCArrow_ = true;
}

/**
 * Turns off showing of the arrow displaying the current instruction.
 */
void
ProximDisassemblyGridTable::hidePCArrow() {
    showPCArrow_ = false;
}


/**
 * Sets address of the current instruction.
 *
 * @param address Address of the current instruction.
 */
void
ProximDisassemblyGridTable::setCurrentInstruction(Word address) {
    currentInstruction_ = address;
}

/**
 * Sets the cell attribute provider for cell's containing move disassembly.
 *
 * ProximDisassemblyGridTable take ownership of the pointer and deletes the
 * object when it's no longer needed.
 *
 * @param attrProvider New move cell attribute provider.
 */
void
ProximDisassemblyGridTable::setMoveCellAttrProvider(
    ProximDisasmAttrProvider* attrProvider) {

    if (attrProvider_ != NULL) {
        delete attrProvider_;
    }
    attrProvider_ = attrProvider;
}

/**
 * Returns the move cell attribute provider.
 *
 * @return Move cell attribute provider, or NULL is it's not set.
 */
ProximDisasmAttrProvider*
ProximDisassemblyGridTable::moveCellAttrProvider() const {
    return attrProvider_;
}


/**
 * Returns cell style attributes for a cell with given row and column.
 *
 * @paran row Row of the grid cell.
 * @param col Column of the grid cell.
 * @return Cell's style attribute.
 */
wxGridCellAttr*
ProximDisassemblyGridTable::GetAttr(
    int row, int col, wxGridCellAttr::wxAttrKind  /* kind */) {

    wxGridCellAttr* attr = new wxGridCellAttr();

    if (ProximToolbox::frontend()->isProgramLoaded() &&  row >= 0 && col > 1) {
        if (attrProvider_ != NULL) {
            return attrProvider_->moveCellAttr(addressOfRow(row), col - 2);
        }
    } else {
        attr->SetBackgroundColour(wxColour(180, 180, 180));
    }

    return attr;
}
