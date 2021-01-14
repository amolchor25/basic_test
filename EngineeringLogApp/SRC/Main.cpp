/**********************************************************************************
* @file Main.cpp
* @brief Event logger application module main function which does the initialisations
* and calls the processFunc() for handling further operations
*
* @copyright Copyright 2017 by Honeywell International Inc.
* All rights reserved.  This software and code comprise proprietary
* information of Honeywell International Inc.  This software and code
* may not be reproduced, used, altered, reengineered, distributed or
* disclosed to others without the written consent of Honeywell.
**********************************************************************************/
#include <sys/resource.h>

#include "Engineering.h"
#include "CommonDef.h"
#include "Utility.h"
#include "DebugPrint/DEBUGPRINT.hpp"
#include "EventUnpacker/EventUnpacker.h"
#include "Mol/Events/EventTypeList.h"
#include<iostream>
#include<ctime>
/**
 * @brief   Event Provider main function
 */
int main()
{
    DEBUGPRINT::SetDebugLevel(DEBUG_ALL);
    DEBUGPRINT::SetOutStream(OutStream::STDOUT);

    struct rlimit rlim;
    rlim.rlim_cur = RLIM_INFINITY;
    rlim.rlim_max = RLIM_INFINITY;
    const uint64_t Panel_ID=Utility::ReadPanelID();
    if (setrlimit(RLIMIT_MSGQUEUE, &rlim) == -1)
    {
        DEBUGPRINT(DEBUG_ERROR, "EL:setrlimit failed");
    }
    try
    {
        TestApplication::FusionTestApp fusionTestApp(PROC_ADDRESS::ENGLOG_APP, Panel_ID);


        fusionTestApp.Init();
        fusionTestApp.Prepare();
        fusionTestApp.Start();
        fusionTestApp.Stop();
        fusionTestApp.Shutdown();
        fusionTestApp.Uninit();

        return SUCCESS;
    } catch (Platform::ERROR& error)
    {
        DEBUGPRINT(DEBUG_INFO, "FusionTestApp : Main constructor error[{}]",(int)error);
        return ERROR_VALUE;
    }

}

