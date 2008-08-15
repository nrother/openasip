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
 * @file ProximClearConsoleCmd.cc
 *
 * Implementation of ProximClearConsoleCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximClearConsoleCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ConsoleWindow.hh"


/**
 * The Constructor.
 */
ProximClearConsoleCmd::ProximClearConsoleCmd():
    GUICommand("Clear Console", NULL) {

    }

/**
 * The Destructor.
 */
ProximClearConsoleCmd::~ProximClearConsoleCmd() {
}


/**
 * Executes the command.
 */
bool
ProximClearConsoleCmd::Do() {
    wxWindow* window = wxGetApp().GetTopWindow()->FindWindow(
        ProximConstants::ID_CONSOLE_WINDOW);
    assert(window != NULL);
    ConsoleWindow* console = dynamic_cast<ConsoleWindow*>(window);
    assert(console != NULL);
    console->clear();
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximClearConsoleCmd::icon() const {
    return "clear_console.png";
}


/**
 * Returns ID of this command.
 */
int
ProximClearConsoleCmd::id() const {
    return ProximConstants::COMMAND_CLEAR_CONSOLE;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximClearConsoleCmd*
ProximClearConsoleCmd::create() const {
    return new ProximClearConsoleCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the console window exists.
 */
bool
ProximClearConsoleCmd::isEnabled() {

    wxWindow* window = wxGetApp().GetTopWindow()->FindWindow(
        ProximConstants::ID_CONSOLE_WINDOW);

    if (window == NULL) {
	return false;
    } else {
	return true;
    }
}
