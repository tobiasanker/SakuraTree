/**
 * @file        sakura_thread.h
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

#ifndef SAKURA_THREAD_H
#define SAKURA_THREAD_H

#include <common.h>
#include <libKitsunemimiCommon/threading/thread.h>
#include <items/sakura_items.h>

using namespace Kitsunemimi;

namespace SakuraTree
{
class SubtreeQueue;

class SakuraThread
        : public Kitsunemimi::Thread
{
public:
    SakuraThread(SubtreeQueue* queue);
    ~SakuraThread();

private:
    bool m_started = false;
    SubtreeQueue* m_queue;

    DataMap m_parentValues;
    std::vector<std::string> m_hierarchy;

    void run();

    bool processSakuraItem(SakuraItem* sakuraItem);

    bool processBlossom(BlossomItem &blossomItem);
    bool processBlossomGroup(BlossomGroupItem &blossomGroupItem);
    bool processTree(TreeItem* treeItem);
    bool processSubtree(SubtreeItem* subtreeItem);
    bool processIf(IfBranching* ifCondition);
    bool processForEach(ForEachBranching* subtree, bool parallel);
    bool processFor(ForBranching* subtree, bool parallel);
    bool processSequeniellPart(SequentiellPart* subtree);
    bool processParallelPart(ParallelPart* parallelPart);

};

}

#endif // SAKURA_THREAD_H
