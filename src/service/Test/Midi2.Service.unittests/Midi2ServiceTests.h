// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once

#include <WexTestClass.h>

class Midi2ServiceTests
    : public WEX::TestClass<Midi2ServiceTests>,
    public IMidiCallback
{
public:

    BEGIN_TEST_CLASS(Midi2ServiceTests)
        TEST_CLASS_PROPERTY(L"ThreadingModel", L"MTA")
        TEST_CLASS_PROPERTY(L"TestClassification", L"Unit")
        TEST_CLASS_PROPERTY(L"BinaryUnderTest", L"Midi2.KSAbstraction.dll")
        TEST_CLASS_PROPERTY(L"BinaryUnderTest", L"Minmidi.sys")
        TEST_CLASS_PROPERTY(L"BinaryUnderTest", L"MidiSrv.exe")
    END_TEST_CLASS()

    TEST_CLASS_SETUP(ClassSetup);
    TEST_CLASS_CLEANUP(ClassCleanup);

    TEST_METHOD_SETUP(TestSetup);
    TEST_METHOD_CLEANUP(TestCleanup);

    //Generic Tests
    TEST_METHOD(TestMidiServiceRPC);

    TEST_METHOD(TestMidiServiceClientRPC);

    Midi2ServiceTests()
    {}

    STDMETHOD(Callback)(_In_ PVOID Data, _In_ UINT Size, _In_ LONGLONG Position)
    {
        if (m_MidiInCallback)
        {
            m_MidiInCallback(Data, Size, Position);
        }
        return S_OK;
    }

    // The test library is not refcounted, stubbed.
    STDMETHODIMP QueryInterface(REFIID, void**) { return S_OK; }
    STDMETHODIMP_(ULONG) AddRef() { return 1; }
    STDMETHODIMP_(ULONG) Release() { return 1; }

private:
    std::function<void(PVOID, UINT32, LONGLONG)> m_MidiInCallback;
};

