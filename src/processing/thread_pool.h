/**
 * @file        thread_pool.h
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

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <common.h>
#include <libKitsunemimiCommon/threading/thread.h>
#include <sakura_root.h>
#include <processing/subtree_queue.h>

class SakuraThread;

class ThreadPool
{
public:
    ThreadPool(const uint32_t numberOfThreads);
    ~ThreadPool();

    SubtreeQueue m_queue;

private:
    void clearChildThreads();

    std::vector<SakuraThread*> m_childThreads;
};

#endif // THREAD_POOL_H
