/**
 * @file        template_blossoms.cpp
 *
 * @author      Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 * @copyright   Apache License Version 2.0
 *
 *      Copyright 2019 Tobias Anker
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "template_blossoms.h"

#include <libKitsunemimiSakuraLang/sakura_lang_interface.h>

#include <libKitsunemimiPersistence/files/file_methods.h>
#include <libKitsunemimiPersistence/files/text_file.h>

#include <libKitsunemimiJinja2/jinja2_converter.h>

#include <sakura_root.h>

using Kitsunemimi::Jinja2::Jinja2Converter;

/**
 * @brief convert a jinja2-template with values into a new string
 *
 * @param output reference for the converted output-string
 * @param templatePath relative-path to the template-file within the sakura-garden
 * @param values value-item-map with all values, which should be inserted into the template
 * @param errorMessage reference-string for output of the error-message in case of an error
 *
 * @return true, if successful, else false
 */
bool
convertTemplate(std::string &output,
                const std::string &templatePath,
                Kitsunemimi::DataMap &values,
                std::string &errorMessage)
{
    // read template-file
    SakuraLangInterface* interface = SakuraLangInterface::getInstance();
    const std::string templateCont = interface->getTemplate(templatePath);
    if(templateCont == "")
    {
        errorMessage = "couldn't find template-file " + templatePath;
        return false;
    }

    // create file-content form template
    Jinja2Converter* converter = Jinja2Converter::getInstance();
    const bool jinja2Result = converter->convert(output,
                                                 templateCont,
                                                 &values,
                                                 errorMessage);

    if(jinja2Result == false)
    {
        errorMessage = "couldn't convert template-file "
                       + templatePath +
                       " with reason: "
                       + errorMessage;
        return false;
    }

    return true;
}

/**
 * @brief get absolute template path
 *
 * @param blossomLeaf actual blossom-leaf
 *
 * @return absolute path
 */
const std::string
getAbsoluteTemplatePath(BlossomLeaf &blossomLeaf)
{
    std::string templatePath = blossomLeaf.input.getStringByKey("source_path");
    const bfs::path path = bfs::path("templates") / bfs::path(templatePath);

    SakuraLangInterface* interface = SakuraLangInterface::getInstance();
    templatePath = interface->getRelativePath(blossomLeaf.blossomPath, path).string();
    return templatePath;
}

//==================================================================================================
// TemplateCreateFileBlossom
//==================================================================================================
TemplateCreateFileBlossom::TemplateCreateFileBlossom()
    : Blossom()
{
    validationMap.emplace("source_path", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
    validationMap.emplace("dest_path", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
    validationMap.emplace("owner", BlossomValidDef(IO_ValueType::INPUT_TYPE, false));
    validationMap.emplace("permission", BlossomValidDef(IO_ValueType::INPUT_TYPE, false));
    validationMap.emplace("variables", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
}

/**
 * runTask
 */
bool
TemplateCreateFileBlossom::runTask(BlossomLeaf &blossomLeaf, std::string &errorMessage)
{
    const std::string templatePath = getAbsoluteTemplatePath(blossomLeaf);
    const std::string destinationPath = blossomLeaf.input.getStringByKey("dest_path");
    const std::string owner = blossomLeaf.input.getStringByKey("owner");
    const std::string permission = blossomLeaf.input.getStringByKey("permission");

    std::string convertedContent = "";
    bool ret = convertTemplate(convertedContent,
                               templatePath,
                               *blossomLeaf.input.get("variables")->toMap(),
                               errorMessage);
    if(ret == false) {
        return false;
    }

    // check if template-file already exist
    if(bfs::exists(destinationPath))
    {
        // read the already existing file and compare it the current file-content
        std::string existingFileContent;
        Kitsunemimi::Persistence::readFile(existingFileContent, destinationPath, errorMessage);
        if(convertedContent == existingFileContent) {
            return true;
        }
    }

    // write converted template into the file
    ret = Kitsunemimi::Persistence::writeFile(destinationPath,
                                              convertedContent,
                                              errorMessage,
                                              true);
    if(ret == false)
    {
        errorMessage = "couldn't write template-file to "
                       + destinationPath +
                       " with reason: "
                       + errorMessage;
        return false;
    }

    // set owner if defined
    if(owner != "")
    {
        const std::string command = "chown " + owner + ":" + owner + " " + destinationPath;
        if(SakuraRoot::m_root->runCommand(command, errorMessage) == false) {
            return false;
        }
    }

    // set permission if defined
    if(permission != "")
    {
        const std::string command = "chmod " + permission + " " + destinationPath;
        if(SakuraRoot::m_root->runCommand(command, errorMessage) == false) {
            return false;
        }
    }

    // post-check
    std::string fileContent = "";
    ret = Kitsunemimi::Persistence::readFile(fileContent, destinationPath, errorMessage);
    if(ret == false
            || convertedContent != fileContent)
    {
        return false;
    }

    return true;
}

//==================================================================================================
// TemplateCreateStringBlossom
//==================================================================================================
TemplateCreateStringBlossom::TemplateCreateStringBlossom()
    : Blossom()
{
    validationMap.emplace("source_path", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
    validationMap.emplace("variables", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
    validationMap.emplace("text", BlossomValidDef(IO_ValueType::OUTPUT_TYPE, true));
}

/**
 * runTask
 */
bool
TemplateCreateStringBlossom::runTask(BlossomLeaf &blossomLeaf, std::string &errorMessage)
{
    const std::string templatePath = getAbsoluteTemplatePath(blossomLeaf);

    std::string convertedContent = "";
    const bool ret = convertTemplate(convertedContent,
                                     templatePath,
                                     *blossomLeaf.input.get("variables")->toMap(),
                                     errorMessage);

    if(ret == false) {
        return false;
    }

    blossomLeaf.output.insert("text", new DataValue(convertedContent));

    return true;
}
