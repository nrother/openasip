/*
    Copyright (c) 2002-2015 Tampere University.

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
/**
 * @file NetlistWriter.hh
 *
 * Declaration of NetlistWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Henry Linjam�ki 2015 (henry.linjamaki-no.spam.tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_WRITER_HH
#define TTA_NETLIST_WRITER_HH

#include <string>
#include "Exception.hh"

namespace ProGe {

class BaseNetlistBlock;

/**
 * Interface for different block level netlist writers.
 */
class NetlistWriter {
public:
    NetlistWriter(const BaseNetlistBlock& targetBlock);
    virtual ~NetlistWriter();

    virtual void write(const std::string& dstDirectory) = 0;

protected:
    const BaseNetlistBlock& targetNetlistBlock() const;

private:
    /// Netlist block to be written.
    const BaseNetlistBlock& netlistBlock_;
};
}

#endif
