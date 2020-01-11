﻿/**
 * @file        converter.cpp
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

#include "converter.h"

#include <libKitsunemimiSakuraParser/sakura_parsing.h>
#include <libKitsunemimiSakuraParser/sakura_parsing.h>

#include <libKitsunemimiJson/json_item.h>

#include <sakura_root.h>
#include <items/sakura_items.h>
#include <processing/common/item_methods.h>
#include <converter/common_converter_methods.h>

using Kitsunemimi::Json::JsonItem;

namespace SakuraTree
{

/**
 * @brief constructor
 */
Converter::Converter() {}

/**
 * @brief destructor
 */
Converter::~Converter() {}

/**
 * @brief convert a json-item based tree into a sakura-items based version and run additional
 *        validation checks while converting the tree
 *
 * @param tree tree based on json-items
 *
 * @return tree based on sakura-items if converting was successful, else null-pointer
 */
SakuraItem*
Converter::convert(const JsonItem &tree)
{
    // debug-output
    if(DEBUG)
    {
        std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
        const std::string output = tree.toString(true);
        std::cout<<output<<std::endl;
        std::cout<<"-----------------------------------------------------"<<std::endl;
    }

    bool success = true;
    SakuraItem* result = convertPart(tree, success);


    if(success == false)
    {
        delete result;
        result = nullptr;
    }

    return result;
}

/**
 * @brief convert one single value-item
 *
 * @param resultingPart reference to the target-object, where the information should be filled in
 * @param itemInput json-formated item-information
 * @param pairType type of the pari (input, output or compare)
 * @param success reference to the success-status-value
 */
void
Converter::convertSingleItemValue(ValueItem &resultingPart,
                                  const JsonItem &itemInput,
                                  const std::string pairType,
                                  bool &success)
{
    // convert item
    if(itemInput.isMap()
            && itemInput.contains("item"))
    {
        resultingPart.item = itemInput.get("item").getItemContent()->copy()->toValue();
    }
    else if (itemInput.isMap())
    {
        resultingPart.item = itemInput.getItemContent()->copy();
    }
    else if (itemInput.isArray())
    {
        DataArray* tempArray = new DataArray();
        for(uint32_t i = 0; i < itemInput.size(); i++)
        {
            tempArray->append(itemInput.get(i).getItemContent()->copy());
        }
        resultingPart.item = tempArray;
    }

    // set type
    if(pairType == "assign") {
        resultingPart.type = ValueItem::INPUT_PAIR_TYPE;
    }
    if(pairType == "compare") {
        resultingPart.type = ValueItem::COMPARE_EQUAL_PAIR_TYPE;
    }
    if(pairType == "output") {
        resultingPart.type = ValueItem::OUTPUT_PAIR_TYPE;
    }

    // check if current value is value or identifier
    if(itemInput.get("b_type").toString() == "identifier") {
        resultingPart.isIdentifier = true;
    }

    // get function
    JsonItem functions = itemInput.get("functions");
    for(uint32_t f = 0; f < functions.size(); f++)
    {
        FunctionItem functionItem;

        // get function-type
        if(functions.get(f).get("b_type").toString() == "get") {
            functionItem.type = FunctionItem::GET_FUNCTION;
        }
        if(functions.get(f).get("b_type").toString() == "split") {
            functionItem.type = FunctionItem::SPLIT_FUNCTION;
        }
        if(functions.get(f).get("b_type").toString() == "contains") {
            functionItem.type = FunctionItem::CONTAINS_FUNCTION;
        }
        if(functions.get(f).get("b_type").toString() == "size") {
            functionItem.type = FunctionItem::SIZE_FUNCTION;
        }
        if(functions.get(f).get("b_type").toString() == "insert") {
            functionItem.type = FunctionItem::INSERT_FUNCTION;
        }
        if(functions.get(f).get("b_type").toString() == "append") {
            functionItem.type = FunctionItem::APPEND_FUNCTION;
        }

        // get argument-list
        JsonItem arguments = functions.get(f).get("args");
        for(uint32_t a = 0; a < arguments.size(); a++)
        {
            ValueItem newItem;
            convertSingleItemValue(newItem, arguments.get(a), "assign", success);
            functionItem.arguments.push_back(newItem);
        }

        resultingPart.functions.push_back(functionItem);
    }
}

