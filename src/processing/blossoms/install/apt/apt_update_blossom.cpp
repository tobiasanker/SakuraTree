/**
 *  @file    apt_update_blossom.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "apt_update_blossom.h"
#include <processing/blossoms/install/apt/apt_methods.h>
#include <processing/process_methods.h>

namespace SakuraTree
{

AptUdateBlossom::AptUdateBlossom() :
    Blossom() {}

/**
 * initTask
 */
void
AptUdateBlossom::initTask(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * preCheck
 */
void
AptUdateBlossom::preCheck(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * runTask
 */
void
AptUdateBlossom::runTask(BlossomItem &blossomItem)
{
    std::string programm = "sudo apt-get update";
    runSyncProcess(blossomItem, programm);
}

/**
 * postCheck
 */
void
AptUdateBlossom::postCheck(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

/**
 * closeTask
 */
void
AptUdateBlossom::closeTask(BlossomItem &blossomItem)
{
    blossomItem.success = true;
}

}
