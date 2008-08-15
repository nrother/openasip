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
 * Declaration of OldFrontendStackAnalyzer class.
 *
 * This class inserts the annotations to the stack frame initialization
 * and deinitialization code so that it can easily be 
 * removed and recreated later.
 *
 */

#include "Move.hh"
#include "Instruction.hh"
#include "Procedure.hh"
#include "Terminal.hh"
#include "TerminalImmediate.hh"
#include "TerminalRegister.hh"
#include "TerminalFUPort.hh"
#include "POMDisassembler.hh"
#include "Operation.hh"

#include "RegisterFile.hh"

#include "OldFrontendStackAnalyzer.hh"

#include "DataMemory.hh"
#include "AddressSpace.hh"
#include "TCEString.hh"

using namespace TTAProgram;
using namespace TTAMachine;

OldFrontendStackAnalyzer::OldFrontendStackAnalyzer() : 
    stackPointer_(NULL) {
}

OldFrontendStackAnalyzer::~OldFrontendStackAnalyzer() {
    if (stackPointer_ != NULL)
        delete stackPointer_;
}

StackFrameData*
OldFrontendStackAnalyzer::readCrt0Header(TTAProgram::Procedure& proc) 
    throw (IllegalProgram) {
    
    int index = 0;
    int stackInitAddr = 0;

    bool skipExitCall = false;

    StackFrameData* sfd = NULL;
    if (proc.instructionCount() == 15) {
    
        Terminal& t0 = proc.instructionAtIndex(0).move(0).source();
        TerminalImmediate& ti0 = dynamic_cast<TerminalImmediate&>(t0);
        Terminal& t1 = proc.instructionAtIndex(1).move(0).source();
        TerminalImmediate& ti1 = dynamic_cast<TerminalImmediate&>(t1);
        
        stackInitAddr = ti0.value().intValue() + ti1.value().intValue();
        sfd = new StackFrameData(0, stackInitAddr);
        sfd->setCodeSizes(8,5);

        stackPointer_ = 
            proc.instructionAtIndex(2).move(0).destination().copy();

    } else if (proc.instructionCount() == 14) {
        stackInitAddr = 1; // don't change to be 0... this seems to be set later..
        sfd = new StackFrameData(0, stackInitAddr);
        sfd->setCodeSizes(5,5);
        stackPointer_ = 
            proc.instructionAtIndex(0).move(0).source().copy();
        skipExitCall = true;

    } else if (proc.instructionCount() == 12) {

        sfd = new StackFrameData(true);
        sfd->setCodeSizes(5,5);
        stackPointer_ = 
            proc.instructionAtIndex(0).move(0).source().copy();
    } else {
        throw IllegalProgram(
            __FILE__, __LINE__, __func__,
            "crt0() should contain either 12, 14 or 15 instructions. This "
            "should be the case when generating code with the gcc 2.7.0 "
            "frontend r3066 and using optimization flag -O3."
            " Instructiones in crt0: " + Conversion::toString(proc.instructionCount()));
    }

    for (int i = 0; i < sfd->constrCodeSize(); i++) {
        annotateInstruction(
            proc.instructionAtIndex(index++),
            ProgramAnnotation::ANN_STACKFRAME_STACK_INIT, 
            Conversion::toString(stackInitAddr));
    }
    
    index++; //skip the main call
    
    if (skipExitCall) {
        index += 2; //skip exit call
    }

    for (int i = 0; i < 5; i++) {
        annotateInstruction(
            proc.instructionAtIndex(index++),
            ProgramAnnotation::ANN_STACKFRAME_STACK_INIT, 
            Conversion::toString(stackInitAddr));
    }
    
    return sfd;
}

/**
 * Reads the stack generation code from code generated by the old frontend.
 *
 * Analyzes and annotates the stack frame creation and destruction code.
 *
 * @param proc Procedure being read.
 * @return size of stack frame construction code
 */
StackFrameData* 
OldFrontendStackAnalyzer::readProcedureHeader(TTAProgram::Procedure& proc) 
    throw (IllegalProgram) {

    if( proc.name() == "_crt0" ) {
        return readCrt0Header(proc);
    } else {
        return readCommonProcedureHeader(proc);
    }
}

