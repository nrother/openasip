/*
 Copyright (c) 2002-2016 Tampere University.

 This file is part of TTA-Based Codesign Environment (TCE).

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 */
/*
 * @file MemoryBusInterface.hh
 *
 * Declaration of MemoryBusInterface class.
 *
 * Created on: 16.6.2016
 * @author Henry Linjam�ki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef MEMORYBUSINTERFACE_HH_
#define MEMORYBUSINTERFACE_HH_

#include "NetlistPortGroup.hh"

#include "TCEString.hh"

namespace ProGe {

/*
 * NetlistPortGroup for memory bus interface that hold metadata for it.
 */
class MemoryBusInterface: public NetlistPortGroup {
public:
    MemoryBusInterface(
        const TCEString& addressSpaceName = TCEString());
    MemoryBusInterface(
        SignalGroup groupType,
        const TCEString& addressSpaceName = TCEString());
    virtual ~MemoryBusInterface();

    virtual MemoryBusInterface* clone(bool asMirrored = false) const;

    TCEString addressSpace() const;

private:

    MemoryBusInterface();
    MemoryBusInterface(const MemoryBusInterface&, bool asMirrored = false);

    /// Name of the address space the memory bus is bound.
    TCEString addressSpaceName_ = TCEString();

};

} /* namespace ProGe */

#endif /* MEMORYBUSINTERFACE_HH_ */
