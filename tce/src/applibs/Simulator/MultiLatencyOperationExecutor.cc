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
 * @file MultiLatencyOperationExecutor.cc
 *
 * Definition of MultiLatencyOperationExecutor class.
 *
 * @author Pekka Jääskeläinen 2008 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include "MultiLatencyOperationExecutor.hh"
#include "Operation.hh"
#include "SequenceTools.hh"
#include "OperationContext.hh"
#include "PortState.hh"
#include "SimulatorToolbox.hh"
#include "OperationPool.hh"
#include "Application.hh"

using std::vector;
using std::string;

/**
 * Constructor.
 *
 * @param parent Parent of the OperationExecutor.
 * @param hwOp The hardware operation to simulate.
 */
MultiLatencyOperationExecutor::MultiLatencyOperationExecutor(
    FUState& parent, TTAMachine::HWOperation& hwOp) :
    OperationExecutor(parent), context_(NULL), hwOperation_(hwOp) {
    hasPendingOperations_ = true;
}

/**
 * Destructor.
 */
MultiLatencyOperationExecutor::~MultiLatencyOperationExecutor() {
}

/**
 * Starts new operation.
 *
 * First original inputs and outputs are stored. Then outputs of the operation
 * are stored. Then operation is triggered.
 *
 * @param op Operation to be triggered.
 * @todo This can be optimized a lot: try to initialize the vector as rarely
 *       as possible.
 */
void
MultiLatencyOperationExecutor::startOperation(Operation& op) {

    const std::size_t inputOperands = op.numberOfInputs();
    const std::size_t outputOperands = op.numberOfOutputs();
    const std::size_t operandCount = inputOperands + outputOperands;

    // set the inputs to point directly to the input ports
    for (std::size_t i = 1; i <= inputOperands; ++i) {
        /// @todo create valueConst() and value() to avoid these uglies
        iovec_[i - 1] = &(const_cast<SimValue&>(binding(i).value()));
    }
    
    // set outputs to point to a value in a result queue
    for (std::size_t i = inputOperands + 1; i <= operandCount; ++i) {
        PortState& port = binding(i);
        const int resultLatency = hwOperation_.latency(i);

        PendingResult* res = new PendingResult(port, resultLatency);

        iovec_[i - 1] = &res->resultValue();
        pendingResults_.push_back(res);
    }
    op.simulateTrigger(iovec_, *context_);
    hasPendingOperations_ = true;
}

/**
 * Advances clock by one cycle.
 *
 * Takes the oldest result in the pipeline and makes it visible in the
 * function unit's ports.
 */
void
MultiLatencyOperationExecutor::advanceClock() {

    // advance clock of all pending results and remove from
    // queue if ready
    for (PendingResultQueue::iterator i = pendingResults_.begin();
         i != pendingResults_.end();) {
        PendingResult* res = *i;
        if (res->advanceCycle()) {
            i = pendingResults_.erase(i);
            delete res;
        } else {
            ++i;
        }
    }
    hasPendingOperations_ = pendingResults_.size() > 0;
}

/**
 * Copies OperationExecutor.
 *
 * @return The copied OperationExecutor.
 */
OperationExecutor*
MultiLatencyOperationExecutor::copy() {
    return new MultiLatencyOperationExecutor(*this);
}

/**
 * Sets the OperationContext for the OperationExecutor.
 *
 * @param context New OperationContext.
 */
void
MultiLatencyOperationExecutor::setContext(OperationContext& context) {
    context_ = &context;
}

