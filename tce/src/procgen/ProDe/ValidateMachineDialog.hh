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
 * @file ValidateMachineDialog.hh
 *
 * Declaration of ValidateMachineDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_VALIDATE_MACHINE_DIALOG_HH
#define TTA_VALIDATE_MACHINE_DIALOG_HH

#include <wx/wx.h>

class wxHtmlWindow;

namespace TTAMachine {
    class Machine;
}

/**
 * Dialog for listing and editing address spaces of the machine.
 */
class ValidateMachineDialog : public wxDialog {
public:
    ValidateMachineDialog(
        wxWindow* parent, const TTAMachine::Machine& machine);
    ~ValidateMachineDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onValidate(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    void onCheck(wxCommandEvent& event);

    /// Machine to be validated.
    const TTAMachine::Machine& machine_;
    /// HTML-widget for validation result texts.
    wxHtmlWindow* resultsWindow_;

    wxCheckBox* checkAnsiC_;
    wxCheckBox* checkGlobalConnReg_;

    // enumerated ids for dialog controls
    enum {
        ID_VALIDATE = 10000,
        ID_CLOSE,
        ID_RESULTS,
        ID_LINE,
        ID_CHECK_ANSI_C,
        ID_CHECK_GLOBAL_CONN_REGISTER
    };

    DECLARE_EVENT_TABLE()
};
#endif
