/**
 * @file        common_methods.cpp
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

#include "item_methods.h"

#include <libKitsunemimiJinja2/jinja2_converter.h>
#include <sakura_root.h>
#include <processing/blossoms/blossom.h>

using Kitsunemimi::Jinja2::Jinja2Converter;

namespace SakuraTree
{

/**
 * replace the jinja2-converted values of the items-object with the stuff of the insertValues-object
 */
const std::pair<bool, std::string>
fillItems(ValueItemMap &items,
          ValueItemMap &insertValues)
{
    std::pair<bool, std::string> result;

    std::map<std::string, ValueItem>::iterator it;
    for(it = items.valueMap.begin();
        it != items.valueMap.end();
        it++)
    {
        // TODO: make better
        if(it->first == "subtree")
        {
            //it->second.item = new DataValue(insertValues.at(0).item->toString());
            continue;
        }

        if(it->second.type == ValueItem::INPUT_PAIR_TYPE
                && it->second.item->isStringValue())
        {
            const std::string tempItem = it->second.item->toString();

            DataMap possibleValues;
            std::map<std::string, ValueItem>::iterator insertIt;
            for(insertIt = insertValues.valueMap.begin();
                insertIt != insertValues.valueMap.end();
                insertIt++)
            {
                if(insertIt->second.item->isValue()) {
                    possibleValues.insert(insertIt->first, insertIt->second.item->copy());
                }
            }

            Jinja2Converter* converter = SakuraRoot::m_root->m_jinja2Converter;
            const std::pair<bool, std::string> convertResult = converter->convert(tempItem,
                                                                                  &possibleValues);

            if(convertResult.first == false)
            {
                result.first = false;
                result.second = "failed to fill value of: " + it->first;
                return result;
            }

            if(it->second.item != nullptr) {
                delete it->second.item;
            }
            it->second.item = new DataValue(convertResult.second);
        }
    }

    result.first = true;
    return result;
}

/**
 * @brief overrideItems
 */
void overrideItems(ValueItemMap &original,
                   ValueItemMap &override)
{
    std::map<std::string, ValueItem>::iterator overrideIt;
    for(overrideIt = override.valueMap.begin(); overrideIt != override.valueMap.end(); overrideIt++)
    {
        std::map<std::string, ValueItem>::iterator originalIt;
        originalIt = original.valueMap.find(overrideIt->first);

        if(originalIt != original.valueMap.end()) {
            originalIt->second = overrideIt->second;
        }
    }
}

/**
 * @brief checkItems
 *
 * @return list of not initialized values
 */
const std::vector<std::string>
checkItems(ValueItemMap &items)
{
    std::vector<std::string> result;

    std::map<std::string, ValueItem>::iterator it;
    for(it = items.valueMap.begin(); it != items.valueMap.end(); it++)
    {
        if(it->second.item->getString() == "{{}}") {
            result.push_back(it->first);
        }
    }

    return result;
}

/**
 * @brief checkForRequiredKeys
 * @param values
 * @param requiredKeys
 * @return
 */
void
checkForRequiredKeys(BlossomItem &blossomItem,
                     const std::vector<std::string> &requiredKeys)
{
    std::string result = "";
    blossomItem.success = true;

    for(uint32_t i = 0; i < requiredKeys.size(); i++)
    {
        const bool found = blossomItem.values.contains(requiredKeys.at(i));
        if(found == false)
        {
            if(result.size() > 0) {
                result += ", ";
            }
            result += requiredKeys.at(i);
        }
    }

    if(result.size() > 0)
    {
        blossomItem.success = false;
        blossomItem.outputMessage = "following keys are not set: " + result;
    }
}

/**
 * @brief printOutput
 * @param blossom
 */
const std::string
convertBlossomOutput(const BlossomItem &blossom)
{
    std::string output = "";
    output += "+++++++++++++++++++++++++++++++++++++++++++++++++\n";

    // print executeion-state
    switch(blossom.resultState)
    {
        case BlossomItem::SKIPPED_STATE:
            output += "SKIPPED\n";
            break;
        case BlossomItem::CHANGED_STATE:
            output += "CHANGED\n";
            break;
        case BlossomItem::ERROR_INIT_STATE:
            output += "ERROR in init-state\n";
            break;
        case BlossomItem::ERROR_PRECHECK_STATE:
            output += "ERROR in pre-check-state\n";
            break;
        case BlossomItem::ERROR_EXEC_STATE:
            output += "ERROR in exec-state with error-code: "
                    + std::to_string(blossom.execState) + "\n";
            break;
        case BlossomItem::ERROR_POSTCHECK_STATE:
            output += "ERROR in post-check-state\n";
            break;
        case BlossomItem::ERROR_CLOSE_STATE:
            output += "ERROR in error-state\n";
            break;
        default:
            break;
    }

    // print call-hierarchy
    for(uint32_t i = 0; i < blossom.nameHirarchie.size(); i++)
    {
        for(uint32_t j = 0; j < i; j++)
        {
            output += "   ";
        }
        output += blossom.nameHirarchie.at(i) + "\n";
    }

    // print error-output
    if(blossom.outputMessage.size() > 0
            && blossom.resultState >= 3)
    {
        // TODO: red error output
        output += "\n";
        output += blossom.outputMessage + "\n";
    }

    // print error-output
    const bool success = blossom.resultState == BlossomItem::CHANGED_STATE
                         || blossom.resultState == BlossomItem::SKIPPED_STATE;
    if(blossom.outputMessage.size() > 0
            && success)
    {
        output += "\n";
        output += blossom.outputMessage + "\n";
    }

    output += "-------------------------------------------------\n";
    output += "\n";

    return output;
}

}
