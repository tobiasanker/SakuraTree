/**
 * @file        blossom.h
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

#ifndef BLOSSOM_H
#define BLOSSOM_H

#include <common.h>
#include <sakura_root.h>
#include <processing/common/item_methods.h>
#include <libKitsunemimiPersistence/logger/logger.h>

namespace SakuraTree
{

class Blossom
{
public:
    Blossom();
    virtual ~Blossom();

    void growBlossom(BlossomItem &blossomItem,
                     std::string &errorMessage);

    DataMap m_requiredKeys;
    bool m_hasOutput = false;

protected:
    virtual void initBlossom(BlossomItem &blossomItem) = 0;
    virtual void preCheck(BlossomItem &blossomItem) = 0;
    virtual void runTask(BlossomItem &blossomItem) = 0;
    virtual void postCheck(BlossomItem &blossomItem) = 0;
    virtual void closeBlossom(BlossomItem &blossomItem) = 0;
};

}

#endif // BLOSSOM_H
