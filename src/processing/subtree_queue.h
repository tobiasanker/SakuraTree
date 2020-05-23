/**
 * @file        subtree_queue.h
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

#ifndef SUBTREE_QUEUE_H
#define SUBTREE_QUEUE_H

#include <common.h>
#include <libKitsunemimiProjectNetwork/session.h>

namespace SakuraTree
{

class SubtreeQueue
{
public:
    SubtreeQueue();

    /**
     * @brief The ActiveCounter struct is only a simple thread-save counter. This counter should be
     *        increased, after the subtree was fully processed. All subtree-queue objects,
     *        which have the same source and belong to each other, share the same instance
     *        of this counter. With this, the source-thread should be able to check, that all its
     *        spawn subtree-objects have finished their task before increasing this counter.
     */
    struct ActiveCounter
    {
        std::mutex lock;
        uint32_t isCounter = 0;
        uint32_t shouldCount = 0;

        ActiveCounter() {}

        /**
         * @brief increase the counter
         */
        void increaseCounter()
        {
            lock.lock();
            isCounter++;
            lock.unlock();
        }

        /**
         * @brief check, that the counter has reached the expected value
         *
         * @return true, if counter has reached the expected value, else false
         */
        bool isEqual()
        {
            bool result = false;
            lock.lock();
            result = isCounter == shouldCount;
            lock.unlock();
            return result;
        }
    };

    /**
     * @brief The SubtreeObject struct is basically a container to encapsulate a task. It contains
     *        all necessary information to process a subtree. These container are placed in the
     *        queue, from where they are taken by the worker-threads.
     */
    struct SubtreeObject
    {
        // subtree, which should be processed by a worker-thread
        SakuraItem* subtree = nullptr;
        // map with all input-values for the subtree
        DataMap items;
        // shared counter-instance, which will be increased after the subtree was fully processed
        ActiveCounter* activeCounter = nullptr;
        // current position in the processing-hirarchy for status-output
        std::vector<std::string> hirarchy;

        Kitsunemimi::Project::Session* session = nullptr;
        uint64_t blockerId = 0;
    };

    void addSubtreeObject(SubtreeObject* newObject);
    SubtreeObject* getSubtreeObject();

private:
    std::mutex m_lock;
    std::queue<SubtreeObject*> m_queue;

};

}

#endif // SUBTREE_QUEUE_H
