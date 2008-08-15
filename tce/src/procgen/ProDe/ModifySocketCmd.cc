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
 * @file ModifySocketCmd.cc
 *
 * Definition of ModifySocketCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "ModifySocketCmd.hh"
#include "Socket.hh"
#include "EditPart.hh"
#include "SocketDialog.hh"
#include "ErrorDialog.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ModifySocketCmd::ModifySocketCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {

}


/**
 * The Destructor.
 */
ModifySocketCmd::~ModifySocketCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifySocketCmd::Do() {

    Socket* socket = dynamic_cast<Socket*>(editPart_->model());
    assert (socket != NULL);

    assert (parentWindow() != NULL);

    SocketDialog dialog(parentWindow(), socket);

    if (dialog.ShowModal() == wxID_OK) {
	// socket was modified
	return true;
    } else {
	// modification was cancelled
	return false;
    }

    return false;
}
