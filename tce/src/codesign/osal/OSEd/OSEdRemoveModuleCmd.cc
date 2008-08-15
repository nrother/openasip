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
 * @file OSEdRemoveModuleCmd.cc
 *
 * Definition of OSEdRemoveModuleCmd class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <iostream>

#include "OSEdRemoveModuleCmd.hh"
#include "OSEdConstants.hh"
#include "ConfirmDialog.hh"
#include "OSEdTextGenerator.hh"
#include "OperationContainer.hh"
#include "WxConversion.hh"
#include "FileSystem.hh"
#include "Application.hh"
#include "OperationModule.hh"
#include "OSEd.hh"
#include "OSEdTreeView.hh"
#include "OSEdInfoView.hh"
#include "OperationIndex.hh"

using std::string;
using boost::format;
using std::cout;
using std::endl;

/**
 * Constructor.
 */
OSEdRemoveModuleCmd::OSEdRemoveModuleCmd() :
    GUICommand(OSEdConstants::CMD_NAME_REMOVE_MODULE, NULL) {
}

/**
 * Destructor.
 */
OSEdRemoveModuleCmd::~OSEdRemoveModuleCmd() {
}

/**
 * Returns the id of the command.
 * 
 * @return Id of the command.
 */
int
OSEdRemoveModuleCmd::id() const {
    return OSEdConstants::CMD_REMOVE_MODULE;
}

/**
 * Returns a new command.
 *
 * @return A new command.
 */
GUICommand*
OSEdRemoveModuleCmd::create() const {
    return new OSEdRemoveModuleCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdRemoveModuleCmd::Do() {
    
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    string modName = treeView->selectedModule();
    wxTreeItemId modId = treeView->selectedModuleId();
    string path = treeView->pathOfModule(modId);
    
    OperationModule& module = OperationContainer::module(path, modName);

    format fmt = texts.text(OSEdTextGenerator::TXT_QUESTION_REMOVE_MODULE);
    fmt % modName;
    ConfirmDialog dialog(parentWindow(), WxConversion::toWxString(fmt.str()));
    
    if (dialog.ShowModal() == wxID_YES) {
        bool removed = 
            FileSystem::removeFileOrDirectory(module.propertiesModule());
        assert(removed == true);

        if (module.hasBehaviorSource()) {
            fmt = texts.text(
                OSEdTextGenerator::TXT_QUESTION_REMOVE_BEHAVIOR_FILE);
            
            fmt % FileSystem::fileOfPath(module.behaviorSourceModule());
            
            wxString confText = WxConversion::toWxString(fmt.str());
            ConfirmDialog confirm(parentWindow(), confText);
        
            if (confirm.ShowModal() == wxID_YES) {
                FileSystem::removeFileOrDirectory(module.behaviorSourceModule());
            }
        }

        if (module.definesBehavior()) {
            FileSystem::removeFileOrDirectory(module.behaviorModule());
        }
        
        OperationIndex& index = OperationContainer::operationIndex();
        index.removeModule(path, modName);
        treeView->removeItem(modId);
        treeView->infoView()->moduleView(path);
    }
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * @return True if command is enabled.
 */
bool
OSEdRemoveModuleCmd::isEnabled() {
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    if (treeView->isModuleSelected()) {
        wxTreeItemId modId = treeView->selectedModuleId();
        string path = treeView->pathOfModule(modId);
        if (path != "") {
            return FileSystem::fileIsWritable(path);
        } else {
            return false;
        }
    } else {
        return false;
    }

    assert(false);
    return false;
}

/**
 * Returns icon path.
 *
 * @return Empty string (no icons used).
 */
string
OSEdRemoveModuleCmd::icon() const {
    return "";
}
