/**
 * @file        common_methods.h
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

#ifndef COMMON_METHODS_H
#define COMMON_METHODS_H

#include <common.h>

namespace Kitsunemimi
{
namespace Jinja2 {
class Jinja2Converter;
}
}

using Kitsunemimi::DataMap;


bool getProcessedItem(ValueItem &valueItem,
                      DataMap &insertValues,
                      std::string &errorMessage);

// fill functions
bool fillIdentifierItem(ValueItem &valueItem,
                        DataMap &insertValues,
                        std::string &errorMessage);
bool fillJinja2Template(ValueItem &valueItem,
                        DataMap &insertValues,
                        std::string &errorMessage);
bool fillValueItem(ValueItem &valueItem,
                   DataMap &insertValues,
                   std::string &errorMessage);
bool fillInputValueItemMap(ValueItemMap &items,
                           DataMap &insertValues,
                           std::string &errorMessage);
bool fillBlossomOutputValueItemMap(ValueItemMap &items,
                                   DataItem* output);
bool fillSubtreeOutputValueItemMap(ValueItemMap &items,
                                   DataMap *output);

// override functions
void overrideItems(DataMap &original,
                   const DataMap &override,
                   bool onlyExisting = true);
void overrideItems(DataMap &original,
                   const ValueItemMap &override,
                   bool onlyExisting = true);
void overrideItems(ValueItemMap &original,
                   const ValueItemMap &override,
                   bool onlyExisting = false,
                   bool onlyNotExisting = false);

// check items
const std::vector<std::string> checkInput(Kitsunemimi::Sakura::ValueItemMap &original,
                                          const DataMap &itemInputValues);
const std::vector<std::string> checkItems(DataMap &items);

// convert
const std::string convertBlossomOutput(const BlossomItem &blossom);

// error-output
const std::string createError(const BlossomItem &blossomItem,
                              const std::string &errorLocation,
                              const std::string &errorMessage,
                              const std::string &possibleSolution = "");
const std::string createError(const std::string &errorLocation,
                              const std::string &errorMessage,
                              const std::string &possibleSolution = "",
                              const std::string &blossomType = "",
                              const std::string &blossomGroupType = "",
                              const std::string &blossomName = "",
                              const std::string &blossomFilePath = "");

#endif // COMMON_METHODS_H
