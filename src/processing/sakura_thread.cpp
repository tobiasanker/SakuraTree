﻿/**
 * @file        sakura_thread.cpp
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

#include "sakura_thread.h"

#include <sakura_root.h>
#include <tree_handler.h>

#include <processing/common/item_methods.h>
#include <processing/blossoms/blossom.h>
#include <processing/blossoms/blossom_getter.h>
#include <processing/subtree_queue.h>

#include <libKitsunemimiJinja2/jinja2_converter.h>

namespace SakuraTree
{

/**
 * @brief constructor
 *
 * @param queue pointer to the subtree-queue, where new separated subtrees should be added
 */
SakuraThread::SakuraThread(SubtreeQueue* queue)
{
    m_queue = queue;
}

/**
 * destructor
 */
SakuraThread::~SakuraThread()
{
}

/**
 * @brief run the main-loop of the thread and process each subtree, which can be taken from the
 *        queue
 */
void
SakuraThread::run()
{
    m_started = true;
    while(m_abort == false)
    {
        SubtreeQueue::SubtreeObject* currentSubtree = m_queue->getSubtreeObject();

        if(currentSubtree != nullptr)
        {
            if(currentSubtree->subtree == nullptr)
            {
                // if subtree-queue is empty, then wait for 1 milli-seconds before asking the queue
                // again for a new subtree
                std::this_thread::sleep_for(chronoMilliSec(1));
            }
            else
            {
                // handle subtree-object
                m_hierarchy = currentSubtree->hirarchy;
                overrideItems(m_parentValues, currentSubtree->subtree->values, false);
                overrideItems(m_parentValues, currentSubtree->items, false);
                processSakuraItem(currentSubtree->subtree);
                overrideItems(currentSubtree->items, m_parentValues, true);

                // increase active-counter as last step, so the source subtree can check, if all
                // spawned subtrees are finished
                if(currentSubtree->activeCounter != nullptr) {
                    currentSubtree->activeCounter->increaseCounter();
                }
            }
        }
    }
}

/**
 * @brief central method of the thread to process the current part of the execution-tree
 *
 * @param sakuraItem subtree, which should be processed
 *
 * @return true if successful, else false
 */
bool
SakuraThread::processSakuraItem(SakuraItem* sakuraItem)
{
    if(sakuraItem->getType() == SakuraItem::TREE_ITEM)
    {
        TreeItem* subtreeItem = dynamic_cast<TreeItem*>(sakuraItem);
        m_hierarchy.push_back("TREE: " + subtreeItem->id);
        return processTree(subtreeItem);
    }

    if(sakuraItem->getType() == SakuraItem::SUBTREE_ITEM)
    {
        SubtreeItem* subtreeItem = dynamic_cast<SubtreeItem*>(sakuraItem);
        return processSubtree(subtreeItem);
    }

    if(sakuraItem->getType() == SakuraItem::BLOSSOM_ITEM)
    {
        BlossomItem* blossomItem = dynamic_cast<BlossomItem*>(sakuraItem);
        return processBlossom(*blossomItem);
    }

    if(sakuraItem->getType() == SakuraItem::BLOSSOM_GROUP_ITEM)
    {
        BlossomGroupItem* blossomGroupItem = dynamic_cast<BlossomGroupItem*>(sakuraItem);
        return processBlossomGroup(*blossomGroupItem);
    }

    if(sakuraItem->getType() == SakuraItem::IF_ITEM)
    {
        IfBranching* ifBranching = dynamic_cast<IfBranching*>(sakuraItem);
        return processIf(ifBranching);
    }

    if(sakuraItem->getType() == SakuraItem::FOR_EACH_ITEM)
    {
        ForEachBranching* forEachBranching = dynamic_cast<ForEachBranching*>(sakuraItem);
        return processForEach(forEachBranching, false);
    }

    if(sakuraItem->getType() == SakuraItem::FOR_ITEM)
    {
        ForBranching* forBranching = dynamic_cast<ForBranching*>(sakuraItem);
        return processFor(forBranching, false);
    }

    if(sakuraItem->getType() == SakuraItem::PARALLEL_FOR_EACH_ITEM)
    {
        ForEachBranching* forEachBranching = dynamic_cast<ForEachBranching*>(sakuraItem);
        return processForEach(forEachBranching, true);
    }

    if(sakuraItem->getType() == SakuraItem::PARALLEL_FOR_ITEM)
    {
        ForBranching* forBranching = dynamic_cast<ForBranching*>(sakuraItem);
        return processFor(forBranching, true);
    }

    if(sakuraItem->getType() == SakuraItem::SEQUENTIELL_ITEM)
    {
        SequentiellPart* sequentiell = dynamic_cast<SequentiellPart*>(sakuraItem);
        return processSequeniellPart(sequentiell);
    }

    if(sakuraItem->getType() == SakuraItem::PARALLEL_ITEM)
    {
        ParallelPart* parallel = dynamic_cast<ParallelPart*>(sakuraItem);
        return processParallelPart(parallel);
    }

    if(sakuraItem->getType() == SakuraItem::SEED_ITEM)
    {
        SeedItem* forestItem = dynamic_cast<SeedItem*>(sakuraItem);
        TreeItem* branchItem = dynamic_cast<TreeItem*>(forestItem->child);
        return processTree(branchItem);
    }

    return false;
}

