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
 * @file AddGCUCmd.cc
 *
 * Definition of AddGCUCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/docview.h>
#include <boost/format.hpp>

#include "AddGCUCmd.hh"
#include "GCUDialog.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "InformationDialog.hh"
#include "Machine.hh"
#include "ControlUnit.hh"
#include "ProDeTextGenerator.hh"
#include "WxConversion.hh"
#include "ModelConstants.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "FUPort.hh"
#include "SpecialRegisterPort.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

const std::string AddGCUCmd::RA_PORT_NAME = "ra";
const std::string AddGCUCmd::OP_PORT_NAME = "pc";
const std::string AddGCUCmd::OPNAME_JUMP = "jump";
const std::string AddGCUCmd::OPNAME_CALL = "call";

/**
 * The Constructor.
 */
AddGCUCmd::AddGCUCmd() :
    EditorCommand(ProDeConstants::CMD_NAME_ADD_GCU) {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
AddGCUCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();


    // check that the machine doesn't have a global control unit yet
    if (model->getMachine()->controlUnit() != NULL) {
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        format fmt = generator->text(ProDeTextGenerator::MSG_ERROR_ONE_GCU);
        string title = fmt.str();
        wxString message = WxConversion::toWxString(title);
        InformationDialog info(parentWindow(), message);
        info.ShowModal();
        return false;
    }

    model->pushToStack();

    // Add default ports.
    ControlUnit* gcu = new ControlUnit(
        ProDeConstants::COMP_DEFAULT_NAME_GCU, 3, 1);

    FUPort* opPort = new FUPort(OP_PORT_NAME, 32, *gcu, true, true);
    SpecialRegisterPort* raPort =
        new SpecialRegisterPort(RA_PORT_NAME, 32, *gcu);

    gcu->setReturnAddressPort(*raPort);

    // Add default operations.
    HWOperation* jump = new HWOperation(OPNAME_JUMP, *gcu);
    jump->bindPort(1, *opPort);
    jump->pipeline()->addPortRead(1, 0, 1);

    HWOperation* call = new HWOperation(OPNAME_CALL, *gcu);
    call->bindPort(1, *opPort);
    call->pipeline()->addPortRead(1, 0, 1);

    gcu->setMachine(*(model->getMachine()));

    GCUDialog dialog(parentWindow(), gcu);
    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
	return true;
    } else {
	model->popFromStack();
	return false;
    }

    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
AddGCUCmd::id() const {
    return ProDeConstants::COMMAND_ADD_GCU;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddGCUCmd*
AddGCUCmd::create() const {
    return new AddGCUCmd();
}



/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
AddGCUCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_GCU;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddGCUCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
