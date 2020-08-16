/**
 * @file        path_rename_blossom.h
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

#ifndef PATH_RENAME_BLOSSOM_H
#define PATH_RENAME_BLOSSOM_H

#include <processing/blossoms/blossom.h>

class PathRenameBlossom_Test;

class PathRenameBlossom
        : public Blossom
{
public:
    PathRenameBlossom();

protected:
    void initBlossom(BlossomItem &blossomItem);
    void preCheck(BlossomItem &blossomItem);
    void runTask(BlossomItem &blossomItem);
    void postCheck(BlossomItem &blossomItem);
    void closeBlossom(BlossomItem &blossomItem);

private:
    friend PathRenameBlossom_Test;

    std::string m_path = "";
    std::string m_newFileName = "";
    std::string m_newFilePath = "";
};

#endif // PATH_RENAME_BLOSSOM_H
