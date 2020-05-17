/**
 * @file        cmd_blossom.cpp
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

#include "cmd_blossom.h"

namespace SakuraTree
{

CmdBlossom::CmdBlossom()
    : Blossom()
{
    m_hasOutput = true;

    m_requiredKeys.insert("command", new DataValue(true));
    m_requiredKeys.insert("ignore_errors", new DataValue(false));
}

/**
 * initBlossom
 */
void
CmdBlossom::initBlossom(BlossomItem &blossomItem)
{
    m_command = blossomItem.values.getValueAsString("command");
    const ValueItem ignoreResultItem = blossomItem.values.getValueItem("ignore_errors");

    if(ignoreResultItem.item != nullptr)
    {
        if(ignoreResultItem.item->isBoolValue())
        {
            m_ignoreResult = ignoreResultItem.item->getBool();
            blossomItem.success = true;
        }
        else
        {
            const std::string errorMsg = "ignore_errors was set, but is not a bool-value";
            blossomItem.outputMessage = errorMsg;
            blossomItem.success = false;
        }
    }
}

/**
 * preCheck
 */
void
CmdBlossom::preCheck(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * runTask
 */
void
CmdBlossom::runTask(BlossomItem &blossomItem)
{
    ProcessResult processResult = runSyncProcess(m_command);
    blossomItem.success = processResult.success;

    if(m_ignoreResult) {
        blossomItem.success = true;
    }

    blossomItem.outputMessage = processResult.processOutput;
    blossomItem.blossomOutput = new DataValue(processResult.processOutput);
}

/**
 * postCheck
 */
void
CmdBlossom::postCheck(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * closeBlossom
 */
void
CmdBlossom::closeBlossom(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

}
