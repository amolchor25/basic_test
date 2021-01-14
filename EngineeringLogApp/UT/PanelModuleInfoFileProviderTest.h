/******************************************************************************//**
* @file PanelModuleInfoFileProviderTest.h
* @brief Test case verify EventProviderService Class.
*
* @copyright Copyright 2020 by Honeywell International Inc.
* All rights reserved.  This software and code comprise proprietary
* information of Honeywell International Inc.  This software and code
* may not be reproduced, used, altered, reengineered, distributed or
* disclosed to others without the written consent of Honeywell.
**********************************************************************************/

#ifndef PLATFORM_TEST_MAIN_CPU_INFO_H
#define PLATFORM_TEST_MAIN_CPU_INFO_H

#include "PanelModuleInfoFileProvider/PanelModuleInfoFileProvider.h"
#include "tinyxml2.h"
#include "tixml2ex.h"

class PanelModuleInfoFileProviderTest : public Platform::PanelModuleInfoFileProvider
{
public:
    PanelModuleInfoFileProviderTest():
        PanelModuleInfoFileProvider()
    {

    }

    ~PanelModuleInfoFileProviderTest() override = default;

    ERROR_CODE CreateResponseFileTest(const std::string& fileName, FILE_RESPONSE_TYPE responseType)
    {
        return CreateResponseFile(fileName,responseType);
    }

    ERROR_CODE CreateXmlResponseTest()
    {
        return CreateXmlResponse();
    }

    std::string GetFileName()
    {
        return m_fileName;
    }

    FILE_RESPONSE_TYPE GetResponseType()
    {
        return m_responseType;
    }

    uint32_t GetNumberOfModules()
    {
        return m_modulesinfo.size();
    }


    tinyxml2::XMLElement * ModuleDolToXmlElementTest(std::shared_ptr<Dol::Entities::Module> moduleObject, tinyxml2::XMLDocument& m_doc)
    {
        return ModuleDolToXmlElement(moduleObject, m_doc);
    }

    void SetFileNameAndFileFormat(const std::string& fileName, FILE_RESPONSE_TYPE responseType)
    {
        m_fileName = fileName;
        m_responseType = responseType;
    }

    void AddModuleElement(std::shared_ptr<Dol::Entities::Module> moduleObject)
    {
        m_modulesinfo.emplace(moduleObject);
    }

    std::shared_ptr<Dol::Entities::Module> CrateModuleObjectForTest(const std::string& ipv6_address,
            uint64_t id,
            Dol::Entities::Module::MODULE_TYPE type,
            const std::string& lable,
            const std::string& serialNumber,
            const std::string& hardwareVersion,
            Dol::Entities::Module::Status status,
            const std::string& softwareVersion,
            const std::string& configVersion)
    {
        auto module =  std::make_shared<Dol::Entities::Module>(ipv6_address,id);
        module->SetType(type);
        module->SetLabel(lable);
        module->SetSerialNumber(serialNumber);
        module->SetHardwareVersion(hardwareVersion);
        module->SetModuleStatus(status);
        auto version = Utility::GetSemanticVersionUsingRegex(softwareVersion);
        module->SetConfigurationVersion(std::make_shared<Dol::Entities::SemanticVersion>(version));
        version = Utility::GetSemanticVersionUsingRegex(configVersion);
        module->SetSoftwareVersion(std::make_shared<Dol::Entities::SemanticVersion>(version));
        return module;
    }

    std::shared_ptr<Mol::Event::InformationEvent> CrateInformationEvent(Mol::Event::INFORMATION_EVENT_CODE code, std::shared_ptr<Dol::Entities::Module> module, uint64_t sourceID, Mol::DataType::Parameter::CATALOG catalog)
    {
        auto information = std::make_shared<Mol::Event::InformationEvent>(code);
        information->AddParameter(catalog, module);
        Mol::DataType::ObjectReference source{sourceID, Dol::DOMAIN_OBJECT_TYPE::MODULE};
        information->SetSource(source);
        return information;
    }

    ERROR_CODE RemoveModuleInfoTest(std::shared_ptr<Mol::Message<Mol::Event::EVENT_CATEGORY>> event, const uint64_t senderID)
    {
        return RemoveModuleInfo(event, senderID);
    }

    ERROR_CODE AddNewModuleInfoTest(std::shared_ptr<Mol::Message<Mol::Event::EVENT_CATEGORY>> event, const uint64_t senderID)
    {
        return AddNewModuleInfo(event, senderID);
    }

    bool TestComponentLifecycle()
    {
        Init();
        Prepare();
        Start();
        Stop();
        Shutdown();
        Uninit();
        return(true);
    }

};
#endif //PLATFORM_TEST_MAIN_CPU_INFO_H
