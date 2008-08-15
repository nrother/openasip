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
 * @file CollisionMatrix.cc
 *
 * Definition of CollisionMatrix class.
 *
 * Matrix' value is 1 if and only if the operation represented by the row
 * cannot be started after the cycle number represented by the column number.
 * That is, M[O,I] = 1 in case operation O cannot be started I cycles after, 0 
 * otherwise.
 *
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include "CollisionMatrix.hh"
#include "Application.hh"
#include "FunctionUnit.hh"
#include "PipelineElement.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "Conversion.hh"
#include "ResourceVector.hh"
#include "ResourceVectorSet.hh"
#include "ReservationTable.hh"
#include "FUReservationTableIndex.hh"

/**
 * Builds a collision matrix out of a set of reservation tables and an index
 * that points to a row in the index. 
 *
 * The first column is included, that is, it is possible to test whether 
 * an operation can be issued at the same cycle than other operation. This 
 * is useful for the scheduler.
 *
 * @param reservationTables Reservation tables of all operations in the FU.
 * @param index Index of reservation table we compute the matrix for.
 *
 */
CollisionMatrix::CollisionMatrix(
    FUReservationTableIndex& reservationTables, 
    std::size_t index) : 
    BitMatrix(
        reservationTables.at(index).columnCount(), 
        reservationTables.size(), false) {

    ReservationTable& thisOperation = reservationTables.at(index);
    
    for (int row = 0; row < rowCount(); ++row) {
        const ReservationTable& other = reservationTables.at(row);

        for (int column = 0; column < columnCount(); ++column) {
            BitMatrix::setBit(
                column, row, thisOperation.conflictsWith(other, column));
        }
    }
}

/**
 * Constructor.
 *
 * Builds a collision matrix with given dimensions and with each value the
 * same.
 */
CollisionMatrix::CollisionMatrix(
    std::size_t width, std::size_t height, bool value = false) :
    BitMatrix(width, height, value) {
}

/**
 * Destructor.
 */
CollisionMatrix::~CollisionMatrix() {
}

/**
 * Returns the value of the collision matrix at the given location.
 *
 * @param row The row number (operation).
 * @param column The column number (cycle).
 * @return The value.
 */
bool 
CollisionMatrix::isCollision(std::size_t row, std::size_t column) const {
    return BitMatrix::bitAt(column, row);
}