/**
 * @brief convert the values of an item
 *
 * @param sakuraItem item, where the new converted values should be added
 * @param jsonValues json-formated values, which should be converted
 * @param success reference to the success-status-value
 */
void
Converter::convertItemValues(SakuraItem* sakuraItem,
                             const JsonItem &jsonValues,
                             bool &success)
{
    for(uint32_t i = 0; i < jsonValues.size(); i++)
    {
        const std::string key = jsonValues.get(i).get("key").toString();
        const std::string pairType = jsonValues.get(i).get("b_type").toString();
        const JsonItem value = jsonValues.get(i).get("value");

        ValueItem itemValue;
        convertSingleItemValue(itemValue, value, pairType, success);
        sakuraItem->values.insert(key, itemValue);
    }
}

/**
 * @brief convert subtree
 *
 * @param subtree current subtree for converting
 * @param success reference to the success-status-value
 *
 * @return pointer of the current converted part
 */
SakuraItem*
Converter::convertPart(const JsonItem &subtree, bool &success)
{
    const std::string typeName = subtree.get("b_type").toString();

    if(typeName == "blossom_group") {
        return convertBlossomGroup(subtree, success);
    }

    if(typeName == "subtree") {
        return convertSubtree(subtree, success);
    }

    if(typeName == "seed") {
        return convertSeed(subtree, success);
    }

    if(typeName == "if") {
        return convertIf(subtree, success);
    }

    if(typeName == "for_each") {
        return convertForEach(subtree, false, success);
    }

    if(typeName == "for") {
        return convertFor(subtree, false, success);
    }

    if(typeName == "parallel") {
        return convertParallel(subtree, success);
    }

    if(typeName == "parallel_for") {
        return convertFor(subtree, true, success);
    }

    if(typeName == "parallel_for_each") {
        return convertForEach(subtree, true, success);
    }

    // it must everytime match one of the names
    assert(false);

    return nullptr;
}

/**
 * @brief convert blossom-group
 *
 * @param subtree current suttree for converting
 * @param success reference to the success-status-value
 *
 * @return pointer of the current converted part
 */
SakuraItem*
Converter::convertBlossomGroup(const JsonItem &subtree, bool &success)
{
    // init new blossom-group-item
    BlossomGroupItem* blossomGroupItem =  new BlossomGroupItem();
    blossomGroupItem->id = subtree.get("name").toString();
    blossomGroupItem->blossomGroupType = subtree.get("blossom-group-type").toString();

    // convert all blossoms of the group
    const JsonItem subTypeArray = subtree.get("blossoms");
    if(subTypeArray.size() > 0)
    {
        for(uint32_t i = 0; i < subTypeArray.size(); i++)
        {
            const JsonItem item = subTypeArray.get(i);

            // init new blossom-item
            BlossomItem* blossomItem = new BlossomItem();
            blossomItem->blossomName = subtree.get("name").toString();
            blossomItem->blossomPath = subtree.get("b_path").toString();
            blossomItem->blossomType = item.get("blossom-type").toString();
            blossomItem->blossomGroupType = subtree.get("blossom-group-type").toString();

            convertItemValues(blossomItem, subtree.get("items-input"), success);
            convertItemValues(blossomItem, item.get("items-input"), success);

            blossomGroupItem->blossoms.push_back(blossomItem);

            if(checkBlossomItem(*blossomItem) == false) {
                success = false;
            }
        }
    }
    else
    {
        // init new blossom-item
        BlossomItem* blossomItem =  new BlossomItem();
        blossomItem->blossomName = subtree.get("name").toString();
        blossomItem->blossomPath = subtree.get("b_path").toString();
        blossomItem->blossomType = subtree.get("blossom-group-type").toString();
        blossomItem->blossomGroupType = "special";

        convertItemValues(blossomItem, subtree.get("items-input"), success);

        blossomGroupItem->blossomGroupType = "special";
        blossomGroupItem->blossoms.push_back(blossomItem);

        if(checkBlossomItem(*blossomItem) == false) {
            success = false;
        }
    }

    return blossomGroupItem;
}

