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
 * @file TPEFRelocSectionWriter.hh
 *
 * Declaration of TPEFRelocSectionWriter class.
 *
 * @author Jussi Nyk�nen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepist� 10.12.2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_RELOC_SECTION_WRITER_HH
#define TTA_TPEF_RELOC_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "TPEFBaseType.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Writes TPEF relocation section to stream.
 */
class TPEFRelocSectionWriter : public TPEFSectionWriter {
protected:
    TPEFRelocSectionWriter();
    virtual ~TPEFRelocSectionWriter();

    virtual Section::SectionType type() const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* section) const;

    virtual Word elementSize(const Section* section) const;

    virtual void writeInfo(BinaryStream& stream, const Section* sect) const;

private:
    /// Copying not allowed.
    TPEFRelocSectionWriter(const TPEFRelocSectionWriter&);
    /// Assignment not allowed.
    TPEFRelocSectionWriter operator=(const TPEFRelocSectionWriter&);

    /// An unique instance of class.
    static const TPEFRelocSectionWriter instance_;
    /// The fixed size of reloc element.
    static const Word elementSize_;
};
}

#endif
