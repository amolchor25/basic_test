#include "gmock/gmock.h"
#include "PanelModuleInfoFileProviderTest.h"


using Type = Platform::PanelModuleInfoFileProvider;
TEST(PanelModuleInfoFileProviderUT, CreateResponseFileTestEmptyFileName)
{
    PanelModuleInfoFileProviderTest testobject{};
    const std::string& fileName = "";
    Type::FILE_RESPONSE_TYPE responseType = Type::FILE_RESPONSE_TYPE::XML;
    EXPECT_EQ(PanelModuleInfoFileProviderTest::ERROR_CODE::INVALID_FILE_NAME, testobject.CreateResponseFileTest(fileName, responseType));
}


TEST(PanelModuleInfoFileProviderUT, CreateResponseFileTestTypeNotSuppotrted)
{
    PanelModuleInfoFileProviderTest testobject{};
    const std::string& fileName = "test.xml";
    Type::FILE_RESPONSE_TYPE responseType = Type::FILE_RESPONSE_TYPE::END_OF_LIST;
    EXPECT_EQ(PanelModuleInfoFileProviderTest::ERROR_CODE::TYPE_NOT_SUPPORTED, testobject.CreateResponseFileTest(fileName, responseType));
}

TEST(PanelModuleInfoFileProviderUT, CreateResponseFileTestValidParam)
{
    PanelModuleInfoFileProviderTest testobject{};
    const std::string& fileName = "test.xml";
    Type::FILE_RESPONSE_TYPE responseType = Type::FILE_RESPONSE_TYPE::XML;
    testobject.CreateResponseFileTest(fileName, responseType);
    EXPECT_EQ(testobject.GetFileName(), fileName);
    EXPECT_EQ(testobject.GetResponseType(), responseType);
}

TEST(PanelModuleInfoFileProviderUT, CreateResponseFileTestNoElementsInXmlFile)
{
    PanelModuleInfoFileProviderTest testobject{};
    const std::string& fileName = "test.xml";
    Type::FILE_RESPONSE_TYPE responseType = Type::FILE_RESPONSE_TYPE::XML;
    auto result = testobject.CreateResponseFileTest(fileName, responseType);
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::XML_FILE_CREATED_NO_ELEMENTS);
}

TEST(PanelModuleInfoFileProviderUT, CreateResponseFileTestNoElementsInJsonFile)
{
    PanelModuleInfoFileProviderTest testobject{};
    const std::string& fileName = "test.json";
    Type::FILE_RESPONSE_TYPE responseType = Type::FILE_RESPONSE_TYPE::JSON;
    auto result = testobject.CreateResponseFileTest(fileName, responseType);
    //rewrite this UT when component support json format
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::JSON_TYPE_NOT_SUPPORTED);
}

TEST(PanelModuleInfoFileProviderUT, ModuleDolToXmlElementValidateResult)
{
    const char* expectedXMLContent= "<MODULE>\n    "
                                    "<MODULE_TYPE>32</MODULE_TYPE>\n    "
                                    "<MODULE_ID>1152939101357342720</MODULE_ID>\n   "
                                    " <LABEL>MAINCPU</LABEL>\n    "
                                    "<SERIAL_NUMBER>123456789</SERIAL_NUMBER>\n   "
                                    " <IPV6_ADDRESS>ipv6_address</IPV6_ADDRESS>\n    "
                                    "<SOFTWARE_VERSION>4.5.6-V.789</SOFTWARE_VERSION>\n    "
                                    "<HARDWARE_VERSION>987654321</HARDWARE_VERSION>\n    "
                                    "<CONFIG_VERSION>1.2.3-D.123</CONFIG_VERSION>\n    "
                                    "<MODULE_STATUS>0</MODULE_STATUS>"
                                    "\n</MODULE>\n";
    tinyxml2::XMLDocument m_doc;
    tinyxml2::XMLPrinter printer;
    PanelModuleInfoFileProviderTest testobject{};
    auto module = testobject.CrateModuleObjectForTest("ipv6_address",
            1152939101357342720,
            Dol::Entities::Module::MODULE_TYPE::MAINCPU,
            "MAINCPU",
            "123456789",
            "987654321",
            Dol::Entities::Module::Status::NORMAL_OPERATION,
            "1.2.3-D.123",
            "4.5.6-V.789");

    auto element = testobject.ModuleDolToXmlElementTest(module, m_doc);
    EXPECT_TRUE(nullptr != element);
    if(nullptr != element)
    {
        m_doc.LinkEndChild(element);
        m_doc.Print( &printer );
        EXPECT_EQ(0 ,std::strcmp(printer.CStr() ,expectedXMLContent));
    }
}

