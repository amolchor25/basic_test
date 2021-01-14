/*****************************************************************//**
 *
 * @file Loop.h
 * @brief Loop does component monitoring.
 * @copyright Copyright 2018 by Honeywell International Inc.
 * All rights reserved.  This software and code comprise proprietary
 * information of Honeywell International Inc.  This software and code
 * may not be reproduced, used, altered, re-engineered, distributed or
 * disclosed to others without the written consent of Honeywell.
 ********************************************************************/

#ifndef PLATFORM_WATCHDOG_INCLUDE_H
#define PLATFORM_WATCHDOG_INCLUDE_H

#include "Timer/Timer.hpp"
#include"Component/Component.h"
#include "Guard/Guard.hpp"
#include "ComponentManager/ComponentManager.h"

#include <systemd/sd-daemon.h>
#include <atomic>

namespace Platform
{
/**
 *  @brief Loop to monitor component and send watchdog signal to systemd
 *  Incase if any component is in not okay state,watchdog signal to systemd is not send
 *  and this results in application restart.
 */

class Loop :public Platform::Component
{
public:
    Loop(ComponentManager& componentManager):m_componentManager(componentManager){};

    virtual ~Loop() = default;

    /**
    * @brief Start worker thread do monitor component status
    */
    void Start() override
    {
        Platform::Notifier workerData;
        workerData.Connect(this,&Loop::Worker);

        m_timer = Platform::Timer<>{m_notificationIntervalMs,GlobalDataType::Timer::AlarmType::PERIODIC,workerData};
        m_timer.Start();
        Platform::Component::Start();
    }


    /**
    * @brief Worker thread do monitor component status
    */
    void Worker()
    {
        sd_notify (0, "READY=1");
        // termination message verification to be added
        /* Now go for periodic notification */
        if(m_componentManager.IsStatusGood())
        {
            /* Way to ping systemd */
            sd_notify (0, "WATCHDOG=1");
        }

    }

    /**
    * @brief terminate worker thread
    */
    void Stop() override
    {
        m_terminate = true;
        Platform::Component::Stop();
        m_timer.Stop();
    }

protected:
    /// Loop monitor interval, ping to systemd to happen every 30 seconds
    const uint32_t m_notificationIntervalMs = 1000*30;

    Platform::Timer<> m_timer;

    ComponentManager& m_componentManager;

    std::atomic<bool> m_terminate{false};
};

} //end of Platfrom

#endif //PLATFORM_WATCHDOG_INCLUDE_H
