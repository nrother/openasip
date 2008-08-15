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
 * @file BridgeFigure.cc
 *
 * Definition of BridgeFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include "BridgeFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour BridgeFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour BridgeFigure::DEFAULT_BG_COLOUR = wxColour(150, 200, 255);

/**
 * The Constructor.
 */
BridgeFigure::BridgeFigure():
    ConnectionFigure(), direction_(BridgeFigure::DIR_RIGHT) {
    size_ = wxSize(
	MachineCanvasLayoutConstraints::BRIDGE_WIDTH,
	MachineCanvasLayoutConstraints::BRIDGE_HEIGHT);
}

/**
 * The Destructor.
 */
BridgeFigure::~BridgeFigure() {
}

/**
 * Draws the bridge's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
BridgeFigure::drawSelf(wxDC* dc) {

    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    dc->SetBrush(brush);

    const unsigned int triangleWidth = 200;
    
    wxPoint trianglePoints[3];

    if (direction_ == DIR_RIGHT) {
	trianglePoints[0] = wxPoint(
	    location_.x + size_.GetWidth() / 2 - triangleWidth / 2,
	    location_.y );
	trianglePoints[1] = wxPoint(
	    trianglePoints[0].x + triangleWidth, location_.y);
	trianglePoints[2] = wxPoint(
	    trianglePoints[0].x + triangleWidth / 2,
	    location_.y + size_.GetHeight());

    } else {
	trianglePoints[0] = wxPoint(
	    location_.x + size_.GetWidth() / 2 - triangleWidth / 2,
	    location_.y + size_.GetHeight());
	trianglePoints[1] = wxPoint(
	    trianglePoints[0].x + triangleWidth, trianglePoints[0].y);
	trianglePoints[2] = wxPoint(
	    location_.x + size_.GetWidth() / 2, location_.y);
    }

    dc->DrawPolygon(3, trianglePoints);
}