TEST(PanelModuleInfoFileProviderUT, CreateXmlResponseTestEmptyFileName)
{
    PanelModuleInfoFileProviderTest testobject{};
    testobject.SetFileNameAndFileFormat("", Type::FILE_RESPONSE_TYPE::XML);
    auto result = testobject.CreateXmlResponseTest();
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::INVALID_FILE_NAME);
}

TEST(PanelModuleInfoFileProviderUT, CreateXmlResponseTestNoModuleElementsInXmlFile)
{
    const char* expectedXMLContent=
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "\n<MODULES>"
            "\n    <!-- modules and main cpu info -->"
            "\n</MODULES>\n";
    const std::string fileName = "test.xml";
    Utility::RemoveFile(fileName);
    PanelModuleInfoFileProviderTest testobject{};
    testobject.SetFileNameAndFileFormat(fileName, Type::FILE_RESPONSE_TYPE::XML);
    auto result = testobject.CreateXmlResponseTest();
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::XML_FILE_CREATED_NO_ELEMENTS);
    auto content = Utility::ReadFile(fileName);
    EXPECT_EQ(0 ,std::strcmp(content.c_str() ,expectedXMLContent));
}

TEST(PanelModuleInfoFileProviderUT, CreateXmlResponseTestValidateFileContent)
{
    const char* expectedXMLContent=
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "\n<MODULES>"
            "\n    <!-- modules and main cpu info -->"
            "\n    <MODULE>"
            "\n        <MODULE_TYPE>32</MODULE_TYPE>"
            "\n        <MODULE_ID>1152939101357342720</MODULE_ID>"
            "\n        <LABEL>MAINCPU</LABEL>"
            "\n        <SERIAL_NUMBER>123456789</SERIAL_NUMBER>"
            "\n        <IPV6_ADDRESS>ipv6_address</IPV6_ADDRESS>"
            "\n        <SOFTWARE_VERSION>4.5.6-V.789</SOFTWARE_VERSION>"
            "\n        <HARDWARE_VERSION>987654321</HARDWARE_VERSION>"
            "\n        <CONFIG_VERSION>1.2.3-D.123</CONFIG_VERSION>"
            "\n        <MODULE_STATUS>0</MODULE_STATUS>"
            "\n    </MODULE>"
            "\n</MODULES>\n";
    const std::string fileName = "test.xml";
    PanelModuleInfoFileProviderTest testobject{};
    auto module = testobject.CrateModuleObjectForTest("ipv6_address",
            1152939101357342720,
            Dol::Entities::Module::MODULE_TYPE::MAINCPU,
            "MAINCPU",
            "123456789",
            "987654321",
            Dol::Entities::Module::Status::NORMAL_OPERATION,
            "1.2.3-D.123",
            "4.5.6-V.789");
    testobject.SetFileNameAndFileFormat(fileName, Type::FILE_RESPONSE_TYPE::XML);
    testobject.AddModuleElement(module);
    Utility::RemoveFile(fileName);
    auto result = testobject.CreateXmlResponseTest();
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::NO_ERROR);
    auto content = Utility::ReadFile(fileName);
    EXPECT_EQ(0 ,std::strcmp(content.c_str() ,expectedXMLContent));
}

TEST(PanelModuleInfoFileProviderUT, RemoveModuleInfoTestInvalideInfoEvent)
{
    PanelModuleInfoFileProviderTest testobject{};
    auto module = testobject.CrateModuleObjectForTest("ipv6_address",
                1152939101357342720,
                Dol::Entities::Module::MODULE_TYPE::MAINCPU,
                "MAINCPU",
                "123456789",
                "987654321",
                Dol::Entities::Module::Status::NORMAL_OPERATION,
                "1.2.3-D.123",
                "4.5.6-V.789");
    auto event = testobject.CrateInformationEvent(Mol::Event::INFORMATION_EVENT_CODE::CONFIG_UPDATE_STATUS,
                                                  module,
                                                  1152939101357342720,
                                                  Mol::DataType::Parameter::CATALOG::DOMAIN_OBJECT);
    auto result = testobject.RemoveModuleInfoTest(event, 0);
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::IGNORE_INFORMATION_EVENT);
}


