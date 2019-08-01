/**
 *  @file    aptUpdateBlossom.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "aptUpdateBlossom.h"

namespace SakuraTree
{

AptUdateBlossom::AptUdateBlossom() :
    AptBlossom() {}

/**
 * initTask
 */
void
AptUdateBlossom::initTask(BlossomData *blossomData)
{
    blossomData->success = true;
}

/**
 * preCheck
 */
void
AptUdateBlossom::preCheck(BlossomData *blossomData)
{
    blossomData->success = true;
}

/**
 * runTask
 */
void
AptUdateBlossom::runTask(BlossomData *blossomData)
{
    std::string programm = "sudo apt-get update";
    runSyncProcess(blossomData, programm);
}

/**
 * postCheck
 */
void
AptUdateBlossom::postCheck(BlossomData *blossomData)
{
    blossomData->success = true;
}

/**
 * closeTask
 */
void
AptUdateBlossom::closeTask(BlossomData *blossomData)
{
    blossomData->success = true;
}

}