/**
 * @brief process single blossom
 *
 * @param blossomItem item with all information for the blossom
 *
 * @return true if successful, else false
 */
bool
SakuraThread::processBlossom(BlossomItem &blossomItem)
{
    // process values by filling with information of the parent-object
    std::string errorMessage = "";
    const bool result = fillInputValueItemMap(blossomItem.values, m_parentValues, errorMessage);
    if(result == false)
    {
        SakuraRoot::m_root->createError(blossomItem, "processing",
                                        "error while processing blossom items:\n"
                                        + errorMessage);
        blossomItem.success = false;
        return false;
    }

    // get and prcess the requested blossom
    Blossom* blossom = getBlossom(blossomItem.blossomGroupType, blossomItem.blossomType);
    if(blossom == nullptr)
    {
        SakuraRoot::m_root->createError(blossomItem, "processing", "unknow blossom-type");
        blossomItem.success = false;
        return blossomItem.success;
    }
    blossomItem.parentValues = &m_parentValues;
    blossom->growBlossom(blossomItem);
    delete blossom;

    // send result to root
    SakuraRoot::m_root->printOutput(blossomItem);

    // write processing result back to parent
    fillOutputValueItemMap(blossomItem.values, blossomItem.blossomOutput);

    // TODO: override only with the output-values to avoid unnecessary conflicts
    overrideItems(m_parentValues, blossomItem.values);

    return blossomItem.success;
}

/**
 * @brief process a group of blossoms
 *
 * @param blossomGroupItem object, which should be processed
 *
 * @return true if successful, else false
 */
bool
SakuraThread::processBlossomGroup(BlossomGroupItem &blossomGroupItem)
{
    std::string errorMessage = "";

    for(uint32_t i = 0; i < blossomGroupItem.blossoms.size(); i++)
    {
        BlossomItem* blossomItem = blossomGroupItem.blossoms.at(i);
        blossomItem->blossomGroupType = blossomGroupItem.blossomGroupType;
        blossomItem->nameHirarchie = m_hierarchy;

        // convert name as jinja2-string
        Jinja2Converter* converter = SakuraRoot::m_jinja2Converter;
        std::pair<bool, std::string> convertResult;
        convertResult = converter->convert(blossomGroupItem.id, &m_parentValues, errorMessage);
        if(convertResult.first == false)
        {
            SakuraRoot::m_root->createError("jinja2-converter", convertResult.second);
            return false;
        }

        blossomItem->nameHirarchie.push_back("BLOSSOM: " + convertResult.second);

        const bool result = processSakuraItem(blossomItem);
        if(result == false) {
            return false;
        }
    }

    return true;
}

/**
 * @brief process a new tree
 *
 * @param treeItem object, which should be processed
 *
 * @return true if successful, else false
 */
