/**
 * @file        text_blossoms.cpp
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

#include "text_blossoms.h"

#include <libKitsunemimiPersistence/files/file_methods.h>
#include <libKitsunemimiPersistence/files/text_file.h>

/**
 * @brief check if path exist and is a file
 *
 * @param filePath path of the file to check
 * @param errorMessage reference for error-message
 *
 * @return true, if path is valid, else false
 */
bool
checkFile(const std::string &filePath,
          std::string &errorMessage)
{
    if(bfs::exists(filePath) == false)
    {
        errorMessage = "path " + filePath + " doesn't exist";
        return false;
    }

    if(bfs::is_regular_file(filePath) == false)
    {
        errorMessage = "path " + filePath + " is not a file";
        return false;
    }

    return true;
}

//==================================================================================================
// TextAppendBlossom
//==================================================================================================
TextAppendBlossom::TextAppendBlossom()
    : Blossom()
{
    validationMap.emplace("file_path", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
    validationMap.emplace("text", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
}

/**
 * @brief runTask
 */
bool
TextAppendBlossom::runTask(BlossomLeaf &blossomLeaf, std::string &errorMessage)
{
    const std::string filePath = blossomLeaf.input.getStringByKey("file_path");
    const std::string newText = blossomLeaf.input.getStringByKey("text");

    const bool ret = checkFile(filePath, errorMessage);
    if(ret == false) {
        return false;
    }

    return Kitsunemimi::Persistence::appendText(filePath, newText, errorMessage);
}

//==================================================================================================
// TextReadBlossom
//==================================================================================================
TextReadBlossom::TextReadBlossom()
    : Blossom()
{
    validationMap.emplace("file_path", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
    validationMap.emplace("text", BlossomValidDef(IO_ValueType::OUTPUT_TYPE, true));
}

/**
 * @brief runTask
 */
bool
TextReadBlossom::runTask(BlossomLeaf &blossomLeaf, std::string &errorMessage)
{
    const std::string filePath = blossomLeaf.input.getStringByKey("file_path");

    const bool ret = checkFile(filePath, errorMessage);
    if(ret == false) {
        return false;
    }

    std::string fileContent = "";
    const bool result = Kitsunemimi::Persistence::readFile(fileContent, filePath, errorMessage);

    if(result == false) {
        return false;
    }

    blossomLeaf.output.insert("text", new Kitsunemimi::DataValue(fileContent));

    return true;
}

//==================================================================================================
// TextReplaceBlossom
//==================================================================================================
TextReplaceBlossom::TextReplaceBlossom()
    : Blossom()
{
    validationMap.emplace("file_path", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
    validationMap.emplace("old_text", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
    validationMap.emplace("new_text", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
}

/**
 * @brief runTask
 */
bool
TextReplaceBlossom::runTask(BlossomLeaf &blossomLeaf, std::string &errorMessage)
{
    const std::string filePath = blossomLeaf.input.getStringByKey("file_path");
    const std::string oldText = blossomLeaf.input.getStringByKey("old_text");
    const std::string newText = blossomLeaf.input.getStringByKey("new_text");

    const bool ret = checkFile(filePath, errorMessage);
    if(ret == false) {
        return false;
    }

    const bool result = Kitsunemimi::Persistence::replaceContent(filePath,
                                                                 oldText,
                                                                 newText,
                                                                 errorMessage);

    return result;
}

//==================================================================================================
// TextWriteBlossom
//==================================================================================================
TextWriteBlossom::TextWriteBlossom()
    : Blossom()
{
    validationMap.emplace("file_path", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
    validationMap.emplace("text", BlossomValidDef(IO_ValueType::INPUT_TYPE, true));
}

/**
 * @brief runTask
 */
bool
TextWriteBlossom::runTask(BlossomLeaf &blossomLeaf, std::string &errorMessage)
{
    const std::string filePath = blossomLeaf.input.getStringByKey("file_path");
    const std::string text = blossomLeaf.input.getStringByKey("text");

    return Kitsunemimi::Persistence::writeFile(filePath, text, errorMessage);
}
