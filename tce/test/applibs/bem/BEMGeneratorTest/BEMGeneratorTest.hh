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
 * @file BEMGeneratorTest.hh 
 *
 * A test suite for BEMGenerator.
 * 
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_GENERATOR_TEST_HH
#define TTA_BEM_GENERATOR_TEST_HH

#include <string>
#include <TestSuite.h>

#include "BEMGenerator.hh"
#include "FileSystem.hh"
#include "ADFSerializer.hh"
#include "BEMSerializer.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "LImmDstRegisterField.hh"
#include "GuardField.hh"
#include "SourceField.hh"
#include "DestinationField.hh"
#include "SocketEncoding.hh"
#include "SocketCodeTable.hh"

using std::string;
using namespace TTAMachine;

const string DATA_FOLDER = "." + FileSystem::DIRECTORY_SEPARATOR + "data" + 
    FileSystem::DIRECTORY_SEPARATOR;
const string COMPLEX_FILE1 = DATA_FOLDER + "complex.adf";
const string COMPLEX_FILE2 = DATA_FOLDER + "par.adf";
const string BEM_FILE1 = DATA_FOLDER + "complex_bem.bem";
const string BEM_FILE2 = DATA_FOLDER + "par_bem.bem";

/**
 * Class that tests BEMGenerator class.
 */
class BEMGeneratorTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testGeneration1();
    void testGeneration2();
};


/**
 * Called before each test.
 */
void
BEMGeneratorTest::setUp() {
}


/**
 * Called after each test.
 */
void
BEMGeneratorTest::tearDown() {
}


/**
 * Tests generation of BEM for a machine read from an ADF file.
 */
void
BEMGeneratorTest::testGeneration1() {

    ADFSerializer serializer;
    serializer.setSourceFile(COMPLEX_FILE1);
    Machine* mach = serializer.readMachine();

    BEMGenerator generator(*mach);
    BinaryEncoding* bem = generator.generate();

    Machine::BusNavigator busNav = mach->busNavigator();
    TS_ASSERT(bem->moveSlotCount() == busNav.count());
    TS_ASSERT(bem->hasImmediateControlField());

    // check move slot for bus 'B1'
    MoveSlot& slot = bem->moveSlot("B1");
    TS_ASSERT(slot.hasGuardField());
    TS_ASSERT(slot.hasSourceField());
    TS_ASSERT(slot.hasDestinationField());
    GuardField& gField = slot.guardField();
    TS_ASSERT(gField.hasFUGuardEncoding("add", "P1", true));
    TS_ASSERT(gField.hasGPRGuardEncoding("integer", 1, false));
    TS_ASSERT(gField.hasUnconditionalGuardEncoding(false));
    SourceField& sField = slot.sourceField();
    TS_ASSERT(sField.hasSocketEncoding("S3"));
    TS_ASSERT(sField.hasSocketEncoding("S7"));
    TS_ASSERT(sField.hasSocketEncoding("S8"));
    TS_ASSERT(sField.hasSocketEncoding("S11"));
    TS_ASSERT(sField.socketEncodingCount() == 4);
    TS_ASSERT(sField.bridgeEncodingCount() == 0);
    TS_ASSERT(sField.hasImmediateEncoding());
    TS_ASSERT(sField.hasNoOperationEncoding());
    DestinationField& dField = slot.destinationField();
    TS_ASSERT(dField.hasSocketEncoding("S5"));
    TS_ASSERT(dField.hasSocketEncoding("S6"));
    TS_ASSERT(dField.hasSocketEncoding("S9"));
    TS_ASSERT(dField.hasSocketEncoding("S10"));
    TS_ASSERT(dField.hasSocketEncoding("S12"));
    TS_ASSERT(dField.hasSocketEncoding("S13"));
    TS_ASSERT(dField.socketEncodingCount() == 6);
    TS_ASSERT(dField.hasNoOperationEncoding());

    // check long immediate destination register fields
    TS_ASSERT(bem->longImmDstRegisterFieldCount() == 2);
    LImmDstRegisterField& limmDstField1 = bem->longImmDstRegisterField(0);
    LImmDstRegisterField& limmDstField2 = bem->longImmDstRegisterField(1);
    TS_ASSERT(limmDstField1.usedByInstructionTemplate("temp"));
    TS_ASSERT(limmDstField1.usedByInstructionTemplate("temp2"));
    TS_ASSERT(limmDstField2.usedByInstructionTemplate("temp"));
    TS_ASSERT(limmDstField2.usedByInstructionTemplate("temp2"));   

    BEMSerializer bemSerializer;
    bemSerializer.setDestinationFile(BEM_FILE1);
    bemSerializer.writeBinaryEncoding(*bem);

    delete mach;
    delete bem;
}


void
BEMGeneratorTest::testGeneration2() {
    ADFSerializer serializer;
    serializer.setSourceFile(COMPLEX_FILE2);
    Machine* mach = serializer.readMachine();

    BEMGenerator generator(*mach);
    BinaryEncoding* bem = generator.generate();

    // check socket code table for socket S5
    MoveSlot& slot = bem->moveSlot("bus2");
    DestinationField& dstField = slot.destinationField();
    SocketEncoding& socketEncoding = dstField.socketEncoding("S5");
    SocketCodeTable& scTable = socketEncoding.socketCodes();
    TS_ASSERT(scTable.hasFUPortCode("LSU", "P2", "stq"));
    TS_ASSERT(scTable.hasFUPortCode("LSU", "P2", "ldq"));
    
    BEMSerializer bemSerializer;
    bemSerializer.setDestinationFile(BEM_FILE2);
    bemSerializer.writeBinaryEncoding(*bem);

    delete mach;
    delete bem;
}

#endif
