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
 * @file ProGeUI.cc
 *
 * Implementation of ProGeUI class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @author Esa Määttä 2007 (esa.maatta@tut.fi)
 * @note rating: red
 */

#include <fstream>
#include <string>
#include <vector>

#include "ProGeUI.hh"
#include "ProcessorGenerator.hh"

#include "Machine.hh"
#include "ADFSerializer.hh"

#include "BinaryEncoding.hh"
#include "BEMSerializer.hh"
#include "BEMGenerator.hh"
#include "BEMValidator.hh"

#include "MachineImplementation.hh"
#include "IDFSerializer.hh"
#include "IDFValidator.hh"

#include "ProcessorConfigurationFile.hh"
#include "Environment.hh"
#include "FileSystem.hh"

#include "ProGeScriptGenerator.hh"
#include "ProGeTestBenchGenerator.hh"

using namespace IDF;
using std::string;
using std::vector;

namespace ProGe {

/**
 * The constructor.
 */
ProGeUI::ProGeUI() :
    machine_(NULL), bem_(NULL), idf_(NULL), plugin_(NULL), pluginFile_("") {
}


/**
 * The destructor.
 */
ProGeUI::~ProGeUI() {
    if (machine_ != NULL) {
        delete machine_;
    }
    if (bem_ != NULL) {
        delete bem_;
    }
    if (idf_ != NULL) {
        delete idf_;
    }
    if (plugin_ != NULL) {
        delete plugin_;
    }
}


/**
 * Loads machine from the given ADF file.
 *
 * @param adfFile The ADF file.
 * @exception SerializerException If the file cannot be read or is erroneous.
 * @exception ObjectStateLoadingException If the machine cannot be
 *                                        constructed.
 */
void
ProGeUI::loadMachine(const std::string& adfFile)
    throw (SerializerException, ObjectStateLoadingException) {

    ADFSerializer serializer;
    serializer.setSourceFile(adfFile);
    machine_ = serializer.readMachine();
}


/**
 * Loads the binary encoding from the given BEM file.
 *
 * @param bemFile The BEM file.
 * @exception SerializerException If the file cannot be read or is erroneous.
 * @exception ObjectStateLoadingException If the object model cannot be
 *                                        constructed.
 */
void
ProGeUI::loadBinaryEncoding(const std::string& bemFile)
    throw (SerializerException, ObjectStateLoadingException) {

    BEMSerializer serializer;
    serializer.setSourceFile(bemFile);
    bem_ = serializer.readBinaryEncoding();
}


/**
 * Loads the machine implementation from the given IDF file.
 *
 * @param idfFile The IDF file.
 * @exception SerializerException If the file cannot be read or is erroneous.
 * @exception ObjectStateLoadingException If the object model cannot be
 *                                        constructed.
 */
void
ProGeUI::loadMachineImplementation(const std::string& idfFile)
    throw (SerializerException, ObjectStateLoadingException) {

    IDFSerializer serializer;
    serializer.setSourceFile(idfFile);
    idf_ = serializer.readMachineImplementation();
}


/**
 * Loads the given processor configuration.
 *
 * @param configurationFile The PCF file.
 * @exception UnreachableStream If the PCF file cannot be read.
 * @exception SerializerException If some files defined in PCF cannot be
 *                                read or is erroneous.
 * @exception ObjectStateLoadingException If some of the object models cannot
 *                                        be constructed.
 */
void
ProGeUI::loadProcessorConfiguration(const std::string& configurationFile)
    throw (UnreachableStream, SerializerException,
           ObjectStateLoadingException) {

    std::ifstream fileStream(configurationFile.c_str());
    if (!fileStream.good()) {
        string errorMsg = "Unable to read the PCF from '" +
            configurationFile + "'.";
        throw UnreachableStream(__FILE__, __LINE__, __func__, errorMsg);
    }

    ProcessorConfigurationFile pcf(fileStream);
    pcf.setPCFDirectory(FileSystem::directoryOfPath(configurationFile));

    // load machine
    try {
        string adfFile = pcf.architectureName();
        ADFSerializer serializer;
        serializer.setSourceFile(adfFile);
        machine_ = serializer.readMachine();
    } catch (const KeyNotFound&) {
    }

    // load BEM
    try {
        string bemFile = pcf.encodingMapName();
        BEMSerializer serializer;
        serializer.setSourceFile(bemFile);
        bem_ = serializer.readBinaryEncoding();
    } catch (const KeyNotFound&) {
    }

    // load IDF
    try {
        string idfFile = pcf.implementationName();
        IDFSerializer serializer;
        serializer.setSourceFile(idfFile);
        idf_ = serializer.readMachineImplementation();
    } catch (const KeyNotFound&) {
    }
}


/**
 * Loads the given IC/decoder generator plugin.
 *
 * @param pluginFile The file that implements the plugin.
 * @param pluginName Name of the plugin.
 * @exception FileNotFound If the module is not found.
 * @exception DynamicLibraryException If the module cannot be opened.
 * @exception InvalidData If the machine or BEM is not loaded yet.
 */
void
ProGeUI::loadICDecoderGeneratorPlugin(
    const std::string& pluginFile,
    const std::string& pluginName)
    throw (FileNotFound, DynamicLibraryException, InvalidData) {

    pluginFile_ = pluginFile;

    checkIfNull(machine_, "ADF not loaded");
    if (bem_ == NULL) {
        string errorMsg = "BEM not loaded";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    // initialize the plugin tool
    vector<string> pluginPaths = Environment::icDecoderPluginPaths();
    for (vector<string>::const_iterator iter = pluginPaths.begin();
         iter != pluginPaths.end(); iter++) {
        try {
            pluginTool_.addSearchPath(*iter);
        } catch (const FileNotFound&) {
        }
    }

    try {
        pluginTool_.registerModule(pluginFile);
    } catch (const FileNotFound&) {
        string errorMsg = "Plugin file '" + pluginFile + "' doesn't exist";
        throw FileNotFound(__FILE__, __LINE__, __func__, errorMsg);
    }

    ICDecoderGeneratorPlugin* (*creator)(
        TTAMachine::Machine&, BinaryEncoding&);
    pluginTool_.importSymbol(
        "create_generator_plugin_" + pluginName, creator, pluginFile);
    plugin_ = creator(*machine_, *bem_);
    assert(plugin_ != NULL);
}

/**
 * Generates the processor with the loaded data.
 *
 * @param imemWidthInMAUs Width of the instruction memory in MAUs.
 * @param language The language to generate.
 * @param dstDirectory The destination directory.
 * @param outputStream Stream where error messages etc. are written.
 * @exception InvalidData If ADF or IDF is not loaded.
 * @exception IOException If an IO exception occurs.
 * @exception DynamicLibraryException If the default plugin cannot be opened.
 * @exception InvalidData If HDB or IDF is erroneous or if BEM is not
 *                        compatible with the machine.
 * @exception IllegalMachine If the machine is illegal.
 * @exception OutOfRange If the given instruction memory width is not positive.
 * @exception InstanceNotFound Something missing from HDB.
 */
void
ProGeUI::generateProcessor(
    int imemWidthInMAUs,
    HDL language,
    const std::string& dstDirectory,
    std::ostream& outputStream)
    throw (InvalidData, DynamicLibraryException, IOException,
           IllegalMachine, OutOfRange, InstanceNotFound) {

    checkIfNull(machine_, "ADF not loaded");
    checkIfNull(idf_, "IDF not loaded");
    if (bem_ == NULL) {
        BEMGenerator generator(*machine_);
        bem_ = generator.generate();
    }

    // validate IDF against machine
    IDFValidator idfValidator(*idf_, *machine_);
    if (!idfValidator.validate()) {
        string errorMsg = idfValidator.errorMessage(0);
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    // validate BEM against machine
    BEMValidator bemValidator(*bem_, *machine_);
    if (!bemValidator.validate()) {
        string errorMsg("");
        if (bemValidator.errorCount() > 0) {
            errorMsg = bemValidator.errorMessage(0);
        }
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    if (plugin_ == NULL) {
        if (!idf_->hasICDecoderPluginFile() ||
            !idf_->hasICDecoderPluginName()) {
            string errorMsg = "IC/decoder generator plugin not defined";
            throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
        } else {
            loadICDecoderGeneratorPlugin(
                idf_->icDecoderPluginFile(), idf_->icDecoderPluginName());

            // set plugin parameters
            for (unsigned i = 0; i < idf_->icDecoderParameterCount(); i++) {
                plugin_->setParameter(
                    idf_->icDecoderParameterName(i),
                    idf_->icDecoderParameterValue(i));
            }
        }
    }

    ProcessorGenerator generator;
    generator.generateProcessor(
        language, *machine_, *idf_, *plugin_, imemWidthInMAUs,
        dstDirectory, outputStream);
}


/** 
 * Generates a test bench for simulating.
 *
 * @param dstDir Destination directory for the test bench.
 * @param progeOutDir ProGe output directory.
 *
 * @exception InvalidData If Machine or implementation not loaded.
 */
void
ProGeUI::generateTestBench(
    const std::string& dstDir, 
    const std::string& progeOutDir) {

    checkIfNull(machine_, "ADF not loaded");
    checkIfNull(idf_, "IDF not loaded");
    
    ProGeTestBenchGenerator::ProGeTestBenchGenerator tbGen = 
        ProGeTestBenchGenerator::ProGeTestBenchGenerator();
    tbGen.generate(*machine_, *idf_, dstDir, progeOutDir);
}

/** 
 * Generates vhdl compilation and simulation scripts.
 * 
 * @param dstDir Destination directory for the scripts.
 * @param progeOutDir ProGe output directory.
 * @param testBenchDir Directory where a test bench is stored. 
 */
void 
ProGeUI::generateScripts(
    const std::string& dstDir,
    const std::string& progeOutDir,
    const std::string& testBenchDir) {

    ProGeScriptGenerator::ProGeScriptGenerator sGen(dstDir, progeOutDir, 
        testBenchDir);
    sGen.generateAll();
}

/** 
 * Checks if given pointer is NULL.
 *
 * Throws a InvalidData exception if given pointer is NULL and sets given
 * error message as exceptions error message.
 * 
 * @param nullPointer Some pointer. 
 * @param errorMsg Error message for the InvalidData exception.
 */
void
ProGeUI::checkIfNull(void * nullPointer, const std::string& errorMsg)
    throw (InvalidData) {

    if (nullPointer == NULL) {
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
}

} // end of namespace ProGe

