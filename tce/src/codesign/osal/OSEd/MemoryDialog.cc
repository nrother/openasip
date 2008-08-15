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
 * @file MemoryTools.cc
 *
 * Definition of MemoryTools class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <string>

#include "MemoryDialog.hh"
#include "OSEdTextGenerator.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "DialogPosition.hh"
#include "OperationContainer.hh"
#include "Conversion.hh"
#include "MemoryControl.hh"
#include "OSEd.hh"

using boost::format;
using std::string;

BEGIN_EVENT_TABLE(MemoryDialog, wxDialog)
    EVT_BUTTON(ID_BUTTON_CLOSE, MemoryDialog::onClose)
END_EVENT_TABLE() 

/**
 * Constructor.
 *
 * @param window Parent window.
 */
MemoryDialog::MemoryDialog(wxWindow* window) : 
    wxDialog(window, -1, _T(""),
             DialogPosition::getPosition(DialogPosition::DIALOG_MEMORY),
             wxDefaultSize, wxRESIZE_BORDER) {
    
    createContents(this, true, true);
    setTexts();

    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->registerListener(OSEdInformer::EVENT_MEMORY, this);
}

/**
 * Destructor.
 */
MemoryDialog::~MemoryDialog() {
    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_MEMORY, point);
}

/**
 * Set texts to widgets.
 */
void
MemoryDialog::setTexts() {
    
    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();

    // title
    format fmt = osedText.text(OSEdTextGenerator::TXT_MEMORY_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(&osedText, FindWindow(ID_BUTTON_CLOSE),
                          OSEdTextGenerator::TXT_BUTTON_CLOSE);
}

/**
 * Handles the event when dialog is closed.
 */
void
MemoryDialog::onClose(wxCommandEvent&) {
    OSEdInformer* informer = wxGetApp().mainFrame()->informer();
    informer->unregisterListener(OSEdInformer::EVENT_MEMORY, this);
    Close();
}

/**
 * Handles event when the contents of the memory might have changed.
 */
void
MemoryDialog::handleEvent(OSEdInformer::EventId event) {
    switch(event) {
    case OSEdInformer::EVENT_MEMORY:
        memoryWindow_->updateView();
        break;
    default:
        break;
    }
}

/**
 * Creates the contents of the MemoryDialog.
 *
 * @param parent Parent window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the the main sizer as dialog contents. 
 * @return The created sizer.
 */
wxSizer*
MemoryDialog::createContents(
    wxWindow* parent,
    bool call_fit,
    bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
 
    memoryWindow_ = new MemoryControl(
        parent, &OperationContainer::memory(), MemoryDialog::ID_CONTROL_MEMORY);

    item0->Add(memoryWindow_, 1, wxGROW|wxALIGN_CENTER|wxALL, 5);

    wxButton *item1 = new wxButton( parent, ID_BUTTON_CLOSE, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    
    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;

}