bool
SakuraThread::processTree(TreeItem* treeItem)
{
    const std::vector<std::string> uninitItems = checkItems(treeItem->values);
    if(uninitItems.size() > 0)
    {
        std::string message = "The following items are not initialized: \n";
        for(uint32_t i = 0; i < uninitItems.size(); i++)
        {
            message += uninitItems.at(i) + "\n";
        }
        SakuraRoot::m_root->createError("processing", message);
        return false;
    }

    for(uint32_t i = 0; i < treeItem->childs.size(); i++)
    {
        const bool result = processSakuraItem(treeItem->childs.at(i));;
        if(result == false) {
            return false;
        }
    }

    return true;
}

/**
 * @brief process a new subtree
 *
 * @param subtreeItem object, which should be processed
 *
 * @return true if successful, else false
 */
bool
SakuraThread::processSubtree(SubtreeItem* subtreeItem)
{
    std::string errorMessage = "";
    bool fillResult = false;

    SakuraItem* newSubtree = SakuraRoot::m_root->m_treeHandler->getTree(subtreeItem->nameOrPath);


    // fill normal map
    fillResult = fillInputValueItemMap(subtreeItem->values, m_parentValues, errorMessage);
    if(fillResult == false)
    {
        SakuraRoot::m_root->createError("subtree-processing",
                                        "error while processing blossom items:\n"
                                        + errorMessage);
        return false;
    }
    overrideItems(newSubtree->values, subtreeItem->values, false);

    // fill internal maps
    DataMap* internalMap = new DataMap();
    std::map<std::string, ValueItemMap>::iterator mapIt;
    for(mapIt = subtreeItem->internalSubtrees.begin();
        mapIt != subtreeItem->internalSubtrees.end();
        mapIt++)
    {
        DataMap* tempMap = new DataMap();

        std::map<std::string, ValueItem>::iterator valueIt;
        for(valueIt = mapIt->second.begin();
            valueIt != mapIt->second.end();
            valueIt++)
        {
            errorMessage = "";
            fillResult =  fillValueItem(valueIt->second, m_parentValues, errorMessage);
            if(fillResult == false)
            {
                SakuraRoot::m_root->createError("subtree-processing",
                                                "error while processing blossom items:\n"
                                                + errorMessage);
                return false;
            }

            tempMap->insert(valueIt->first, valueIt->second.item->copy());
        }

        internalMap->insert(mapIt->first, tempMap);
    }

    overrideItems(newSubtree->values, subtreeItem->values, false);
    newSubtree->values.insert("internal_subtypes", internalMap);
    overrideItems(m_parentValues, newSubtree->values, false);

    return processSakuraItem(newSubtree);
}

/**
 * @brief process a if-else-condition
 *
 * @param ifCondition object, which should be processed
 *
 * @return true if successful, else false
 */
bool
SakuraThread::processIf(IfBranching* ifCondition)
{
    // initialize
    std::string errorMessage = "";
    bool ifMatch = false;
    ValueItem valueItem;

    // get left side of the comparism
    if(fillValueItem(ifCondition->leftSide, m_parentValues, errorMessage) == false)
    {
        SakuraRoot::m_root->createError("subtree-processing",
                                        "error processing if-condition:\n"
                                        + errorMessage);
        return false;
    }
    const std::string  leftSide = ifCondition->leftSide.item->toValue()->toString();

    // get right side of the comparism
    if(fillValueItem(ifCondition->rightSide, m_parentValues, errorMessage) == false)
    {
        SakuraRoot::m_root->createError("subtree-processing",
                                        "error processing if-condition:\n"
                                        + errorMessage);
        return false;
    }
    const std::string  rightSide = ifCondition->rightSide.item->toValue()->toString();

    // compare based on the compare-type
    switch(ifCondition->ifType)
    {
        case IfBranching::EQUAL:
            {
                ifMatch = leftSide == rightSide;
                break;
            }
        case IfBranching::UNEQUAL:
            {
                ifMatch = leftSide != rightSide;
                break;
            }
        default:
            // not implemented
            assert(false);
            break;
    }

    // based on the result, process the if-subtree or the else-subtree
    if(ifMatch) {
        return processSakuraItem(ifCondition->ifContent);
    } else {
        return processSakuraItem(ifCondition->elseContent);
    }
}

/**
 * @brief process a for-each-loop
 *
 * @param subtree object, which should be processed
 * @param parallel true, to encapsulate each cycle of the loop into a subtree-object and add these
 *                 to the subtree-queue, to be processed by multiple worker-threads
 *
 * @return true if successful, else false
 */