StackFrameData*
OldFrontendStackAnalyzer::readCommonProcedureHeader(
    TTAProgram::Procedure& proc) throw (IllegalProgram) {

    StackFrameData *sfd;
    int index = 0;
    int iCount = proc.instructionCount();
    
    // if stackpoiter wasn't initialized in crt0 (no crt0)
    // it seems to be always first source of first instruction.
    if (stackPointer_ == NULL) {
       stackPointer_ =  proc.instructionAtIndex(0).move(0).source().copy();
    }

    Terminal& ts4 = proc.instructionAtIndex(4).move(0).source();
    
    if (ts4.isImmediate()) { // contains local vars?
        
        Terminal& ts1 = proc.instructionAtIndex(1).move(0).source();
        TerminalImmediate& ti = dynamic_cast<TerminalImmediate&>(ts1);
        
        int localVarSize = -ti.value().intValue();
        sfd = new StackFrameData(localVarSize,0);
        
        std::string payload = //"" + sfd->localVarSize_;
            Conversion::toString(localVarSize);
        while( index < 3 ) {
            
            annotateInstruction(
                proc.instructionAtIndex(index++),
                ProgramAnnotation::ANN_STACKFRAME_LVAR_ALLOC,
                payload);
            annotateInstruction(
                proc.instructionAtIndex(iCount-index-1),
                ProgramAnnotation::ANN_STACKFRAME_LVAR_DEALLOC,
                payload);
        }
//            index-=3; 
    } else { // no local variables
        sfd = new StackFrameData;
    }
    
    for( int i = 0; i < 3; i++ ) {
        annotateInstruction(
            proc.instructionAtIndex(index++),
            ProgramAnnotation::ANN_STACKFRAME_RA_ALLOC,"");
        annotateInstruction(
            proc.instructionAtIndex(iCount-index-1),
            ProgramAnnotation::ANN_STACKFRAME_RA_DEALLOC,"");
    }
    
    for( int i = 0; i < 2; i++ ) {
        annotateInstruction(
            proc.instructionAtIndex(index++),
            ProgramAnnotation::ANN_STACKFRAME_RA_SAVE,"");
        annotateInstruction(
            proc.instructionAtIndex(iCount-index-1),
            ProgramAnnotation::ANN_STACKFRAME_RA_RESTORE,"");
    }
    
    // loop as long as we are reading stack frame initialization
    while (true) {
        // TODO:mark all
        Terminal& nextDst = 
            proc.instructionAtIndex(index+2).move(0).destination();
        if (stackPointer_->equals(nextDst)) {
            Terminal& nextDst2 =
                proc.instructionAtIndex(index+3).move(0).destination();
            
            // push or outgoing parameter space?
            if( stackPointer_->equals(nextDst) && isStoreAddress(nextDst2)) {
                Terminal& nextSrc3 =
                    proc.instructionAtIndex(index+4).move(0).source();
                TerminalRegister* savedReg = 
                    dynamic_cast<TerminalRegister*>(&nextSrc3);
                if( savedReg == NULL ) {
                    throw IllegalProgram(__FILE__,__LINE__,"",
                                         "Broken context save");
                }
                if(savedReg->registerFile().width() == 32 ) {
                    
                    // TODO: push terminal into sfd
//                        sfd->gprSaveCount_++;
//                    sfd.newSavedGPRsCount_++;
                    
                    annotateInstruction(
                        proc.instructionAtIndex(index++),
                        ProgramAnnotation::
                        ANN_STACKFRAME_GPR_SAVE_BEGIN,"");
                    
                    annotateInstruction(
                        proc.instructionAtIndex(iCount-index-1),
                        ProgramAnnotation::
                        ANN_STACKFRAME_GPR_RESTORE,"");
                    
                    for (int i = 0; i < 4; i++) {
                        annotateInstruction(
                            proc.instructionAtIndex(index++),
                            ProgramAnnotation::
                            ANN_STACKFRAME_GPR_SAVE,"");
                        
                        annotateInstruction(
                            proc.instructionAtIndex(iCount-index-1),
                            ProgramAnnotation::
                            ANN_STACKFRAME_GPR_RESTORE,"");
                    }
                    
                    sfd->addRegisterSave(
                        proc.instructionAtIndex(index-1).move(0).source());
                    
                } else {
                    /* Code to save floating point register */
                    if (savedReg->registerFile().width() == 64 ) {
                        
                        // TODO: push terminal into sfd
//                            sfd->fprSaveCount_++;
//                        sfd.newSavedFPRsCount_++;

                        annotateInstruction(
                            proc.instructionAtIndex(index++),
                            ProgramAnnotation::
                            ANN_STACKFRAME_FPR_SAVE_BEGIN,"");

                        annotateInstruction(
                            proc.instructionAtIndex(iCount-index-1),
                            ProgramAnnotation::
                            ANN_STACKFRAME_FPR_RESTORE,"");

                        for( int i = 0; i < 4; i++ ) {
                            annotateInstruction(
                                proc.instructionAtIndex(index++),
                                ProgramAnnotation::
                                ANN_STACKFRAME_FPR_SAVE,"");

                            annotateInstruction(
                                proc.instructionAtIndex(iCount-index-1),
                                ProgramAnnotation::
                                ANN_STACKFRAME_FPR_RESTORE,"");
                        }
                    } else {
                        throw IllegalProgram(__FILE__,__LINE__,"",
                                             "Invalid reg type context save");
                    }
                }

            } else {
                /* Code to allocate space for output parameters */
                Terminal& prevSrc = 
                    proc.instructionAtIndex(index+1).move(0).source(); 
                if (prevSrc.isImmediate()) {
                    TerminalImmediate& ti = 
                        dynamic_cast<TerminalImmediate&>(prevSrc);
                    sfd->setOutputParamsSize(-ti.value().intValue());
                        
                    for (int i = 0; i < 3; i++) {
                        annotateInstruction(
                            proc.instructionAtIndex(index++),
                            ProgramAnnotation::ANN_STACKFRAME_OUT_PARAM_ALLOC,
                            Conversion::toString(sfd->outputParamsSize()));
                        
                        annotateInstruction(
                            proc.instructionAtIndex(iCount-index-1),
                            ProgramAnnotation::ANN_STACKFRAME_OUT_PARAM_DEALLOC
                            , Conversion::toString(sfd->outputParamsSize()));
                    }

                    sfd->setCodeSizes(index,index);
                    return sfd;
                }
                throw IllegalProgram(__FILE__,__LINE__,"",
                                     "Invalid stack frame code");
            }
        } 
        else {
            sfd->setCodeSizes(index,index/*+1*/);
            
            return sfd;
        }
    }
}

