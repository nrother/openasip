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
 * @file AssocToolsTest.hh
 *
 * A test suite for AssocTools class.
 *
 * @author Pekka Jääskeläinen 2007 (pekka.jaaskelainen@tut.fi)
 */

#ifndef TTA_ASSOC_TOOLS_TEST_HH
#define TTA_ASSOC_TOOLS_TEST_HH

#include <TestSuite.h>
#include "AssocTools.hh"

#include <set>
#include <utility>

/**
 * Implements the tests needed to verify correct operation of AssocTools.
 */
class AssocToolsTest : public CxxTest::TestSuite {
public:
    void testPairs();
};

/**
 * Tests the pairs() function.
 */
void
AssocToolsTest::testPairs() {

    std::set<std::string> firstSet;
    firstSet.insert("a");
    firstSet.insert("b");

    std::set<int> secondSet;
    secondSet.insert(1);
    secondSet.insert(2);
    
    std::set<std::pair<std::string, int> > expected;
    expected.insert(std::make_pair("a", 1));
    expected.insert(std::make_pair("a", 2));
    expected.insert(std::make_pair("b", 1));
    expected.insert(std::make_pair("b", 2));

    std::set<std::pair<std::string, int> > pairs =
        AssocTools::pairs(firstSet, secondSet);
  
    TS_ASSERT_EQUALS(pairs, expected);
}

#endif
