/**
 * @file        text_append_blossom.cpp
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

#include "text_append_blossom.h"
#include <processing/blossoms/files/file_methods.h>

namespace SakuraTree
{

TextAppendBlossom::TextAppendBlossom() :
    Blossom() {}

/**
 * @brief initTask
 */
void
TextAppendBlossom::initTask(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * @brief preCheck
 */
void
TextAppendBlossom::preCheck(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * @brief runTask
 */
void
TextAppendBlossom::runTask(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * @brief postCheck
 */
void
TextAppendBlossom::postCheck(BlossomItem &blossomItem)
{

    blossomItem.success = true;
}

/**
 * @brief closeTask
 */
void
TextAppendBlossom::closeTask(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

}
