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
 * @file BlockImplementationFile.hh
 *
 * Declaration of BlockImplementationFile class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BLOCK_IMPLEMENTATION_FILE_HH
#define TTA_BLOCK_IMPLEMENTATION_FILE_HH

#include <string>

namespace HDB {

/**
 * Represents a file that contains implementation for a block in HDB.
 */
class BlockImplementationFile {
public:
    /// Format of the file.
    enum Format {
        VHDL ///< VHDL file.
    };

    BlockImplementationFile(const std::string& pathToFile, Format format);
    virtual ~BlockImplementationFile();
    
    std::string pathToFile() const;
    Format format() const;

    void setPathToFile(const std::string& pathToFile);
    void setFormat(Format format);

private:
    /// The file.
    std::string file_;
    /// Format of the file.
    Format format_;
};
}

#endif
