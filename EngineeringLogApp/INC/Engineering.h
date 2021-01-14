/*****************************************************************//**
 *
 * @file    AuditLogApp.h
 * @brief   Module communicator class for receiving and forwarding messages.
 *          It is also responsible for Module discovery and monitoring
 *
 * @copyright Copyright 2018 by Honeywell International Inc.
 * All rights reserved.  This software and code comprise proprietary
 * information of Honeywell International Inc.  This software and code
 * may not be reproduced, used, altered, re-engineered, distributed or
 * disclosed to others without the written consent of Honeywell.
 ********************************************************************/


#ifndef AUDIT_LOG_APP__H
#define AUDIT_LOG_APP__H

#include "Application/Application.h"
#include "Watchdog/Watchdog.h"
#include "ShutdownMonitor/ShutdownMonitor.h"
#include "Utility.h"
#include "LogLevel/LogLevel.h"
#include "DOL/Entities/Module/CPUModule.h"
#include "DOL/Entities/Module.h"
#include "EventRouter/EventRouterStateHandler.h"
#include "CommonDef.h"
#include <fstream>
#include <dirent.h>
#include "Mol/Commands/CommandCategory.h"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

namespace TestApplication
{
    enum class ENGLOG_STATE:uint8_t
    {
        ENGLOG_INIT=0,
        ENGLOG_START,
        ENGLOG_TXFER,
        ENGLOG_STOP,
        ENGLOG_FAIL,
        ENGLOG_SUCCESS,
        END_OF_LIST

    };
class FusionTestApp : public Platform::Application
{

 public:
    FusionTestApp(PROC_ADDRESS address, uint64_t sourceID )
            : Application{address, sourceID}
    {
        AddComponent(std::make_shared<Platform::Watchdog>(m_componentManager));
        AddComponent(std::make_shared<Platform::ShutdownMonitor>(m_looper));
        AddComponent(std::make_shared<Platform::LogLevel>());
    }
    ~FusionTestApp() override =  default;

    /**
     * @brief Init Initializes the Application to connect to the Message
              Notifiers to send all kinds of Messages like Events,Commands
              etc to other Applications
     *
     */
    void Init() override
    {

        Platform::Application::Init();

    }
 
    /**
     * @brief Prepare
     */
    void Prepare() final
    {
        m_communicator.m_messageTransporter.Connect(PROC_ADDRESS::NETWORK);
        m_communicator.m_messageTransporter.Connect(PROC_ADDRESS::EVENTLOGAPP);
        m_communicator.m_messageTransporter.Connect(PROC_ADDRESS::MOL_RECEIVER);
        m_communicator.m_messageTransporter.Connect(PROC_ADDRESS::FIRMWAREAPP);
       
        Platform::Notifier notifier;
        notifier.Connect(this, &FusionTestApp::timerFunction);
        m_timer = Platform::Timer<>(5000, GlobalDataType::Timer::AlarmType::PERIODIC, notifier);
    
        m_communicator.m_messageTransporter.m_notifiers[Platform::Message::MessageType::DOL].Connect(this,&FusionTestApp::Readfunction);
        m_communicator.m_messageTransporter.m_notifiers[Platform::Message::MessageType::EVENT].Connect(this,&FusionTestApp::Readfunction);
        m_communicator.m_messageTransporter.m_notifiers[Platform::Message::MessageType::COMMAND].Connect(this,&FusionTestApp::Readfunction);
        m_communicator.m_messageTransporter.m_notifiers[Platform::Message::MessageType::REQUEST].Connect(this,&FusionTestApp::Readfunction);
        m_communicator.m_messageTransporter.m_notifiers[Platform::Message::MessageType::RESPONSE].Connect(this,&FusionTestApp::Readfunction);       
        Platform::Application::Prepare();
    }

    virtual void Start() final
    {
        m_timer.Start();
        Platform::Application::Start();
    }

    /**
     * @brief Shutdown
     */
    void Shutdown() override
    {
        m_communicator.m_messageTransporter.Disconnect(PROC_ADDRESS::NETWORK);
        m_communicator.m_messageTransporter.Disconnect(PROC_ADDRESS::FIRE_DOMAIN_APP);
        m_communicator.m_messageTransporter.Disconnect(PROC_ADDRESS::EVENTLOGAPP);
        m_communicator.m_messageTransporter.Disconnect(PROC_ADDRESS::MOL_RECEIVER);
        m_communicator.m_messageTransporter.Disconnect(PROC_ADDRESS::ENGLOG_APP);
        m_communicator.m_messageTransporter.m_notifiers[Platform::Message::MessageType::DOL].Disconnect(this,&FusionTestApp::Readfunction);
        m_communicator.m_messageTransporter.m_notifiers[Platform::Message::MessageType::EVENT].Disconnect(this,&FusionTestApp::Readfunction);
        m_communicator.m_messageTransporter.m_notifiers[Platform::Message::MessageType::COMMAND].Disconnect(this,&FusionTestApp::Readfunction);
        m_communicator.m_messageTransporter.m_notifiers[Platform::Message::MessageType::REQUEST].Disconnect(this,&FusionTestApp::Readfunction);
        m_communicator.m_messageTransporter.m_notifiers[Platform::Message::MessageType::RESPONSE].Disconnect(this,&FusionTestApp::Readfunction);
        m_timer.Stop();
        m_timer.Shutdown();
        Platform::Application::Shutdown();
    }

