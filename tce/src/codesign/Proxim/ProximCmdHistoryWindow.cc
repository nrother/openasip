/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file ProximCmdHistoryWindow.cc
 *
 * Implementation of ProximCmdHistoryWindow class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <fstream>
#include <wx/statline.h>
#include "ProximCmdHistoryWindow.hh"
#include "ProximLineReader.hh"
#include "WxConversion.hh"
#include "SimulatorEvent.hh"
#include "ErrorDialog.hh"

#if wxCHECK_VERSION(3, 0, 0)
    #define wxSAVE wxFD_SAVE
#endif

BEGIN_EVENT_TABLE(ProximCmdHistoryWindow, ProximSimulatorWindow)
    EVT_SIMULATOR_COMMAND_DONE(0, ProximCmdHistoryWindow::onSimulatorCommand)
    EVT_BUTTON(ID_CLOSE, ProximCmdHistoryWindow::onClose)
    EVT_BUTTON(ID_SAVE, ProximCmdHistoryWindow::onSave)
    EVT_LISTBOX_DCLICK(ID_LIST, ProximCmdHistoryWindow::onCommandDClick)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Proxim main frame.
 * @param lineReader Line reader handling the command history.
 */
ProximCmdHistoryWindow::ProximCmdHistoryWindow(
    ProximMainFrame* parent, wxWindowID id, ProximLineReader& lineReader):
    ProximSimulatorWindow(parent, id),
    lineReader_(lineReader) {

    createContents(this, true, true);
    cmdList_ = dynamic_cast<wxListBox*>(FindWindow(ID_LIST));

    updateCommandList();
}


/**
 * The Destructor.
 */
ProximCmdHistoryWindow::~ProximCmdHistoryWindow() {
}


/**
 * Updates the command history list.
 */
void
ProximCmdHistoryWindow::updateCommandList() {
    cmdList_->Clear();
    for (size_t i = 0; i < lineReader_.inputsInHistory(); i++) {
        std::string command = lineReader_.inputHistoryEntry(i);
        cmdList_->Insert(WxConversion::toWxString(command), 0);
    }
    if (cmdList_->GetCount() > 0) {
        FindWindow(ID_SAVE)->Enable();
    } else {
        FindWindow(ID_SAVE)->Disable();
    }
}

/**
 * Calls history list update when a simulator event of a completed command
 * is received.
 */
void
ProximCmdHistoryWindow::onSimulatorCommand(SimulatorEvent& event) {
    updateCommandList();
    event.Skip();
}


/**
 * Handles the Close button events.
 *
 * Closes the window.
 */
void
ProximCmdHistoryWindow::onClose(wxCommandEvent&) {
    GetParent()->Close();
}

/**
 * Event handler for the command list double clicks.
 *
 * Sends the double clicked command to the linereader input.
 *
 * @param event Event of the item dclick.
 */
void
ProximCmdHistoryWindow::onCommandDClick(wxCommandEvent& event) {
    std::string command = WxConversion::toString(event.GetString());
    lineReader_.input(command);
}

/**
 * Displays dialog for saving command history to a file.
 */
void
ProximCmdHistoryWindow::onSave(wxCommandEvent&) {

    wxString title = _T("Save Command History");
    wxString filters = _T("Command history logs (*.log)|*.log|All files|*.*");
    wxFileDialog dialog(
        this, title, _T(""), _T("commands.log"), filters, wxSAVE);

    if (dialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    std::string filename = WxConversion::toString(dialog.GetPath());

    std::ofstream file(filename.c_str());
    if (file.bad()) {
        wxString message = _T("Error saving file '.");
        message.Append(dialog.GetPath());
        message.Append(_T("'."));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    size_t cmdCount = lineReader_.inputsInHistory();
    for (size_t i = 0; i < cmdCount; i++) {
        std::string command =
            lineReader_.inputHistoryEntry(cmdCount - 1 - i);
        file << command << std::endl;
    }

    file.close();
}


/**
 * Creates the window widgets.
 *
 * Code generated by wxDesigner. Do not edit manually.
 *
 * @param parent Parent window of the window contents.
 * @param call_fit If true, resize the parent window to fit the created
 *                 contents.
 * @param set_sizer If true, sets the created sizer as parent window contents.
 */
wxSizer*
ProximCmdHistoryWindow::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxString *strs1 = (wxString*) NULL;
    wxListBox *item1 = new wxListBox( parent, ID_LIST, wxDefaultPosition, wxSize(300,300), 0, strs1, wxLB_SINGLE );
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticLine *item2 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item3 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item4 = new wxButton( parent, ID_SAVE, wxT("&Save"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item5 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(-1,20), wxLI_VERTICAL );
    item3->Add( item5, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxButton *item6 = new wxButton( parent, ID_CLOSE, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
