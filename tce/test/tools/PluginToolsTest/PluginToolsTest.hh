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
 * @file PluginToolsTest.hh 
 *
 * A test suite for PluginTools.
 *
 * @author Jussi Nyk�nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J��skel�inen 2005 (pjaaskel@cs.tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: green
 */

#ifndef TTA_PLUGIN_TOOLS_TEST_HH
#define TTA_PLUGIN_TOOLS_TEST_HH

#include <TestSuite.h>
#include <string>

#include "PluginTools.hh"
#include "Conversion.hh"
#include "FileSystem.hh"
#include "Exception.hh"

using std::string;


/**
 * A class that tests PluginTools.
 */
class PluginToolsTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testLoadVariable();
    void testLoadFunction();
    void testLoadBadSymbol();
    void testLoadWithAbsolutePath();
    void testAddNonExistingPath();
    void testRemoveSearchPath();
    void testClearSearchPaths();
    void testRegisterModuleWithAbsolutePath();
    void testLoadSymWhenNoModuleDefined();
    void testRegisterModuleMultipleTimes();
    void testUnregisterModule();
    void testUnregisterAll();
    void testDlclose();
    void testModulesWithClashingSymbols();
    void testLoadWithoutRegistering();
    void testErrors();
    
private:
    /// Path where plug-in modules are.
    static const string PLUGIN_SEARCH_PATH;
    /// Non-existing path.
    static const string NON_EXISTING_PATH;
};

const string PluginToolsTest::PLUGIN_SEARCH_PATH = 
    FileSystem::currentWorkingDir() + FileSystem::DIRECTORY_SEPARATOR + "data";
const string PluginToolsTest::NON_EXISTING_PATH =
    PLUGIN_SEARCH_PATH + FileSystem::DIRECTORY_SEPARATOR + "foo" +
    FileSystem::DIRECTORY_SEPARATOR + "bar" + "foo";


/**
 * Called before each test.
 */
void
PluginToolsTest::setUp() {
}


/**
 * Called after each test.
 */
void
PluginToolsTest::tearDown() {
}


/*
 * Loads a variable from a dynamic test module, foo.so, and checks
 * its value.
 */
void
PluginToolsTest::testLoadVariable() {
    PluginTools tools;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("foo.so"));
    int* variable = NULL;
    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("var", variable));
    TS_ASSERT_EQUALS(*variable, 5);
}


/**
 * Loads a function from a dynamic test module, foo.so, and checks
 * its value.
 */
void
PluginToolsTest::testLoadFunction() {
    PluginTools tools;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("foo.so"));
    int (*function)(int);
    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("bar", function, "foo.so"));
    TS_ASSERT_EQUALS(15, (*function)(5));
}


/**
 * Tests that DynamicLibraryException is thrown when trying to import
 * non-existing symbols.
 */
void
PluginToolsTest::testLoadBadSymbol() {
    PluginTools tools;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("foo.so"));
    int (*func)(int);
    TS_ASSERT_THROWS(
        tools.importSymbol("daa", func, "foo.so"), DynamicLibraryException);
}


/**
 * Loads a function from a dynamic test module foo2.so using absolute path
 * which is not found in search path list and not yet registered.
 */
void
PluginToolsTest::testLoadWithAbsolutePath() {
    PluginTools tools;
    int* variable;
    string path = PLUGIN_SEARCH_PATH + FileSystem::DIRECTORY_SEPARATOR +
        "foo2.so";
    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("var", variable, path));
    TS_ASSERT_EQUALS(*variable, 50);
    int (*function)(int);
    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("bar", function, "foo2.so"));
    TS_ASSERT_EQUALS(15, (*function)(15));
}


/**
 * Tests that adding a non-existing path throws.
 */
void
PluginToolsTest::testAddNonExistingPath() {
    PluginTools tools;
    TS_ASSERT_THROWS(tools.addSearchPath(NON_EXISTING_PATH), FileNotFound);
}


/**
 * Tests that removing the search path works.
 */
void
PluginToolsTest::testRemoveSearchPath() {
    PluginTools tools;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.removeSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS(tools.registerModule("foo.so"), FileNotFound);
}


/**
 * Tests that removing all search paths works.
 */
void
PluginToolsTest::testClearSearchPaths() {
    PluginTools tools;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    tools.clearSearchPaths();
    TS_ASSERT_THROWS(tools.registerModule("foo.so"), FileNotFound);
}


