/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file TCETargeMachine.cpp
 *
 * Declaration of TCETargetMachine class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam—tut.fi)
 */

#include "llvm/PassManager.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Target/TargetRegistry.h"

#include "TCETargetMachine.hh"
#include "TCETargetAsmInfo.hh"
#include "LLVMPOMBuilder.hh"
#include "PluginTools.hh"
#include "FileSystem.hh"
#include "ADFSerializer.hh"

#include <iostream>

using namespace llvm;


namespace llvm {
    Target TheTCETarget;
}

extern "C" void LLVMInitializeTCETargetInfo() { 
    RegisterTarget<Triple::tce> X(TheTCETarget, "tce", "TTA Codesign Environment");
    RegisterTargetMachine<TCETargetMachine> Y(TheTCETarget);
    RegisterAsmInfo<TCETargetAsmInfo> Z(TheTCETarget);
}

//
// Data layout:
//--------------
// E-p:32:32:32-i1:8:8-i8:8:8-i32:32:32-i64:32:64-f32:32:32-f64:32:64"
// E = Big endian data
// -p:32:32:32 = Pointer size 32 bits, api & preferred alignment 32 bits.
// -i8:8:8 = 8bit integer api & preferred alignment 8 bits.
// etc.
// 
//  Frame info:
// -------------
// Grows down, alignment 4 bytes.
//
TCETargetMachine::TCETargetMachine(const Target &T, const std::string &TT,
                                   const std::string &FS)
    : LLVMTargetMachine(T,TT),
      Subtarget(TT,FS),
      DataLayout(
        "E-p:32:32:32"
        "-a0:32:32"
        "-i1:8:8"
        "-i8:8:8"
        "-i16:16:16"
        "-i32:32:32"
        "-i64:32:64"
        "-f32:32:32"
        "-f64:32:64"),
      FrameInfo(TargetFrameInfo::StackGrowsDown, 4, -4),
      plugin_(NULL), pluginTool_(NULL) {
}

/**
 * The Destructor.
 */
TCETargetMachine::~TCETargetMachine() {
    if (pluginTool_ != NULL) {
        delete pluginTool_;
        pluginTool_ = NULL;
    }
}

/**
 * InsertBranch caused some bad results.. so disable need for that.
bool 
TCETargetMachine::getEnableTailMergeDefault() const { 
    return false; 
}
 */

void 
TCETargetMachine::setTargetMachinePlugin(TCETargetMachinePlugin& plugin) {

    plugin_ = &plugin;
    missingOps_.clear();
    if (!plugin_->hasSDIV()) missingOps_.insert(std::make_pair(llvm::ISD::SDIV, MVT::i32));
    if (!plugin_->hasUDIV()) missingOps_.insert(std::make_pair(llvm::ISD::UDIV, MVT::i32));
    if (!plugin_->hasSREM()) missingOps_.insert(std::make_pair(llvm::ISD::SREM, MVT::i32));
    if (!plugin_->hasUREM()) missingOps_.insert(std::make_pair(llvm::ISD::UREM, MVT::i32));
    if (!plugin_->hasMUL()) missingOps_.insert(std::make_pair(llvm::ISD::MUL, MVT::i32));
    if (!plugin_->hasROTL()) missingOps_.insert(std::make_pair(llvm::ISD::ROTL, MVT::i32));
    if (!plugin_->hasROTR()) missingOps_.insert(std::make_pair(llvm::ISD::ROTR, MVT::i32));

    if (!plugin_->hasSXHW()) missingOps_.insert(
        std::make_pair(llvm::ISD::SIGN_EXTEND_INREG, MVT::i16));

    if (!plugin_->hasSXQW()) missingOps_.insert(
	std::make_pair(llvm::ISD::SIGN_EXTEND_INREG, MVT::i8));

    // register machine to plugin
    plugin_->registerTargetMachine(*this);
}

/**
 * Creates an instruction selector instance.
 *
 * @param pm Function pass manager to add isel pass.
 * @param fast Not used.
 */                                     
bool
TCETargetMachine::addInstSelector(
    PassManagerBase& pm, CodeGenOpt::Level /* OptLevel */) {
    pm.add(plugin_->createISelPass(this));
    return false;
}

/**
 * Creates a TTAMachine::Machine object of the target architecture.
 */
TTAMachine::Machine*
TCETargetMachine::createMachine() {
    ADFSerializer serializer;
    serializer.setSourceString(*plugin_->adfXML());
    return serializer.readMachine();
}

/**
 * Returns list of llvm::ISD SelectionDAG opcodes for operations that are not
 * supported in the target architecture.
 *
 * The returned operations have to be expanded to emulation fucntion calls
 * or emulation patterns in TCETargetLowering.
 */
const std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> >*
TCETargetMachine::missingOperations() {
    return &missingOps_;
}
