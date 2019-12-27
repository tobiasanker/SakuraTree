/**
 * @file        file_rename_blossom.cpp
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

#include "file_rename_blossom.h"
#include <processing/blossoms/files/file_methods.h>
#include <libKitsunemimiCommon/common_methods/string_methods.h>
#include <libKitsunemimiCommon/common_methods/vector_methods.h>

namespace SakuraTree
{
using Kitsunemimi::Common::splitStringByDelimiter;

FileRenameBlossom::FileRenameBlossom()
    : Blossom()
{
    m_requiredKeys.insert("file_path", new DataValue(true));
    m_requiredKeys.insert("new_name", new DataValue(true));
}

/**
 * @brief initBlossom
 */
void
FileRenameBlossom::initBlossom(BlossomItem &blossomItem)
{
    m_filePath = blossomItem.values.getValueAsString("file_path");
    m_newFileName = blossomItem.values.getValueAsString("new_name");

    std::vector<std::string> stringParts = splitStringByDelimiter(m_filePath, '/');
    stringParts[stringParts.size()-1] = m_newFileName;

    Kitsunemimi::Common::removeEmptyStrings(&stringParts);
    for(uint32_t i = 0; i < stringParts.size(); i++)
    {
        m_newFilePath += "/";
        m_newFilePath += stringParts.at(i);
    }

    blossomItem.success = true;
}

/**
 * @brief preCheck
 */
void
FileRenameBlossom::preCheck(BlossomItem &blossomItem)
{
    if(doesPathExist(m_filePath) == false
            && doesPathExist(m_newFilePath))
    {
        blossomItem.skip = true;
        blossomItem.success = true;
        return;
    }

    if(doesPathExist(m_filePath) == false)
    {
        blossomItem.success = false;
        blossomItem.outputMessage = "source-path "
                                   + m_filePath
                                   + " doesn't exist";
        return;
    }

    blossomItem.success = true;
}

/**
 * @brief runTask
 */
void
FileRenameBlossom::runTask(BlossomItem &blossomItem)
{
    const Result renameResult = renameFileOrDir(m_filePath, m_newFilePath);

    if(renameResult.success == false)
    {
        blossomItem.success = false;
        blossomItem.outputMessage = renameResult.errorMessage;
        return;
    }

    blossomItem.success = true;
}

/**
 * @brief postCheck
 */
void
FileRenameBlossom::postCheck(BlossomItem &blossomItem)
{
    if(doesFileExist(m_filePath))
    {
        blossomItem.success = false;
        blossomItem.outputMessage = "old object still exist";
        return;
    }

    if(doesPathExist(m_newFilePath) == false)
    {
        blossomItem.success = false;
        blossomItem.outputMessage = "was not able to rename from "
                                   + m_filePath
                                   + " to "
                                   + m_newFileName;
        return;
    }

    blossomItem.success = true;
}

/**
 * @brief closeBlossom
 */
void
FileRenameBlossom::closeBlossom(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

}