bool
SakuraThread::processForEach(ForEachBranching* subtree,
                             bool parallel)
{
    std::string errorMessage = "";

    // initialize the array, over twhich the loop should iterate
    if(fillInputValueItemMap(subtree->iterateArray, m_parentValues, errorMessage) == false)
    {
        SakuraRoot::m_root->createError("subtree-processing",
                                        "error processing for-loop:\n"
                                        + errorMessage);
        return false;
    }
    DataArray* array = subtree->iterateArray.get("array")->toArray();

    // process content normal or parallel via worker-threads
    if(parallel == false)
    {
        // backup the parent-values to avoid permanent merging with loop-internal values
        DataMap preBalueBackup = m_parentValues;
        overrideItems(m_parentValues, subtree->values, false);

        for(uint32_t i = 0; i < array->size(); i++)
        {
            m_parentValues.insert(subtree->tempVarName, array->get(i), true);
            const bool result = processSakuraItem(subtree->content->copy());
            if(result == false) {
                return false;
            }
        }

        // restore the old parent values and update only the existing values with the one form the
        // loop. That way, variables like the counter-variable are not added to the parent.
        DataMap postBalueBackup = m_parentValues;
        m_parentValues = preBalueBackup;
        overrideItems(m_parentValues, postBalueBackup);
    }
    else
    {
        // copy the parent-values
        DataMap internalValues = m_parentValues;

        // create and initialize one counter-instance for all new subtrees
        SubtreeQueue::ActiveCounter* counter = new SubtreeQueue::ActiveCounter();
        counter->shouldCount = static_cast<uint32_t>(array->size());
        std::vector<SubtreeQueue::SubtreeObject*> spawnedObjects;

        // iterate of all items of the array and for each item, create a subtree-object
        // and add it to the subtree-queue for parallel processing
        for(uint32_t i = 0; i < array->size(); i++)
        {
            internalValues.insert(subtree->tempVarName, array->get(i)->copy(), true);
            SubtreeQueue::SubtreeObject* object = new SubtreeQueue::SubtreeObject();
            object->subtree = subtree->content->copy();
            object->items = internalValues;
            object->hirarchy = m_hierarchy;
            object->activeCounter = counter;
            m_queue->addSubtreeObject(object);
            spawnedObjects.push_back(object);
        }

        // wait until the created subtree was fully processed by the worker-threads
        while(counter->isEqual() == false) {
            std::this_thread::sleep_for(chronoMilliSec(10));
        }

        // post-processing and cleanup
        for(uint32_t i = 0; i < array->size(); i++)
        {
            std::string errorMessage = "";
            if(fillInputValueItemMap(subtree->values,
                                     spawnedObjects.at(static_cast<uint32_t>(i))->items,
                                     errorMessage) == false)
            {
                SakuraRoot::m_root->createError("subtree-processing",
                                                "error processing post-aggregation of for-loop:\n"
                                                + errorMessage);
                return false;
            }
        }
        overrideItems(m_parentValues, subtree->values, true);
    }

    return true;
}

/**
 * @brief process a for-loop
 *
 * @param subtree object, which should be processed
 * @param parallel true, to encapsulate each cycle of the loop into a subtree-object and add these
 *                 to the subtree-queue, to be processed by multiple worker-threads
 *
 * @return true if successful, else false
 */