/**
 * Finds and annotates all references to stack from the function body
 *
 * @param proc Procedure whose stack references are being processed.
 * @param sfSize Size of stack frame construction code.
 */
void OldFrontendStackAnalyzer::annotateStackOffsets(
    TTAProgram::Procedure& proc, const StackFrameData& sfd) 
    throw (IllegalProgram) {

    assert(stackPointer_ != NULL);
    
    for (int index = sfd.constrCodeSize(); 
         index < proc.instructionCount()-sfd.deconstrCodeSize();
         index++) {
        Instruction& ins = proc.instructionAtIndex(index);
        if( ins.moveCount() != 1 ) {
            throw IllegalProgram(__FILE__,__LINE__,"",
                                 "not exactly one move in instruction"+
                                 POMDisassembler::disassemble(ins));
        }
        Move& move = ins.move(0);

        if (move.source().equals(*stackPointer_)) {
            if (move.destination().isFUPort()) {
                TerminalFUPort& tfu = 
                    dynamic_cast<TerminalFUPort&>(move.destination());
                if( tfu.hintOperation().name() == "ADD" ) {
                    Instruction& ins2 = proc.instructionAtIndex(index+1);
                    Move& move2 = ins2.move(0);
                    if(move2.source().isImmediate()) {
                        
                        annotateStackUsage(move2, sfd);
                        
                    } else {
                        throw IllegalProgram(
                            __FILE__,__LINE__,"", 
                            std::string("SP+not_immediate: addr: ")+
                            Conversion::toString(ins2.address().location()) +
                            std::string("Code: " ) +
                            POMDisassembler::disassemble(move) +
                            std::string(";") +
                            POMDisassembler::disassemble(move2));
                    }
                }  else {
                    if( !tfu.hintOperation().usesMemory()) {
                        throw IllegalProgram(
                            __FILE__,__LINE__,"",
                            std::string(
                                "SP to some other op than add or mem op addr:")
                            + Conversion::toString(ins.address().location()) + 
                            std::string("Code: " ) +
                            POMDisassembler::disassemble(move));
                    } 
                }
            } else {
                /*
                  // TODO: hjandle special case index == 0
                throw IllegalProgram(
                    __FILE__,__LINE__,"", 
                    std::string("SP to not_FU, addr: ")+
                    Conversion::toString(ins.address().location()) +
                    std::string("Code: " ) +
                    POMDisassembler::disassemble(move));
                */
            }
        }
    }
}