    void Readfunction(std::shared_ptr<Platform::Message> message)
    {
        //DEBUGPRINT(DEBUG_INFO,"/******* Check Function ********/");
        std::cout<<"/******* Received data ********/"<<std::endl;
        Dol::Translator<Platform::Message,Platform::Message> messageTranslator;
        Dol::Translator<Platform::Message,Platform::Message> eventTranslator;
        std::string packedData = messageTranslator.DomainObjectMessageToString(message);
        DEBUGPRINT(DEBUG_INFO, "NW:MolDolMessage received size[{0}]",(uint32_t)message->m_Size);
        DEBUGPRINT(DEBUG_INFO, "NW:MolDolMessage Message type[{0}]",(uint32_t)message->m_messageType);
        DEBUGPRINT(DEBUG_INFO, "NW:MolDolMessage DestinationID[{0}]",(uint64_t)message->m_DestinationUniqueID);
        DEBUGPRINT(DEBUG_INFO, "NW:MolDolMessage SourceID[{0}]",(uint64_t)message->m_SourceUniqueID);
        DEBUGPRINT(DEBUG_INFO, "NW:MolDolMessage Data[{0}]",(std::string)message->m_Data);
        DEBUGPRINT(DEBUG_INFO, "NW:MolDolMessage SenderProcAddr[{0}]",(uint32_t)message->m_sender);
        switch (message->m_messageType)
        {
            case (Platform::Message::MessageType::EVENT):
            {
                DEBUGPRINT(DEBUG_INFO,"Received EVENT");
                Dol::Translator<Mol::Event::InformationEvent, Mol::Event::InformationEvent> eventTranslator;
                std::shared_ptr<Mol::Event::InformationEvent> recvInfo = eventTranslator.StringToDomainObjectMessage(message->m_Data);
                processInformationEvent(recvInfo);
                break;
            }
            case (Platform::Message::MessageType::COMMAND):
            {
                using CommandMessageType  = Mol::Message<Mol::Command::COMMAND_CATEGORY>;
                Dol::Translator<CommandMessageType, CommandMessageType> m_commandTranslator;
                DEBUGPRINT(DEBUG_INFO,"Received COMMAND");
                std::shared_ptr<Mol::Message<Mol::Command::COMMAND_CATEGORY>> command;
                command = m_commandTranslator.StringToDomainObjectMessage(message->m_Data);
                //For testing using command as SET_DEBUG_LEVEL but actual implementaion will use something like AUDITLOG_TRANSFER COMMAND
                if(command->GetObjectType()==Mol::Command::COMMAND_CATEGORY::SET_DEBUG_LEVEL)
                {
                    DEBUGPRINT(DEBUG_INFO,"RECEIVED REQUEST for ENGLOG Encrypt !!!");
                    EngReq=true;
                }
                break;
            }
            default:
            {
                DEBUGPRINT(DEBUG_ERROR, "Auditlog unsupported Message[{}]", static_cast<int>(message->m_messageType));
                break;
            }
        }
    }
    /**************************************************************************************************
    *   @brief Periodic Function called on timer
    **************************************************************************************************/
    void timerFunction()
    {
        // Initiate statemachine only on receiving auditlog request  & cleared on success/failure
        if (EngReq==true)
        {
            EngineeringLog_statemachine();
        }
    }
/**************************************************************************************************
     * @brief: Decode the information events for audit logs
     **************************************************************************************************/
    void processInformationEvent(std::shared_ptr<Mol::Event::InformationEvent> message)
    {
        switch (message->GetEventCode())
        {
            // case Mol::Event::INFORMATION_EVENT_CODE::USB_MOUNT_SUCCESS:
            // {
            //     DEBUGPRINT(DEBUG_INFO,"Received EVENT USB Mount");
            //     if(adtState==AUDITLOG_STATE::ADTLOG_START)
            //     {
            //         DEBUGPRINT(DEBUG_INFO,"Mount sucessfully");
            //         adtState=AUDITLOG_STATE::ADTLOG_TXFER;
            //     }
            //     break;
            // }
            // case Mol::Event::INFORMATION_EVENT_CODE::USB_MOUNT_FAILURE:
            // {    
            //     DEBUGPRINT(DEBUG_INFO,"Received EVENT USB Mount failure");
            //     if((adtState==AUDITLOG_STATE::ADTLOG_START)||(adtState==AUDITLOG_STATE::ADTLOG_STOP))
            //     {   
            //         //SendEvent(Mol::Event::INFORMATION_EVENT_CODE::USB_MOUNT_FAILURE,PROC_ADDRESS::NETWORK);
            //         adtState=AUDITLOG_STATE::ADTLOG_FAIL;
            //     }
            //     break;
            // }
            // case Mol::Event::INFORMATION_EVENT_CODE::USB_DISCONNECTED:
            // case Mol::Event::INFORMATION_EVENT_CODE::USB_UNMOUNT_SUCCESS:
            // {
            //     if(adtState==AUDITLOG_STATE::ADTLOG_STOP)
            //     {
            //        DEBUGPRINT(DEBUG_INFO,"Unmount sucessfully");
            //         adtState=AUDITLOG_STATE::ADTLOG_SUCCESS;
            //     }
            //     break;
            // }
            // case Mol::Event::INFORMATION_EVENT_CODE::INVALID_USB_CONNECTED:
            // {
            //     DEBUGPRINT(DEBUG_INFO,"Received EVENT USB fail");
            //     if((adtState==AUDITLOG_STATE::ADTLOG_START) || (adtState==AUDITLOG_STATE::ADTLOG_STOP))
            //     {
            //         adtState=AUDITLOG_STATE::ADTLOG_FAIL;
            //     }
            //     break;
            // }
            case Mol::Event::INFORMATION_EVENT_CODE::HONEYWELL_AUDIT_MODE:
            {
                DEBUGPRINT(DEBUG_INFO,"Received HONEYWELL_AUDIT_MODE EVENT");
                // if((adtState==AUDITLOG_STATE::ADTLOG_START) || (adtState==AUDITLOG_STATE::ADTLOG_STOP))
                // {
                //     adtState=AUDITLOG_STATE::ADTLOG_FAIL;
                // }
                EngReq = true;
                break;
            }
            default:
            {
                DEBUGPRINT(DEBUG_INFO,"Unsupported Event for auditlog Received Eventcode{}",(uint32_t)message->GetEventCode());
                break;
            }
        }
    }
    /***************************************************************************************************
    * @brief Function to send events
    ***************************************************************************************************/
    void SendEvent(Mol::Event::INFORMATION_EVENT_CODE state,PROC_ADDRESS dest)
    {
        auto fault = std::make_shared<Mol::Event::InformationEvent>(state);
        Mol::DataType::ObjectReference source{GetSourceID(), Dol::DOMAIN_OBJECT_TYPE::CPU_MODULE};
        fault->SetSource(source);
        m_communicator.m_event.Send(fault, dest);
    }
    void handleErrors(void)
    {
        ERR_print_errors_fp(stderr);
        // abort();
    }

