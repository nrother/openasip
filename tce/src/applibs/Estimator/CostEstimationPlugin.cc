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
 * @file CostEstimationPlugin.cc
 *
 * Implementation of CostEstimationPlugin class
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 *
 * @note rating: red
 */

#include "CostEstimationPlugin.hh"

namespace CostEstimator {

/**
 * Constructor.
 *
 * @param name The name of the cost estimation plugin in the HDB.
 * @param dataSource The HDB that stores the cost estimation data of
 *                   the plugin. This reference can also be used to add new 
 *                   data, in case required by the algorithm.
 */
CostEstimationPlugin::CostEstimationPlugin(
    const std::string& name) :    
    name_(name) {
}

/**
 * Destructor.
 */
CostEstimationPlugin::~CostEstimationPlugin() {
}

/**
 * Returns the name of this cost estimation plugin.
 *
 * @return The name of the plugin.
 */
std::string
CostEstimationPlugin::name() const {
    return name_;
}

/**
 * Returns the description of this cost estimation plugin.
 *
 * @return The default description is empty.
 */
std::string
CostEstimationPlugin::description() const {
    return "";
}

}
