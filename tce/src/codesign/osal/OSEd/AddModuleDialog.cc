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
 * @file AddModuleDialog.cc
 *
 * Definition of AddModuleDialog class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <boost/regex.hpp>

#include "AddModuleDialog.hh"
#include "ErrorDialog.hh"
#include "WxConversion.hh"
#include "GUITextGenerator.hh"
#include "OSEdTextGenerator.hh"
#include "WidgetTools.hh"
#include "DialogPosition.hh"
#include "OperationContainer.hh"
#include "OperationModule.hh"

using std::string;
using boost::format;

BEGIN_EVENT_TABLE(AddModuleDialog, wxDialog)
	EVT_BUTTON(wxID_OK, AddModuleDialog::onOk)
    EVT_TEXT_ENTER(ID_MODULE_NAME, AddModuleDialog::onOk)
END_EVENT_TABLE()

/**
 * Constructor.
 *
 * @parent Parent window.
 */
AddModuleDialog::AddModuleDialog(wxWindow* parent, string path) : 
    wxDialog(parent, -1, _T(""), 
             DialogPosition::getPosition(DialogPosition::DIALOG_ADD_MODULE)),
	path_(path), name_(_T("")){

    createContents(this, true, true);
    FindWindow(ID_MODULE_NAME)->
        SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));

    FindWindow(wxID_OK)->SetFocus();
    setTexts();
}

/**
 * Destructor.
 */
AddModuleDialog::~AddModuleDialog() {
    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_ADD_MODULE, point);
}

/**
 * Set texts to widgets.
 */
void
AddModuleDialog::setTexts() {
	
    GUITextGenerator& guiText = *GUITextGenerator::instance();
    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();

    //title
    format fmt = osedText.text(OSEdTextGenerator::TXT_ADD_MODULE_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    WidgetTools::setLabel(&osedText, FindWindow(ID_TEXT),
                          OSEdTextGenerator::TXT_LABEL_MODULE_NAME);

    // buttons
    WidgetTools::setLabel(&guiText, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(&guiText, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);
}

/**
 * Handles the event when OK button is pushed.
 */
void
AddModuleDialog::onOk(wxCommandEvent&) {
    TransferDataFromWindow();
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    if (name_ == _T("")) {
        format fmt = texts.text(OSEdTextGenerator::TXT_ERROR_NO_NAME);
        fmt % "operation module";
        ErrorDialog dialog(this, WxConversion::toWxString(fmt.str()));
        dialog.ShowModal();
        return;
    } else {
        string modName = WxConversion::toString(name_);
        // let's check that module name is legal
        const char* regExp = "\\w+";
        boost::regex expression(regExp);
        boost::match_results<string::const_iterator> what;
        if (!boost::regex_match(modName, what, expression)) {
            format fmt = texts.text(OSEdTextGenerator::TXT_ERROR_MOD_NAME);
            fmt % modName;
            ErrorDialog dialog(this, WxConversion::toWxString(fmt.str()));
            dialog.ShowModal();
            return;
        }
        
        // let's check there is not already a module by that name
        if (&OperationContainer::module(path_, modName) !=
            &NullOperationModule::instance()) {
            
            format fmt = 
                texts.text(OSEdTextGenerator::TXT_ERROR_MODULE_EXISTS);
            fmt % modName;
            string msg = fmt.str();
            ErrorDialog error(this, WxConversion::toWxString(msg));
            error.ShowModal();
        } else {
            EndModal(wxID_OK);
        }
    }
}

/**
 * Returns the name of the added module.
 *
 * @return The name of the added module.
 */
string
AddModuleDialog::name() const {
    return WxConversion::toString(name_);
}

/**
 * Creates the contents of the dialog.
 *
 * @param parent Parent window.
 * @param call_fit If true fits the contents inside the dialog.
 * @param set_sizer If true sets the main sizer as the contents of the dialog.
 * @return The created sizer.
 */
wxSizer*
AddModuleDialog::createContents(
    wxWindow *parent, 
    bool call_fit, 
    bool set_sizer ) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item2 = new wxStaticText( parent, ID_TEXT, wxT("Name of the module:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_MODULE_NAME, wxT(""), wxDefaultPosition, wxSize(80,-1), wxTE_PROCESS_ENTER );
    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item4 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item5 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item6 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
