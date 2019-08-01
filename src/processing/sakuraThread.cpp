﻿/**
 *  @file    sakuraThread.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "sakuraThread.h"
#include <sakuraRoot.h>
#include <commonMethods.h>
#include <processing/blossoms/blossom.h>
#include <processing/blossoms/blossomGetter.h>

#include <processing/blossoms/install/apt/aptBlossom.h>
#include <processing/blossoms/install/apt/aptPresentBlossom.h>
#include <processing/blossoms/install/apt/aptAbsentBlossom.h>
#include <processing/blossoms/install/apt/aptUpdateBlossom.h>

namespace SakuraSeed
{

/**
 * constructor
 */
SakuraThread::SakuraThread(JsonObject *growPlan,
                           JsonObject *values,
                           const std::vector<std::string> &hirarchie)
{
    assert(growPlan != nullptr);
    assert(values != nullptr);

    m_values = values;
    m_growPlan = growPlan;
    m_hirarchie = hirarchie;
}

/**
 * destructor
 */
SakuraThread::~SakuraThread()
{
    clearChilds();
}

/**
 * wait until the thread was started
 */
void
SakuraThread::waitUntilStarted()
{
    while(m_started == false)
    {
        std::this_thread::sleep_for(chronoMicroSec(10));
    }
}

/**
 * check if the thread was aborted
 *
 * @return true, if thread was aborted, else false
 */
bool
SakuraThread::isAborted() const
{
    return m_abort;
}

/**
 * start thread
 */
void
SakuraThread::run()
{
    m_started = true;
    grow(m_growPlan, m_values, m_hirarchie);
}

/**
 * central method of the thread to process the current part of the execution-tree
 */
void
SakuraThread::grow(JsonObject* growPlan,
                   JsonObject* values,
                   const std::vector<std::string> &hirarchie)
{
    if(m_abort) {
        return;
    }

    JsonObject* items = values;
    if(growPlan->contains("items")) {
        items = dynamic_cast<JsonObject*>(growPlan->get("items"));
    }

    if(growPlan->contains("items-input"))
    {
        JsonObject* itemsInput = dynamic_cast<JsonObject*>(growPlan->get("items-input"));
        itemsInput = fillItems(itemsInput, values);
        items = overrideItems(items, itemsInput);
    }

    if(growPlan->getString("type") == "blossom")
    {
        items = fillItems(items, values);
        std::vector<std::string> newHirarchie = hirarchie;
        newHirarchie.push_back("BLOSSOM: " + growPlan->getString("name"));
        processBlossom(growPlan, items, newHirarchie);
        return;
    }

    if(growPlan->getString("type") == "branch")
    {
        std::vector<std::string> newHirarchie = hirarchie;
        newHirarchie.push_back("BRANCH: " + growPlan->getString("name"));
        processBranch(growPlan, items, newHirarchie);
        return;
    }

    if(growPlan->getString("type") == "tree")
    {
        std::vector<std::string> newHirarchie = hirarchie;
        newHirarchie.push_back("TREE: " + growPlan->getString("name"));
        processParallelPart(growPlan, items, newHirarchie);
        return;
    }

    if(growPlan->getString("type") == "forest")
    {
        std::vector<std::string> newHirarchie = hirarchie;
        newHirarchie.push_back("FOREST: " + growPlan->getString("name"));
        processForest(growPlan, items, newHirarchie);
        return;
    }

    if(growPlan->getString("type") == "sequentiell")
    {
        processSequeniellPart(growPlan, items, hirarchie);
        return;
    }

    if(growPlan->getString("type") == "parallel")
    {
        processParallelPart(growPlan, items, hirarchie);
        return;
    }

    return;
}

/**
 * @brief SakuraThread::processBlossom
 */
