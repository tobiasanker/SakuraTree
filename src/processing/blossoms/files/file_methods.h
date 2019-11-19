/**
 * @file        file_methods.h
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

#ifndef FILE_METHODS_H
#define FILE_METHODS_H

#include <common.h>

namespace SakuraTree
{

bool doesPathExist(const std::string path);
bool doesFileExist(const std::string filePath);
bool doesDirExist(const std::string dirPath);
std::pair<bool, std::string> renameFileOrDir(const std::string oldPath,
                                             const std::string newPath);
std::pair<bool, std::string> copyFile(const std::string sourcePath,
                                      const std::string targetPath);
bool deleteFileOrDis(const std::string path);

}

#endif // FILE_METHODS_H