/**
 * @brief convert branch
 *
 * @param subtree current suttree for converting
 * @param success reference to the success-status-value
 *
 * @return pointer of the current converted part
 */
SakuraItem*
Converter::convertSubtree(const JsonItem &subtree, bool &success)
{
    // init new branch-item
    SubtreeItem* branchItem = new SubtreeItem();
    branchItem->id = subtree.get("b_id").toString();

    // fill values with the input of the upper level and convert the result
    JsonItem items = subtree.get("items");
    if(subtree.contains("items-input")) {
        overrideItems(items, subtree.get("items-input"));
    }
    convertItemValues(branchItem, items, success);

    // convert parts of the branch
    const JsonItem parts = subtree.get("parts");
    if(parts.isValid() == false)
    {
        success = false;
        return  branchItem;
    }

    // convert parts of the subtree
    for(uint32_t i = 0; i < parts.size(); i++)
    {
        JsonItem newMap = parts.get(i);
        newMap.insert("b_path", subtree.get("b_path"));
        branchItem->childs.push_back(convertPart(newMap, success));
    }

    return branchItem;
}

/**
 * @brief convert seed-object
 *
 * @param subtree current suttree for converting
 * @param success reference to the success-status-value
 *
 * @return pointer of the current converted part
 */
SakuraItem*
Converter::convertSeed(const JsonItem &subtree, bool &)
{
    // init new seed-item
    SeedItem* seedItem = new SeedItem();
    seedItem->name = subtree.get("b_id").toString();

    // generate new branch-item based on the information
    const JsonItem connectionInfos = subtree.get("connection");
    // TODO: enable again in 0.3.0
    /*SubtreeItem* provisioningBranch = createProvisionBranch
            (
                connectionInfos.get("address").getString(),
                connectionInfos.get("ssh_port").getInt(),
                connectionInfos.get("ssh_user").getString(),
                connectionInfos.get("ssh_key").getString(),
                SakuraRoot::m_executablePath,
                "",
                subtree.get("subtree").getString()
            );

    seedItem->child = provisioningBranch;*/

    return seedItem;
}

/**
 * @brief convert if-condition
 *
 * @param subtree current suttree for converting
 * @param success reference to the success-status-value
 *
 * @return pointer of the current converted part
 */
SakuraItem*
Converter::convertIf(const JsonItem &subtree, bool &success)
{
    // init new if-item
    IfBranching* newItem = new IfBranching();

    // convert the both sides of the if-condition
    convertSingleItemValue(newItem->leftSide, subtree.get("left"), "assign", success);
    convertSingleItemValue(newItem->rightSide, subtree.get("right"), "assign", success);

    // convert compare-tpye
    if(subtree.get("if_type").getString() == "==") {
        newItem->ifType = IfBranching::EQUAL;
    }
    if(subtree.get("if_type").getString() == "!=") {
        newItem->ifType = IfBranching::UNEQUAL;
    }
    if(subtree.get("if_type").getString() == ">=") {
        newItem->ifType = IfBranching::GREATER_EQUAL;
    }
    if(subtree.get("if_type").getString() == ">") {
        newItem->ifType = IfBranching::GREATER;
    }
    if(subtree.get("if_type").getString() == "<=") {
        newItem->ifType = IfBranching::SMALLER_EQUAL;
    }
    if(subtree.get("if_type").getString() == "<") {
        newItem->ifType = IfBranching::SMALLER;
    }

    // get and check parts of the if-subtree
    JsonItem if_parts = subtree.get("if_parts");
    if(if_parts.isValid() == false)
    {
        success = false;
        return  newItem;
    }

    // create a sequentiell section to encapsulate the if-part of the if-condition
    SequentiellPart* sequentiellContentIf = new SequentiellPart();
    for(uint32_t i = 0; i < if_parts.size(); i++)
    {
        JsonItem newMap = if_parts.get(i);
        newMap.insert("b_path", subtree.get("b_path"));
        sequentiellContentIf->childs.push_back(convertPart(newMap, success));
    }
    newItem->ifContent = sequentiellContentIf;

    // get and check parts of the else-subtree
    JsonItem else_parts = subtree.get("else_parts");
    if(else_parts.isValid() == false)
    {
        success = false;
        return  newItem;
    }

    // create a sequentiell section to encapsulate the else-part of the if-condition
    SequentiellPart* sequentiellContentElse = new SequentiellPart();
    for(uint32_t i = 0; i < else_parts.size(); i++)
    {
        JsonItem newMap = else_parts.get(i);
        newMap.insert("b_path", subtree.get("b_path"));
        sequentiellContentElse->childs.push_back(convertPart(newMap, success));
    }
    newItem->elseContent = sequentiellContentElse;

    return newItem;
}