void
SakuraThread::processBlossom(JsonObject* growPlan,
                             JsonObject* values,
                             const std::vector<std::string> &hirarchie)
{
    // init
    BlossomData blossomData;
    blossomData.name = growPlan->getString("name");
    blossomData.settings = dynamic_cast<JsonObject*>(growPlan->get("common-settings"));
    blossomData.items = values;
    blossomData.nameHirarchie = hirarchie;
    std::string type = growPlan->getString("blossom-type");

    // iterate over all subtypes and execute each as separate blossom
    JsonArray* subtypes = dynamic_cast<JsonArray*>(growPlan->get("blossom-subtypes"));
    for(uint32_t i = 0; i < subtypes->getSize(); i++)
    {
        std::string subtype = subtypes->get(i)->toString();
        Blossom* blossom = getBlossom(type, subtype);
        blossom->growBlossom(&blossomData);
        delete blossom;
    }

    // abort if blossom-result was an error
    if(blossomData.success == false)
    {
        m_abort = true;
        std::string output = "ABORT after ERROR";
        SakuraRoot::m_root->addMessage(&blossomData);
    }

    // send result to root
    SakuraRoot::m_root->addMessage(&blossomData);

    return;
}

/**
 * @brief SakuraThread::processBranch
 */
void
SakuraThread::processBranch(JsonObject *growPlan,
                            JsonObject *values,
                            const std::vector<std::string> &hirarchie)
{
    JsonItem* parts = growPlan->get("parts");
    assert(parts != nullptr);
    for(uint32_t i = 0; i < parts->getSize(); i++)
    {
        grow(dynamic_cast<JsonObject*>(parts->get(i)),
             values,
             hirarchie);
    }

    return;
}

/**
 * @brief SakuraThread::processForest
 */
void
SakuraThread::processForest(JsonObject* growPlan,
                            JsonObject* values,
                            const std::vector<std::string> &hirarchie)
{
    JsonItem* parts = growPlan->get("parts");
    assert(parts != nullptr);
    for(uint32_t i = 0; i < parts->getSize(); i++)
    {
        grow(dynamic_cast<JsonObject*>(parts->get(i)),
             values,
             hirarchie);
    }

    return;
}

/**
 * @brief SakuraThread::processSequeniellPart
 */
void
SakuraThread::processSequeniellPart(JsonObject *growPlan,
                                    JsonObject *values,
                                    const std::vector<std::string> &hirarchie)
{
    JsonItem* parts = growPlan->get("parts");
    assert(parts != nullptr);
    for(uint32_t i = 0; i < parts->getSize(); i++)
    {
        grow(dynamic_cast<JsonObject*>(parts->get(i)),
             values,
             hirarchie);
    }

    return;
}

/**
 * @brief SakuraThread::processParallelPart
 */
void
SakuraThread::processParallelPart(JsonObject* growPlan,
                                  JsonObject* values,
                                  const std::vector<std::string> &hirarchie)
{
    JsonItem* parts = growPlan->get("parts");
    assert(parts != nullptr);

    // create and initialize all threads
    for(uint32_t i = 0; i < parts->getSize(); i++)
    {
        SakuraThread* child = new SakuraThread(dynamic_cast<JsonObject*>(parts->get(i)),
                                               values,
                                               hirarchie);
        m_childs.push_back(child);
        child->start();
    }

    // wait for the end of all threads
    for(uint32_t i = 0; i < m_childs.size(); i++)
    {
        // wait the process is startet, so the join-method can not be called
        // before the new thread was really started
        m_childs.at(i)->waitUntilStarted();

        // finish the threads after they done their tasks
        m_childs.at(i)->waitForFinish();

        // check if any child was aborted
        if(m_childs.at(i)->isAborted()) {
            m_abort = true;
        }
    }
    clearChilds();

    return;
}

/**
 * stop and delete all child processes
 */
void
SakuraThread::clearChilds()
{
    for(uint32_t i = 0; i < m_childs.size(); i++)
    {
        m_childs.at(i)->stop();
        delete m_childs[i];
    }
    m_childs.clear();
}

}
