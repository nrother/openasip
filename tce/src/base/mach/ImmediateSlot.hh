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
 * @file ImmediateSlot.hh
 *
 * Declaration of ImmediateSlot class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_SLOT_HH
#define TTA_IMMEDIATE_SLOT_HH

#include "MachinePart.hh"

namespace TTAMachine {

/**
 * Immediate slot is a machine component and represents a dedicated long
 * immediate field in the TTA instruction.
 */
class ImmediateSlot : public Component {
public:
    ImmediateSlot(const std::string& name, Machine& parent)
        throw (InvalidName, ComponentAlreadyExists);
    ImmediateSlot(const ObjectState* state, Machine& parent)
        throw (ObjectStateLoadingException, ComponentAlreadyExists);
    virtual ~ImmediateSlot();

    int width() const;

    virtual void setMachine(Machine& machine)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for ImmediateSlot.
    static const std::string OSNAME_IMMEDIATE_SLOT;
};
}

#endif
