/**
 *  @file    sakura_root.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "sakura_root.h"
#include <jinja2_converter.h>

#include <initializing/sakura_compiler.h>
#include <common_methods.h>
#include <processing/sakura_thread.h>

namespace SakuraTree
{

SakuraRoot* SakuraRoot::m_root = nullptr;
Jinja2Converter* SakuraRoot::m_jinja2Converter = nullptr;

/**
 * constructor
 */
SakuraRoot::SakuraRoot()
{
    m_root = this;
    m_jinja2Converter = new Kitsune::Jinja2::Jinja2Converter;
}

/**
 * destructor
 */
SakuraRoot::~SakuraRoot()
{
    if(m_rootThread != nullptr)
    {
        m_rootThread->stop();
        delete m_rootThread;
        m_rootThread = nullptr;
    }
}

/**
 * initialize and start rollout-process
 *
 * @return true if successful, else false
 */
bool
SakuraRoot::startProcess(const std::string &rootPath,
                         std::string seedName)
{
    // parsing
    SakuraConverter* sakuraParser = new SakuraConverter(DEBUG);
    SakuraCompiler compiler(sakuraParser);
    DataMap* processPlan = compiler.compile(rootPath, seedName);

    // debug-output
    if(DEBUG)
    {
        std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
        std::string output = processPlan->toString(true);
        std::cout<<output<<std::endl;
        std::cout<<"-----------------------------------------------------"<<std::endl;
    }

    // run process
    DataMap* dummyObj = new DataMap();
    m_rootThread = new SakuraThread(processPlan, dummyObj, std::vector<std::string>());
    m_rootThread->start();
    m_rootThread->waitUntilStarted();
    m_rootThread->waitForFinish();

    std::cout<<"finish"<<std::endl;

    return true;
}

/**
 * @brief SakuraRoot::addMessage
 */
void
SakuraRoot::addMessage(BlossomData* blossomData)
{
    m_mutex.lock();
    std::cout<<" "<<std::endl;
    printOutput(blossomData);
    m_mutex.unlock();
}

}