bool
SakuraThread::processFor(ForBranching* subtree,
                         bool parallel)
{
    std::string errorMessage = "";

    // get start-value
    if(fillValueItem(subtree->start, m_parentValues, errorMessage) == false)
    {
        SakuraRoot::m_root->createError("subtree-processing",
                                        "error processing for-loop:\n"
                                        + errorMessage);
        return false;
    }
    const long startValue = subtree->start.item->toValue()->getLong();

    // get end-value
    if(fillValueItem(subtree->end, m_parentValues, errorMessage) == false)
    {
        SakuraRoot::m_root->createError("subtree-processing",
                                        "error processing for-loop:\n"
                                        + errorMessage);
        return false;
    }
    const long endValue = subtree->end.item->toValue()->getLong();

    // process content normal or parallel via worker-threads
    if(parallel == false)
    {
        // backup the parent-values to avoid permanent merging with loop-internal values
        DataMap preBalueBackup = m_parentValues;

        for(long i = startValue; i < endValue; i++)
        {
            // add the counter-variable as new value to be accessable within the loop
            m_parentValues.insert(subtree->tempVarName, new DataValue(i), true);

            const bool result = processSakuraItem(subtree->content);
            if(result == false) {
                return false;
            }
        }

        // restore the old parent values and update only the existing values with the one form the
        // loop. That way, variables like the counter-variable are not added to the parent.
        DataMap postBalueBackup = m_parentValues;
        m_parentValues = preBalueBackup;
        overrideItems(m_parentValues, postBalueBackup);
    }
    else
    {
        // copy the parent-values
        DataMap internalValues = m_parentValues;

        // create and initialize one counter-instance for all new subtrees
        SubtreeQueue::ActiveCounter* counter = new SubtreeQueue::ActiveCounter();
        counter->shouldCount = static_cast<uint32_t>(endValue - startValue);
        std::vector<SubtreeQueue::SubtreeObject*> spawnedObjects;

        for(long i = startValue; i < endValue; i++)
        {
            // add the counter-variable as new value to be accessable within the loop
            internalValues.insert(subtree->tempVarName, new DataValue(i), true);

            // encapsulate the content of the loop together with the values and the counter-object
            // as an subtree-object and add it to the subtree-queue
            SubtreeQueue::SubtreeObject* object = new SubtreeQueue::SubtreeObject();
            object->subtree = subtree->content->copy();
            object->items = internalValues;
            object->hirarchy = m_hierarchy;
            object->activeCounter = counter;
            m_queue->addSubtreeObject(object);
            spawnedObjects.push_back(object);
        }

        // wait until the created subtree was fully processed by the worker-threads
        while(counter->isEqual() == false) {
            std::this_thread::sleep_for(chronoMilliSec(10));
        }

        // post-processing and cleanup
        for(long i = startValue; i < endValue; i++)
        {
            std::string errorMessage = "";
            if(fillInputValueItemMap(subtree->values,
                                     spawnedObjects.at(static_cast<uint32_t>(i))->items,
                                     errorMessage) == false)
            {
                SakuraRoot::m_root->createError("subtree-processing",
                                                "error processing post-aggregation of for-loop:\n"
                                                + errorMessage);
                return false;
            }
        }
        overrideItems(m_parentValues, subtree->values, true);
    }

    return true;
}

/**
 * @brief process sequentiall part
 *
 * @param subtree object, which should be processed
 *
 * @return true if successful, else false
 */
bool
SakuraThread::processSequeniellPart(SequentiellPart* subtree)
{
    for(uint32_t i = 0; i < subtree->childs.size(); i++)
    {
        const bool result = processSakuraItem(subtree->childs.at(i));;
        if(result == false) {
            return false;
        }
    }

    return true;
}

/**
 * @brief process parallel part via worker-threads
 *
 * @param parallelPart object, which should be processed
 *
 * @return true if successful, else false
 */
bool
SakuraThread::processParallelPart(ParallelPart* parallelPart)
{
    // create and initialize all threads
    SubtreeQueue::ActiveCounter* counter = new SubtreeQueue::ActiveCounter();
    counter->shouldCount = static_cast<uint32_t>(parallelPart->childs.size());
    std::vector<SubtreeQueue::SubtreeObject*> spawnedObjects;

    // encapsulate each subtree of the paralle part as subtree-object and add it to the
    // subtree-queue for parallel processing
    for(uint32_t i = 0; i < parallelPart->childs.size(); i++)
    {
        SubtreeQueue::SubtreeObject* object = new SubtreeQueue::SubtreeObject();
        object->subtree = parallelPart->childs.at(i)->copy();
        object->hirarchy = m_hierarchy;
        object->items = m_parentValues;
        object->activeCounter = counter;
        m_queue->addSubtreeObject(object);
        spawnedObjects.push_back(object);
    }

    // wait until the created subtree was fully processed by the worker-threads
    while(counter->isEqual() == false) {
        std::this_thread::sleep_for(chronoMilliSec(10));
    }

    return true;
}

}