TEST(PanelModuleInfoFileProviderUT, RemoveModuleInfoTestInvalideCatalogEvent)
{
    PanelModuleInfoFileProviderTest testobject{};
    auto module = testobject.CrateModuleObjectForTest("ipv6_address",
                1152939101357342720,
                Dol::Entities::Module::MODULE_TYPE::MAINCPU,
                "MAINCPU",
                "123456789",
                "987654321",
                Dol::Entities::Module::Status::NORMAL_OPERATION,
                "1.2.3-D.123",
                "4.5.6-V.789");
    auto event = testobject.CrateInformationEvent(Mol::Event::INFORMATION_EVENT_CODE::MODULE_DISCONNECTED,
                                                  module,
                                                  1152939101357342720,
                                                  Mol::DataType::Parameter::CATALOG::ANALOG_VALUE);
    auto result = testobject.RemoveModuleInfoTest(event, 0);
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::NO_MODULE_OBJECT);
}


TEST(PanelModuleInfoFileProviderUT, RemoveModuleInfoTestNullModuleObject)
{
    PanelModuleInfoFileProviderTest testobject{};
    auto event = testobject.CrateInformationEvent(Mol::Event::INFORMATION_EVENT_CODE::MODULE_DISCONNECTED,
                                                  nullptr,
                                                  1152939101357342720,
                                                  Mol::DataType::Parameter::CATALOG::DOMAIN_OBJECT);
    auto result = testobject.RemoveModuleInfoTest(event, 0);
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::NULL_MODULE_OBJECT);
}


TEST(PanelModuleInfoFileProviderUT, RemoveModuleInfoTestNormalOperation)
{
    PanelModuleInfoFileProviderTest testobject{};
    auto module = testobject.CrateModuleObjectForTest("ipv6_address",
                1152939101357342720,
                Dol::Entities::Module::MODULE_TYPE::MAINCPU,
                "MAINCPU",
                "123456789",
                "987654321",
                Dol::Entities::Module::Status::NORMAL_OPERATION,
                "1.2.3-D.123",
                "4.5.6-V.789");
    testobject.AddModuleElement(module);
    EXPECT_EQ(static_cast<std::size_t>(1) ,testobject.GetNumberOfModules());
    auto event = testobject.CrateInformationEvent(Mol::Event::INFORMATION_EVENT_CODE::MODULE_DISCONNECTED,
                                                  module,
                                                  1152939101357342720,
                                                  Mol::DataType::Parameter::CATALOG::DOMAIN_OBJECT);
    auto result = testobject.RemoveModuleInfoTest(event, 0);

    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::NO_ERROR);
    EXPECT_EQ(static_cast<std::size_t>(0) ,testobject.GetNumberOfModules());
}


TEST(PanelModuleInfoFileProviderUT, AddNewModuleInfoTestInvalideInfoEvent)
{
    PanelModuleInfoFileProviderTest testobject{};
    auto module = testobject.CrateModuleObjectForTest("ipv6_address",
                1152939101357342720,
                Dol::Entities::Module::MODULE_TYPE::MAINCPU,
                "MAINCPU",
                "123456789",
                "987654321",
                Dol::Entities::Module::Status::NORMAL_OPERATION,
                "1.2.3-D.123",
                "4.5.6-V.789");
    auto event = testobject.CrateInformationEvent(Mol::Event::INFORMATION_EVENT_CODE::CONFIG_UPDATE_STATUS,
                                                  module,
                                                  1152939101357342720,
                                                  Mol::DataType::Parameter::CATALOG::DOMAIN_OBJECT);
    auto result = testobject.AddNewModuleInfoTest(event, 0);
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::IGNORE_INFORMATION_EVENT);
}