/**
 * Test registering the module using an absolute path.
 */
void
PluginToolsTest::testRegisterModuleWithAbsolutePath() {
    PluginTools tools;
    string path = PLUGIN_SEARCH_PATH + FileSystem::DIRECTORY_SEPARATOR +
        "foo.so";
    int* variable;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule(path));
    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("var", variable));
    TS_ASSERT_EQUALS(*variable, 5);
}


/**
 * Test that registering the same module twice is not an error.
 */
void
PluginToolsTest::testRegisterModuleMultipleTimes() {
    PluginTools tools;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("foo.so"));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("foo.so"));
}


/**
 * Test that loadSym() works if no module is defined.
 */
void
PluginToolsTest::testLoadSymWhenNoModuleDefined() {
    PluginTools tools;
    int* variable;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("foo.so"));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("foo2.so"));
    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("var2", variable));
    TS_ASSERT_EQUALS(*variable, 100);
}


/**
 * Test that the unregistering of the module works.
 */
void
PluginToolsTest::testUnregisterModule() {
    PluginTools tools;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("foo.so"));
    TS_ASSERT_THROWS_NOTHING(tools.unregisterModule("foo.so"));
    string absPath = PLUGIN_SEARCH_PATH + FileSystem::DIRECTORY_SEPARATOR +
        "foo.so";
    TS_ASSERT_THROWS(tools.unregisterModule(absPath), FileNotFound);
    int* variable = NULL;
    TS_ASSERT_THROWS(tools.importSymbol("var", variable), SymbolNotFound);
}


/**
 * Tests that unregistering all modules works.
 */
void
PluginToolsTest::testUnregisterAll() {
    PluginTools tools;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("foo.so"));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("foo2.so"));
    int (*func)(int);
    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("bar", func));
    TS_ASSERT_EQUALS((*func)(3), 9);
    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("bar", func, "foo2.so"));
    TS_ASSERT_EQUALS((*func)(3), 3);
    TS_ASSERT_THROWS_NOTHING(tools.unregisterAllModules());
    TS_ASSERT_THROWS(tools.importSymbol("bar", func), SymbolNotFound);
}


/**
 * Tests that dlclose() works correctly when multiple instances
 * of PluginTools opens the same module.
 */
void
PluginToolsTest::testDlclose() {
    PluginTools tools1;
    PluginTools tools2;

    TS_ASSERT_THROWS_NOTHING(tools1.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools2.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools1.registerModule("foo.so"));
    TS_ASSERT_THROWS_NOTHING(tools2.registerModule("foo.so"));
    TS_ASSERT_THROWS_NOTHING(tools1.unregisterModule("foo.so"));
    TS_ASSERT_THROWS_NOTHING(tools2.unregisterModule("foo.so"));
}


/**
 * Loads two modules with a clashing symbol.
 *
 * Both modules define symbol 'int var' with different initial values and
 * functions that handle this variable. Because of RTLD_LOCAL flag used
 * in dlsym(), the first var should not be used in both functions. 
 */
void 
PluginToolsTest::testModulesWithClashingSymbols() {
    PluginTools tools;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("clashing1.so"));
    TS_ASSERT_THROWS_NOTHING(tools.registerModule("clashing2.so"));

    int (*func)();
    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("bar1", func));
    TS_ASSERT_EQUALS(func(), 51);

    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("bar2", func));
    TS_ASSERT_EQUALS(func(), 101);
    
}


/**
 * Tests loading symbol without registering the module first.
 */
void
PluginToolsTest::testLoadWithoutRegistering() {
    PluginTools tools;
    int* variable;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(tools.importSymbol("var", variable, "foo.so"));
    TS_ASSERT_EQUALS(*variable, 5);
}


#include "Application.hh"
/**
 * Tests various erronous situations.
 */
void
PluginToolsTest::testErrors() {
    PluginTools tools;
    TS_ASSERT_THROWS_NOTHING(tools.addSearchPath(PLUGIN_SEARCH_PATH));
    TS_ASSERT_THROWS(tools.registerModule(""), FileNotFound);
    // this causes a memory leak, seems to be a problem in dlopen()
    // implementation, it doesn't clean up all of the memory it 
    // reserved when trying to open an erroneus library
    TS_ASSERT_THROWS(tools.registerModule("foo.cc"), DynamicLibraryException);
}

#endif
