/**
 * @file        sakura_copy_subtree_blossom.cpp
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

#include "sakura_copy_subtree_blossom.h"
#include <sakura_root.h>

namespace SakuraTree
{

SakuraCopySubtreeBlossom::SakuraCopySubtreeBlossom()
    : Blossom()
{
    m_requiredKeys.insert("subtree", new DataValue(true));
    m_requiredKeys.insert("address", new DataValue(true));
    m_requiredKeys.insert("values", new DataValue(true));
}

/**
 * initBlossom
 */
void
SakuraCopySubtreeBlossom::initBlossom(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * preCheck
 */
void
SakuraCopySubtreeBlossom::preCheck(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * runTask
 */
void
SakuraCopySubtreeBlossom::runTask(BlossomItem &blossomItem)
{
    const std::string address = blossomItem.values.getValueAsString("address");
    const std::string subtree = blossomItem.values.getValueAsString("subtree");
    const std::string values = blossomItem.values.getValueAsString("values");

    SakuraRoot::m_root->sendTreefile(address, subtree, values);

    sleep(2);
}

/**
 * postCheck
 */
void
SakuraCopySubtreeBlossom::postCheck(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * closeBlossom
 */
void
SakuraCopySubtreeBlossom::closeBlossom(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

}
