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
 * @file DisassemblyImmediateAssignment.cc
 *
 * Implementation of DisassemblyImmediateAssignment class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyImmediateAssignment.hh"
#include "Conversion.hh"

/**
 * The Constructor.
 *
 * Creates disassembly of an immediate assignment.
 *
 * @param value Value of the immediate to assign.
 * @param destination The destination register.
 */
DisassemblyImmediateAssignment::DisassemblyImmediateAssignment(
    SimValue value,
    DisassemblyElement* destination):
    DisassemblyInstructionSlot(),
    value_(value), destination_(destination), hasValue_(true) {

}

/**
 * The construtor.
 *
 * @param destination The destination register.
 */
DisassemblyImmediateAssignment::DisassemblyImmediateAssignment(
    DisassemblyElement* destination):
    DisassemblyInstructionSlot(),
    destination_(destination),
    hasValue_(false) {

}


/**
 * The destructor.
 */
DisassemblyImmediateAssignment::~DisassemblyImmediateAssignment() {
    delete destination_;
}


/**
 * Disassembles the immediate assignment.
 *
 * @return Disassembled immediate assignment as a string.
 */
std::string
DisassemblyImmediateAssignment::toString() const {
    std::string disassembly =  "[" + destination_->toString();
    if (hasValue_) {
	disassembly =
	    disassembly + "=" + Conversion::toString(value_.uIntWordValue());
    }
    disassembly = disassembly + "]";
    
    for (int i = 0; i < annotationCount(); i++) {
        disassembly += annotation(i).toString();
    }
    
    return disassembly;
}