TEST(PanelModuleInfoFileProviderUT, AddNewModuleInfoTestInvalideCatalogEvent)
{
    PanelModuleInfoFileProviderTest testobject{};
    auto module = testobject.CrateModuleObjectForTest("ipv6_address",
                1152939101357342720,
                Dol::Entities::Module::MODULE_TYPE::MAINCPU,
                "MAINCPU",
                "123456789",
                "987654321",
                Dol::Entities::Module::Status::NORMAL_OPERATION,
                "1.2.3-D.123",
                "4.5.6-V.789");
    auto event = testobject.CrateInformationEvent(Mol::Event::INFORMATION_EVENT_CODE::NEW_MODULE_CONNECTED,
                                                  module,
                                                  1152939101357342720,
                                                  Mol::DataType::Parameter::CATALOG::ANALOG_VALUE);
    auto result = testobject.AddNewModuleInfoTest(event, 0);
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::NO_MODULE_OBJECT);
}


TEST(PanelModuleInfoFileProviderUT, AddNewModuleInfoTestNullModuleObject)
{
    PanelModuleInfoFileProviderTest testobject{};
    auto event = testobject.CrateInformationEvent(Mol::Event::INFORMATION_EVENT_CODE::NEW_MODULE_CONNECTED,
                                                  nullptr,
                                                  1152939101357342720,
                                                  Mol::DataType::Parameter::CATALOG::DOMAIN_OBJECT);
    auto result = testobject.AddNewModuleInfoTest(event, 0);
    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::NULL_MODULE_OBJECT);
}

TEST(PanelModuleInfoFileProviderUT, AddNewModuleInfoTestNormalOperation)
{
    PanelModuleInfoFileProviderTest testobject{};
    auto module = testobject.CrateModuleObjectForTest("ipv6_address",
                1152939101357342720,
                Dol::Entities::Module::MODULE_TYPE::MAINCPU,
                "MAINCPU",
                "123456789",
                "987654321",
                Dol::Entities::Module::Status::NORMAL_OPERATION,
                "1.2.3-D.123",
                "4.5.6-V.789");
    EXPECT_EQ(static_cast<std::size_t>(0) ,testobject.GetNumberOfModules());
    auto event = testobject.CrateInformationEvent(Mol::Event::INFORMATION_EVENT_CODE::NEW_MODULE_CONNECTED,
                                                  module,
                                                  1152939101357342720,
                                                  Mol::DataType::Parameter::CATALOG::DOMAIN_OBJECT);
    auto result = testobject.AddNewModuleInfoTest(event, 0);

    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::NO_ERROR);
    EXPECT_EQ(static_cast<std::size_t>(1) ,testobject.GetNumberOfModules());
}

TEST(PanelModuleInfoFileProviderUT, AddNewModuleInfoTestNoDuplicationOfSameModuleID)
{
    PanelModuleInfoFileProviderTest testobject{};
    auto module = testobject.CrateModuleObjectForTest("ipv6_address",
                1152939101357342720,
                Dol::Entities::Module::MODULE_TYPE::MAINCPU,
                "MAINCPU",
                "123456789",
                "987654321",
                Dol::Entities::Module::Status::NORMAL_OPERATION,
                "1.2.3-D.123",
                "4.5.6-V.789");
    EXPECT_EQ(static_cast<std::size_t>(0) ,testobject.GetNumberOfModules());
    auto event = testobject.CrateInformationEvent(Mol::Event::INFORMATION_EVENT_CODE::NEW_MODULE_CONNECTED,
                                                  module,
                                                  1152939101357342720,
                                                  Mol::DataType::Parameter::CATALOG::DOMAIN_OBJECT);
    auto result = testobject.AddNewModuleInfoTest(event, 0);

    EXPECT_EQ(result ,PanelModuleInfoFileProviderTest::ERROR_CODE::NO_ERROR);
    EXPECT_EQ(static_cast<std::size_t>(1) ,testobject.GetNumberOfModules());
    result = testobject.AddNewModuleInfoTest(event, 0);
    EXPECT_EQ(static_cast<std::size_t>(1) ,testobject.GetNumberOfModules());
}

TEST(PanelModuleInfoFileProviderUT, Test)
{
    PanelModuleInfoFileProviderTest testobject{};

    EXPECT_TRUE(testobject.TestComponentLifecycle());
}
