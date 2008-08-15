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
 * @file SocketCodeTable.hh
 *
 * Declaration of SocketCodeTable class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOCKET_CODE_TABLE_HH
#define TTA_SOCKET_CODE_TABLE_HH

#include <string>
#include <vector>

#include "Exception.hh"
#include "Serializable.hh"

class BinaryEncoding;
class FUPortCode;
class RFPortCode;
class IUPortCode;
class SlotField;

/**
 * If a socket is attached to several ports, each port is identified
 * by a different control code. This control code forms the variable
 * part of the socket encoding and is defined by SocketCodeTable
 * class.
 *
 * The SocketCodeTable class contains objects that represent port
 * control codes, namely FUPortCode, RFPortCode and
 * IUPortCode. Because a single SocketCodeTable instance is shared by
 * several socket encodings, it is not owned by SocketEncoding
 * objects. Instead, socket code tables are owned by BinaryEncoding
 * class, which provides methods to access and handle them.
 *
 * Port codes consists of one or two parts: port ID and/or register
 * index. Port IDs are aligned to the left end of the field and
 * register indices to the right end. That is, if port ID + register
 * index do not take all the bits of the width reserved for the table,
 * there are unused bits between port ID and register index.
 */
class SocketCodeTable : public Serializable {
public:
    SocketCodeTable(const std::string& name, BinaryEncoding& parent)
	throw (ObjectAlreadyExists);
    SocketCodeTable(const ObjectState* state, BinaryEncoding& parent)
	throw (ObjectStateLoadingException);
    virtual ~SocketCodeTable();

    BinaryEncoding* parent() const;

    std::string name() const;
    void setName(const std::string& name)
	throw (ObjectAlreadyExists);

    void setExtraBits(int bits)
	throw (OutOfRange);
    int extraBits() const;
    int width() const;

    void addFUPortCode(FUPortCode& code)
	throw (ObjectAlreadyExists);
    void removeFUPortCode(FUPortCode& code);
    int fuPortCodeCount() const;
    FUPortCode& fuPortCode(int index) const
	throw (OutOfRange);

    bool hasFUPortCode(const std::string& fu, const std::string& port) const;
    bool hasFUPortCode(
	const std::string& fu,
	const std::string& port,
	const std::string& operation) const;
    FUPortCode& fuPortCode(
	const std::string& fu,
	const std::string& port) const;
    FUPortCode& fuPortCode(
	const std::string& fu,
	const std::string& port,
	const std::string& operation) const;

    void addRFPortCode(RFPortCode& code)
	throw (ObjectAlreadyExists);
    void removeRFPortCode(RFPortCode& code);
    int rfPortCodeCount() const;
    RFPortCode& rfPortCode(int index) const
	throw (OutOfRange);
    bool hasRFPortCode(const std::string& regFile) const;
    RFPortCode& rfPortCode(const std::string& regFile) const;

    void addIUPortCode(IUPortCode& code)
	throw (ObjectAlreadyExists);
    void removeIUPortCode(IUPortCode& code);
    int iuPortCodeCount() const;
    IUPortCode& iuPortCode(int index) const
	throw (OutOfRange);
    bool hasIUPortCode(const std::string& immediateUnit) const;
    IUPortCode& iuPortCode(const std::string& immediateUnit) const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    /// ObjectState name for socket code table.
    static const std::string OSNAME_SOCKET_CODE_TABLE;
    /// ObjectState attribute key for name of the table.
    static const std::string OSKEY_NAME;
    /// ObjectState attribute key for the number of extra bits.
    static const std::string OSKEY_EXTRA_BITS;

private:
    /// Container type for FU port codes.
    typedef std::vector<FUPortCode*> FUPortCodeTable;
    /// Container type for RF port codes.
    typedef std::vector<RFPortCode*> RFPortCodeTable;
    /// Container type for IU port codes.
    typedef std::vector<IUPortCode*> IUPortCodeTable;

    void removeReferences(SlotField& field) const;
    bool hasParentSCTable(const std::string& name) const;
    void deleteRFPortCodes();
    void deleteFUPortCodes();
    void deleteIUPortCodes();
    bool hasRFOrIUPortCodeWithoutEncoding() const;
    bool containsPortCode() const;

    /// The parent binary encoding map.
    BinaryEncoding* parent_;
    /// Name of the table.
    std::string name_;
    /// FU port codes.
    FUPortCodeTable fuPortCodes_;
    /// RF port codes.
    RFPortCodeTable rfPortCodes_;
    /// IU port codes.
    IUPortCodeTable iuPortCodes_;
    /// The number of extra bits.
    int extraBits_;
};

#endif