    /**************************************************************************************************
     * @brief: Auditlog statemachine for transfer logs to USB drive 
     **************************************************************************************************/
    void EngineeringLog_statemachine(void)
    {

        switch (EngState)
        {
            case ENGLOG_STATE::ENGLOG_INIT:
            {
                EngState = ENGLOG_STATE::ENGLOG_START;

                break;
            }
            case ENGLOG_STATE::ENGLOG_START:
            {
                std::cout << "Start encryption mechanism" << std::endl;
                uint8_t retValue = Utility::RunShellCommand("cd /userdata/var/log/ && tar cz messages | openssl enc -aes-256-cbc -e -pass pass:honeywell > /userdata/messages.tar.gz.enc");
                if (retValue != SUCCESS)
                {
                    DEBUGPRINT(DEBUG_INFO, "encryption failed for messages %d", retValue);
                    EngState = ENGLOG_STATE::ENGLOG_FAIL;
                }
                else
                {
                    DEBUGPRINT(DEBUG_INFO, "File Encryption completed for engineering logs");
                }
                EngState = ENGLOG_STATE::ENGLOG_SUCCESS;
                break;
            }
            case ENGLOG_STATE::ENGLOG_SUCCESS:
            {
                DEBUGPRINT(DEBUG_INFO, "Engineering log task successfully completed !!!");
                EngState = ENGLOG_STATE::ENGLOG_INIT;
                overflowFlag = false;
                thresholFlag = false;
                EngReq = false;
                break;
            }
            default:
            {
                DEBUGPRINT(DEBUG_INFO, "Unsupported state");
                break;
            }
        }
    }

protected:

    Platform::Timer<> m_timer;
    bool thresholFlag{false};
    bool overflowFlag{false};
    bool EngReq{false};
    ENGLOG_STATE EngState=ENGLOG_STATE::ENGLOG_INIT;
};

} // end of EngineeringLogApp
#endif //ENGINEERING_LOG_APPLICATION__H
