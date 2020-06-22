/**
 * @file        validator.h
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

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <common.h>

namespace SakuraTree
{

bool checkOutput(BlossomItem &blossomItem,
                 const bool hasOutput);
bool checkBlossomItem(BlossomItem &blossomItem);
bool checkBlossomItem(BlossomItem &blossomItem,
                      DataMap &requiredKeys);


bool checkSakuraItem(SakuraItem* sakuraItem,
                     const std::string &filePath = "");

bool checkAllItems(const SakuraGarden &garden);

}

#endif // VALIDATOR_H