/** 
 * Analyzes stack offset of one SP-offset-containing immediate move,
 * and adds annotation which tells what kind of stack reference it is.
 *
 * @param move Move whose source is SP-offset immediate.
 * @sfd Details about stack frame structure of current procedure.
 */
void 
OldFrontendStackAnalyzer::annotateStackUsage(
    Move& move, const StackFrameData& sfd ) {
    TerminalImmediate& ti = 
        dynamic_cast<TerminalImmediate&>(move.source());
    int spOffset = ti.value().intValue();
    
    // TODO: stack offset analysis

    int oldBorder = 0;
    int border = sfd.outputParamsSize();
    if( spOffset < border ) {
        
        move.addAnnotation(
            ProgramAnnotation(
                ProgramAnnotation::ANN_STACKUSE_OUT_PARAM,
                Conversion::toString(spOffset)));
        return;
    } 
    oldBorder = border;
    border += (sfd.gprSaveCount() << 2 );
    if( spOffset < border ) {
        
        move.addAnnotation(
            ProgramAnnotation(
                ProgramAnnotation::ANN_STACKUSE_SAVED_GPR,
                Conversion::toString(spOffset-oldBorder)));
        return;
    } 
    oldBorder = border;
    border += 4;
    if( spOffset < border ) {

        move.addAnnotation(
            ProgramAnnotation(
                ProgramAnnotation::ANN_STACKUSE_RA,""));

        return;
    }
    oldBorder = border;
    border += sfd.localVarSize();
    if( spOffset < border ) {

        move.addAnnotation(
            ProgramAnnotation(
                ProgramAnnotation::ANN_STACKUSE_LOCAL_VARIABLE,
                Conversion::toString(spOffset-oldBorder)));
        return;
    }

    move.addAnnotation(
        ProgramAnnotation(
            ProgramAnnotation::ANN_STACKUSE_IN_PARAM,
            Conversion::toString(spOffset-border)));
    
}


/**
 * Creates an annotation and adds that to the given instruction
 *
 * @param ins Instruction containg move where to insert the annotation.
 * @param Id id of the annotation being added.
 * @param payload Annotation payload
*/
void 
OldFrontendStackAnalyzer::annotateInstruction(
    Instruction&ins, ProgramAnnotation::Id id, const std::string& payload) 
    throw (IllegalProgram) {

    if (!ins.moveCount()) {
        throw IllegalProgram(
            __FILE__, __LINE__, __func__, "Sequential NOP");
    }
    ProgramAnnotation annotation(id,payload);
    ins.move(0).addAnnotation(annotation);
}


bool
OldFrontendStackAnalyzer::isStoreAddress(Terminal& terminal) {

    if (terminal.isFUPort()) {
        TerminalFUPort& tfup = dynamic_cast<TerminalFUPort&>(terminal);
        if(!tfup.isOpcodeSetting() && tfup.hintOperation().writesMemory()) {
            return true;
        }
    }
    return false;
}

void 
OldFrontendStackAnalyzer::analyzeAndAnnotateProcedure(
    TTAProgram::Procedure& proc) throw (IllegalProgram){
    StackFrameData* sfd = readProcedureHeader(proc);
    annotateStackOffsets(proc, *sfd);
    delete sfd;
}