/**
 * @brief convert for-each-loop
 *
 * @param subtree current suttree for converting
 * @param parallel true, if the for-each-loop was declared as parallel loop
 * @param success reference to the success-status-value
 *
 * @return pointer of the current converted part
 */
SakuraItem*
Converter::convertForEach(const JsonItem &subtree,
                          const bool parallel,
                          bool &success)
{
    // init new for-each-item
    ForEachBranching* newItem = new ForEachBranching(parallel);
    newItem->tempVarName = subtree.get("variable").getItemContent()->toString();

    // convert the item, over which should be iterated
    ValueItem itemValue;
    convertSingleItemValue(itemValue, subtree.get("list"), "assign", success);
    newItem->iterateArray.insert("array", itemValue);
    convertItemValues(newItem, subtree.get("items"), success);

    // get and check parts of the subtree
    const JsonItem parts = subtree.get("parts");
    if(parts.isValid() == false)
    {
        success = false;
        return  newItem;
    }

    // create a sequentiell section to encapsulate the content of the for-loop
    SequentiellPart* sequentiellContent = new SequentiellPart();
    for(uint32_t i = 0; i < parts.size(); i++)
    {
        JsonItem newMap = parts.get(i);
        newMap.insert("b_path", subtree.get("b_path"));
        sequentiellContent->childs.push_back(convertPart(newMap, success));
    }
    newItem->content = sequentiellContent;

    return newItem;
}

/**
 * @brief convert for-loop
 *
 * @param subtree current suttree for converting
 * @param parallel true, if the for-loop was declared as parallel loop
 * @param success reference to the success-status-value
 *
 * @return pointer of the current converted part
 */
SakuraItem*
Converter::convertFor(const JsonItem &subtree,
                      const bool parallel,
                      bool &success)
{
    // init new for-item
    ForBranching* newItem = new ForBranching(parallel);
    newItem->tempVarName = subtree.get("variable1").getItemContent()->toString();

    // convert start- and end-point of the iterations
    convertSingleItemValue(newItem->start, subtree.get("start"), "assign", success);
    convertSingleItemValue(newItem->end, subtree.get("end"), "assign", success);
    convertItemValues(newItem, subtree.get("items"), success);

    // get and check parts of the subtree
    const JsonItem parts = subtree.get("parts");
    if(parts.isValid() == false)
    {
        success = false;
        return  newItem;
    }

    // create a sequentiell section to encapsulate the content of the for-loop
    SequentiellPart* sequentiellContent = new SequentiellPart();
    for(uint32_t i = 0; i < parts.size(); i++)
    {
        JsonItem newMap = parts.get(i);
        newMap.insert("b_path", subtree.get("b_path"));
        sequentiellContent->childs.push_back(convertPart(newMap, success));
    }
    newItem->content = sequentiellContent;

    return newItem;
}

/**
 * @brief convert parallel part of subtree
 *
 * @param subtree current suttree for converting
 * @param success reference to the success-status-value
 *
 * @return pointer of the current converted part
 */
SakuraItem*
Converter::convertParallel(const JsonItem &subtree, bool &success)
{
    // init new parallel-branching-item
    ParallelPart* newItem = new ParallelPart();

    // get and check parts of the subtree
    const JsonItem parts = subtree.get("parts");
    if(parts.isValid() == false)
    {
        success = false;
        return  newItem;
    }

    // convert alls parts of the subtree
    for(uint32_t i = 0; i < parts.size(); i++)
    {
        newItem->childs.push_back(convertPart(parts.get(i), success));
    }

    return newItem;
}

}
