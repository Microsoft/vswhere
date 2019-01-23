// <copyright file="XmlFormatterTests.cpp" company="Microsoft Corporation">
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt in the project root for license information.
// </copyright>

#include "stdafx.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(XmlFormatterTests)
{
public:
    TEST_METHOD(Write_Instance)
    {
        CommandArgs args;
        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" },
            { L"Description", L"This description contains \"quotes\"." },
        };

        XmlFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <instanceId>a1b2c3</instanceId>\n"
            L"    <installDate>2017-02-23T01:22:35Z</installDate>\n"
            L"    <installationName>test</installationName>\n"
            L"    <description>This description contains \"quotes\".</description>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    TEST_METHOD(Write_Instances)
    {
        CommandArgs args;
        TestConsole console(args);
        TestInstance instance1 =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };

        TestInstance instance2 =
        {
            { L"instanceId", L"b1c2d3" },
            { L"installationPath", L"C:\\ShouldNotExist" },
            { L"installationVersion", L"1.2.3.4" },
        };

        vector<ISetupInstancePtr> instances =
        {
            &instance1,
            &instance2,
        };

        XmlFormatter sut;
        sut.Write(args, console, instances);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <instanceId>a1b2c3</instanceId>\n"
            L"    <installationName>test</installationName>\n"
            L"  </instance>\n"
            L"  <instance>\n"
            L"    <instanceId>b1c2d3</instanceId>\n"
            L"    <installationPath>C:\\ShouldNotExist</installationPath>\n"
            L"    <installationVersion>1.2.3.4</installationVersion>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    TEST_METHOD(Write_No_Instances)
    {
        CommandArgs args;
        TestConsole console(args);
        vector<ISetupInstancePtr> instances;

        XmlFormatter sut;
        sut.Write(args, console, instances);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_With_Properties)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_With_Properties)
    {
        CommandArgs args;
        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        XmlFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <instanceId>a1b2c3</instanceId>\n"
            L"    <installDate>2017-02-23T01:22:35Z</installDate>\n"
            L"    <installationName>test</installationName>\n"
            L"    <properties>\n"
            L"      <campaignId>abcd1234</campaignId>\n"
            L"      <nickname>test</nickname>\n"
            L"    </properties>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_All_Properties)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_All_Properties)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property properties");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        XmlFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <properties>\n"
            L"      <campaignId>abcd1234</campaignId>\n"
            L"      <nickname>test</nickname>\n"
            L"    </properties>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Specified_Property)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Specified_Property)
    {
        // Verify that "/" works as separator since this fits with XML using XPath syntax.
        CommandArgs args;
        args.Parse(L"vswhere.exe -property properties/nickname");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        XmlFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <properties>\n"
            L"      <nickname>test</nickname>\n"
            L"    </properties>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Specified_Short_Property)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Specified_Short_Property)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property nickname");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        XmlFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <properties>\n"
            L"      <nickname>test</nickname>\n"
            L"    </properties>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Multiple_With_Properties)
        TEST_WORKITEM(104)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Multiple_With_Properties)
    {
        CommandArgs args;
        TestConsole console(args);

        TestInstance instance1 =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
        };
        instance1.AssignAdditionalProperties(TestPropertyStore
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test1") },
        });

        TestInstance instance2 =
        {
            { L"InstanceId", L"b1c2d3" },
            { L"InstallationName", L"test" },
        };
        instance2.AssignAdditionalProperties(TestPropertyStore
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test2") },
        });

        vector<ISetupInstancePtr> instances =
        {
            &instance1,
            &instance2,
        };

        XmlFormatter sut;
        sut.Write(args, console, instances);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <instanceId>a1b2c3</instanceId>\n"
            L"    <installationName>test</installationName>\n"
            L"    <properties>\n"
            L"      <campaignId>abcd1234</campaignId>\n"
            L"      <nickname>test1</nickname>\n"
            L"    </properties>\n"
            L"  </instance>\n"
            L"  <instance>\n"
            L"    <instanceId>b1c2d3</instanceId>\n"
            L"    <installationName>test</installationName>\n"
            L"    <properties>\n"
            L"      <campaignId>abcd1234</campaignId>\n"
            L"      <nickname>test2</nickname>\n"
            L"    </properties>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Missing_Property_Writes_Empty_Object)
        TEST_WORKITEM(108)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Missing_Property_Writes_Empty_Object)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe -property missing");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore catalogInfo =
        {
            { L"productName", make_tuple(VT_BSTR, L"Test") },
            { L"productSemanticVersion", make_tuple(VT_BSTR, L"1.0") },
            { L"productLineVersion", make_tuple(VT_I4, L"2017") },
        };

        instance.AssignCatalogProperties(catalogInfo);

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        XmlFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_All)
        TEST_WORKITEM(109)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_All)
    {
        CommandArgs args;
        args.Parse(L"vswhere.exe");

        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"InstallationName", L"test" },
            { L"InstallDate", L"2017-02-23T01:22:35Z" }
        };

        TestPropertyStore catalogInfo =
        {
            { L"productName", make_tuple(VT_BSTR, L"Test") },
            { L"productSemanticVersion", make_tuple(VT_BSTR, L"1.0") },
            { L"productLineVersion", make_tuple(VT_I4, L"2017") },
        };

        instance.AssignCatalogProperties(catalogInfo);

        TestPropertyStore properties =
        {
            { L"campaignId", make_tuple(VT_BSTR, L"abcd1234") },
            { L"nickname", make_tuple(VT_BSTR, L"test") },
        };

        instance.AssignAdditionalProperties(properties);

        XmlFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <instanceId>a1b2c3</instanceId>\n"
            L"    <installDate>2017-02-23T01:22:35Z</installDate>\n"
            L"    <installationName>test</installationName>\n"
            L"    <isPrerelease>0</isPrerelease>\n"
            L"    <catalog>\n"
            L"      <productLineVersion>2017</productLineVersion>\n"
            L"      <productName>Test</productName>\n"
            L"      <productSemanticVersion>1.0</productSemanticVersion>\n"
            L"    </catalog>\n"
            L"    <properties>\n"
            L"      <campaignId>abcd1234</campaignId>\n"
            L"      <nickname>test</nickname>\n"
            L"    </properties>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_State)
        TEST_WORKITEM(133)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_State)
    {
        CommandArgs args;
        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"State", L"11" },
            { L"IsComplete", L"true"},
            { L"IsLaunchable", L"false" },
        };

        XmlFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <instanceId>a1b2c3</instanceId>\n"
            L"    <state>11</state>\n"
            L"    <isComplete>1</isComplete>\n"
            L"    <isLaunchable>0</isLaunchable>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Complete_State)
        TEST_WORKITEM(133)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Complete_State)
    {
        CommandArgs args;
        TestConsole console(args);
        TestInstance instance =
        {
            { L"InstanceId", L"a1b2c3" },
            { L"State", L"4294967295" },
            { L"IsComplete", L"true"},
            { L"IsLaunchable", L"true" },
        };

        XmlFormatter sut;
        sut.Write(args, console, &instance);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<instances>\n"
            L"  <instance>\n"
            L"    <instanceId>a1b2c3</instanceId>\n"
            L"    <state>4294967295</state>\n"
            L"    <isComplete>1</isComplete>\n"
            L"    <isLaunchable>1</isLaunchable>\n"
            L"  </instance>\n"
            L"</instances>\n";

        Assert::AreEqual(expected, console);
    }

    BEGIN_TEST_METHOD_ATTRIBUTE(Writes_Array)
        TEST_WORKITEM(162)
    END_TEST_METHOD_ATTRIBUTE()
    TEST_METHOD(Writes_Array)
    {
        CommandArgs args;
        TestConsole console(args);
        vector<wstring> values =
        {
            L"a",
            L"b",
            L"c",
        };

        XmlFormatter sut;
        sut.Write(console, L"values", L"value", values);

        auto expected =
            L"<?xml version=\"1.0\"?>\n"
            L"<values>\n"
            L"  <value>a</value>\n"
            L"  <value>b</value>\n"
            L"  <value>c</value>\n"
            L"</values>\n";

        Assert::AreEqual(expected, console);
    }
};
