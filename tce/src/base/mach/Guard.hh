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
 * @file Guard.hh
 *
 * Declaration of Guard class and its derived classes.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 */

#ifndef TTA_GUARD_HH
#define TTA_GUARD_HH

#include "MachinePart.hh"
#include "ObjectState.hh"

namespace TTAMachine {

class FUPort;
class Bus;
class RegisterFile;

/////////////////////////////////////////////////////////////////////////////
// Guard
/////////////////////////////////////////////////////////////////////////////

/**
 * Guard expression representing an execution predicate.
 *
 * Evaluates to true (execute) or false (don't execute). This is a
 * base class for real Guards.
 */
class Guard : public SubComponent {
public:
    virtual ~Guard();

    virtual Bus* parentBus() const;
    virtual bool isEqual(const Guard& guard) const = 0;
    virtual bool isInverted() const;
    virtual bool isMoreRestrictive(const Guard& guard) const;
    virtual bool isLessRestrictive(const Guard& guard) const;
    virtual bool isDisjoint(const Guard& guard) const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for guard.
    static const std::string OSNAME_GUARD;
    /// ObjectState attribute key for inverted feature.
    static const std::string OSKEY_INVERTED;

protected:
    Guard(bool inverted, Bus& parentBus);
    Guard(const ObjectState* state, Bus& parentBus)
        throw (ObjectStateLoadingException);

private:
    /// Indicated whether the condition term is inverted.
    bool inverted_;
    /// The parent bus of the guard.
    Bus* parent_;
};


/////////////////////////////////////////////////////////////////////////////
// PortGuard
/////////////////////////////////////////////////////////////////////////////

/**
 * Guard where the condition term is taken from the value of an
 * output port of a FunctionUnit.
 */
class PortGuard : public Guard {
public:
    PortGuard(
        bool inverted, 
        FUPort& port, 
        Bus& parentBus)
        throw (IllegalRegistration, ComponentAlreadyExists);
    PortGuard(const ObjectState* state, Bus& parentBus)
        throw (ObjectStateLoadingException);
    virtual ~PortGuard();

    bool isEqual(const Guard& guard) const;
    FUPort* port() const;

    ObjectState* saveState() const;
    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for PortGuard ObjectState.
    static const std::string OSNAME_PORT_GUARD;
    /// ObjectState attribute key for function unit name.
    static const std::string OSKEY_FU;
    /// ObjectState attribute key for port name.
    static const std::string OSKEY_PORT;

private:
    /// Port from which the condition term is taken.
    FUPort* port_;
};


/////////////////////////////////////////////////////////////////////////////
// RegisterGuard
/////////////////////////////////////////////////////////////////////////////

/**
 * Guard where the condition term is taken from the value of a
 * register (from a RegisterFile).
 */
class RegisterGuard : public Guard {
public:
    RegisterGuard(
        bool inverted,
        RegisterFile& regFile,
        int registerIndex,
        Bus& parentBus)
        throw (IllegalRegistration, ComponentAlreadyExists, OutOfRange,
               InvalidData);
    RegisterGuard(const ObjectState* state, Bus& parentBus)
        throw (ObjectStateLoadingException);
    virtual ~RegisterGuard();

    bool isEqual(const Guard& guard) const;
    RegisterFile* registerFile() const;
    int registerIndex() const;

    ObjectState* saveState() const;
    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for RegisterGuard.
    static const std::string OSNAME_REGISTER_GUARD;
    /// ObjectState attribute key for register file name.
    static const std::string OSKEY_REGFILE;
    /// ObjectState attribute key for register index.
    static const std::string OSKEY_INDEX;

private:
    /// RegisterFile from which the condition term is taken.
    RegisterFile* regFile_;
    /// Index of the register from which the condition term is taken.
    int registerIndex_;
};


/////////////////////////////////////////////////////////////////////////////
// UnconditionalGuard
/////////////////////////////////////////////////////////////////////////////

/**
 * Always true guard term. Always false if inverted.
 */
class UnconditionalGuard : public Guard {
public:
    UnconditionalGuard(bool inverted, Bus& parentBus)
        throw (ComponentAlreadyExists);
    UnconditionalGuard(const ObjectState* state, Bus& parentBus)
        throw (ObjectStateLoadingException);
    virtual ~UnconditionalGuard();

    bool isEqual(const Guard& guard) const;
    ObjectState* saveState() const;
    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for UnconditionalGuard.
    static const std::string OSNAME_UNCONDITIONAL_GUARD;
};
}

#include "Guard.icc"

#endif
