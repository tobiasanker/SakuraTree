/**
 * @file        provision_branch_builder.cpp
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

#include "provision_branch_builder.h"
#include <items/sakura_items.h>

namespace SakuraTree
{

/**
 * @brief createProvisionBranch
 * @param address
 * @param port
 * @param userName
 * @param keyPath
 * @param sakaraTreePath
 * @param targetPath
 * @param subtree
 * @return
 */
BranchItem*
createProvisionBranch(const std::string &address,
                      const int port,
                      const std::string &userName,
                      const std::string &keyPath,
                      const std::string &sakaraTreePath,
                      const std::string &targetPath,
                      const std::string &subtree)
{
    BranchItem* item = new BranchItem();

    BlossomItem* updateBlossom = createAptUpdateBlossom(address,
                                                        port,
                                                        userName,
                                                        keyPath);
    item->childs.push_back(updateBlossom);
    BlossomItem* installBlossom = createAptLatesBlossom(address,
                                                        port,
                                                        userName,
                                                        keyPath);
    item->childs.push_back(installBlossom);

    BlossomItem* scpBlossom = createScpBlossom(address,
                                               port,
                                               userName,
                                               keyPath,
                                               sakaraTreePath,
                                               targetPath);
    item->childs.push_back(scpBlossom);

    BlossomItem* fileCreateBlossom = createSshServiceFileBlossom(address,
                                                                 port,
                                                                 userName,
                                                                 keyPath);
    item->childs.push_back(fileCreateBlossom);

    BlossomItem* serviceStartBlossom = createServiceStartBlossom(address,
                                                                 port,
                                                                 userName,
                                                                 keyPath);
    item->childs.push_back(serviceStartBlossom);

    BlossomItem* copySubtreeBlossom = createCopySubtreeBlossom(address, subtree);
    item->childs.push_back(copySubtreeBlossom);



    return item;
}

/**
 * @brief createAptUpdateBlossom
 * @param address
 * @param port
 * @param userName
 * @param keyPath
 * @return
 */
BlossomItem*
createAptUpdateBlossom(const std::string &address,
                       const int port,
                       const std::string &userName,
                       const std::string &keyPath)
{
    BlossomItem* item = new BlossomItem();

    item->blossomGroupType = "ssh";
    item->blossomType = "cmd";

    item->inputValues->insert("address", new DataValue(address));
    item->inputValues->insert("user", new DataValue(userName));
    item->inputValues->insert("port", new DataValue(port));
    item->inputValues->insert("ssh_key", new DataValue(keyPath));
    item->inputValues->insert("command", new DataValue("sudo apt-get update"));

    return item;
}

/**
 * @brief createAptLatesBlossom
 * @param address
 * @param port
 * @param userName
 * @param keyPath
 * @return
 */
BlossomItem*
createAptLatesBlossom(const std::string &address,
                      const int port,
                      const std::string &userName,
                      const std::string &keyPath)
{
    BlossomItem* item = new BlossomItem();

    item->blossomGroupType = "ssh";
    item->blossomType = "cmd";

    item->inputValues->insert("address", new DataValue(address));
    item->inputValues->insert("user", new DataValue(userName));
    item->inputValues->insert("port", new DataValue(port));
    item->inputValues->insert("ssh_key", new DataValue(keyPath));
    item->inputValues->insert("command", new DataValue("sudo apt-get install -y libboost-filesystem-dev libsqlite3-dev libboost-program-options-dev"));

    return item;
}


/**
 * @brief createScpBlossom
 * @param address
 * @param port
 * @param userName
 * @param keyPath
 * @param sakaraTreePath
 * @param targetPath
 * @return
 */
BlossomItem*
createScpBlossom(const std::string &address,
                 const int port,
                 const std::string &userName,
                 const std::string &keyPath,
                 const std::string &sakaraTreePath,
                 const std::string &targetPath)
{
    BlossomItem* item = new BlossomItem();

    item->blossomGroupType = "ssh";
    item->blossomType = "scp";

    item->inputValues->insert("address", new DataValue(address));
    item->inputValues->insert("user", new DataValue(userName));
    item->inputValues->insert("port", new DataValue(port));
    item->inputValues->insert("ssh_key", new DataValue(keyPath));
    item->inputValues->insert("source_path", new DataValue(sakaraTreePath));
    item->inputValues->insert("target_path", new DataValue(targetPath));

    return item;
}

/**
 * @brief createCopySubtreeBlossom
 * @param address
 * @param subtree
 * @return
 */
BlossomItem*
createCopySubtreeBlossom(const std::string &address,
                         const std::string &subtree)
{
    BlossomItem* item = new BlossomItem();

    item->blossomGroupType = "special";
    item->blossomType = "copy-subtree";

    item->inputValues->insert("address", new DataValue(address));
    item->inputValues->insert("subtree", new DataValue(subtree));

    return item;
}

BlossomItem*
createSshServiceFileBlossom(const std::string &address,
                            const int port,
                            const std::string &userName,
                            const std::string &keyPath)
{
    std::string fileContent = "[Unit]\n"
                              "Description=SakuraTree-Test\n"
                              "StartLimitIntervalSec=0\n"
                              "\n"
                              "[Service]\n"
                              "Type=simple\n"
                              "Restart=always\n"
                              "RestartSec=1\n"
                              "User=neptune\n"
                              "ExecStart=/home/neptune/SakuraTree --server-address 127.0.0.1 --server-port 1337\n"
                              "\n"
                              "[Install]\n"
                              "WantedBy=multi-user.target\n";

    BlossomItem* item = new BlossomItem();

    item->blossomGroupType = "ssh";
    item->blossomType = "file_create";

    item->inputValues->insert("address", new DataValue(address));
    item->inputValues->insert("user", new DataValue(userName));
    item->inputValues->insert("port", new DataValue(port));
    item->inputValues->insert("ssh_key", new DataValue(keyPath));
    item->inputValues->insert("file_content", new DataValue(fileContent));
    item->inputValues->insert("file_path", new DataValue("/etc/systemd/system/sakura_tree.service"));

    return item;
}

/**
 * @brief createServiceStartBlossom
 * @param address
 * @param port
 * @param userName
 * @param keyPath
 * @return
 */
BlossomItem*
createServiceStartBlossom(const std::string &address,
                          const int port,
                          const std::string &userName,
                          const std::string &keyPath)
{
    BlossomItem* item = new BlossomItem();


    item->blossomGroupType = "ssh";
    item->blossomType = "cmd";

    item->inputValues->insert("address", new DataValue(address));
    item->inputValues->insert("user", new DataValue(userName));
    item->inputValues->insert("port", new DataValue(port));
    item->inputValues->insert("ssh_key", new DataValue(keyPath));
    item->inputValues->insert("async", new DataValue(true));
    item->inputValues->insert("command", new DataValue("sudo systemctl start sakura_tree"));

    return item;
}


}
