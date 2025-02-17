// Copyright (c) Microsoft Corporation. All rights reserved.
#include "stdafx.h"

#include "MidiTestCommon.h"
#include "MidiAbstraction.h"
#include "MidiDefs.h"
#include "MidiXProc.h"
#include "Midi2ServiceTests.h"

#include "MidiKsCommon.h"
#include "MidiKsEnum.h"

// Temporary code to locate a bidirectional midi device which will be replaced 
// once we have the MidiDeviceManager functional within MidiSrv.
LPCWSTR GetBiDiMidiDevice(KSMidiDeviceEnum* MidiDeviceEnum)
{
    LPCWSTR midiDevice = nullptr;

    VERIFY_IS_TRUE(MidiDeviceEnum->m_AvailableMidiInPinCount > 0);
    VERIFY_IS_TRUE(MidiDeviceEnum->m_AvailableMidiOutPinCount > 0);

    for (UINT i = 0; i < MidiDeviceEnum->m_AvailableMidiInPinCount && !midiDevice; i++)
    {
        UMP_PINS* testInPin = &(MidiDeviceEnum->m_AvailableMidiInPins[i]);

        for (UINT j = 0; j < MidiDeviceEnum->m_AvailableMidiOutPinCount && !midiDevice; j++)
        {
            UMP_PINS* testOutPin = &(MidiDeviceEnum->m_AvailableMidiOutPins[j]);

            // has to be on the same filter to be a pair, both need to be UMP, and the same
            // buffering mode must be used
            if (0 == _wcsicmp(testInPin->FilterName.get(), testOutPin->FilterName.get()) &&
                testInPin->UMP && testOutPin->UMP && testInPin->Cyclic == testOutPin->Cyclic)
                midiDevice = testInPin->FilterName.get();
        }
    }

    return midiDevice;
}

_Use_decl_annotations_
void * midl_user_allocate(size_t size)
{
    return (void*)new (std::nothrow) BYTE[size];
}

_Use_decl_annotations_
void midl_user_free(void* p)
{
    delete[](BYTE*)p;
}

// using the protocol and endpoint, retrieve the midisrv
// rpc binding handle
HRESULT GetMidiSrvBindingHandle(handle_t* BindingHandle)
{
    RETURN_HR_IF(E_INVALIDARG, nullptr == BindingHandle);
    *BindingHandle = NULL;

    RPC_WSTR stringBinding = nullptr;
    auto cleanupOnExit = wil::scope_exit([&]() {
        if (stringBinding)
        {
            RpcStringFree(&stringBinding);
        }
    });

    RETURN_IF_WIN32_ERROR(RpcStringBindingCompose(
        nullptr,
        reinterpret_cast<RPC_WSTR>(MIDISRV_LRPC_PROTOCOL),
        nullptr,
        reinterpret_cast<RPC_WSTR>(MIDISRV_ENDPOINT),
        nullptr,
        &stringBinding));

    RETURN_IF_WIN32_ERROR(RpcBindingFromStringBinding(
        stringBinding,
        BindingHandle));

    return S_OK;
}

void Midi2ServiceTests::TestMidiServiceRPC()
{
    WEX::TestExecution::SetVerifyOutput verifySettings(WEX::TestExecution::VerifyOutputSettings::LogOnlyFailures);

    static const GUID identifierGuid = {0x4c4c29c3, 0xee6a, 0x4195, {0xb5, 0x56, 0xec, 0xda, 0x8f, 0x5e, 0x87, 0x2b}};
    LPCWSTR endpointIdentifier{L"{4C4C29C3-EE6A-4195-B556-ECDA8F5E872B}"};
    REQUESTED_SETTINGS requestedSettings{0};
    PALLOCATED_SETTINGS allocatedSettings{nullptr};
    wil::unique_rpc_binding bindingHandle;

    requestedSettings.Flag = TRUE;
    requestedSettings.Request = RequestEntry1;
    requestedSettings.Identifier = identifierGuid;

    auto cleanupOnExit = wil::scope_exit([&]() {
        if (allocatedSettings)
        {
            LOG_OUTPUT(L"Freeing allocated memory");
            MIDL_user_free(allocatedSettings);
        }
    });

    LOG_OUTPUT(L"Retrieving binding handle");
    VERIFY_SUCCEEDED(GetMidiSrvBindingHandle(&bindingHandle));

    VERIFY_SUCCEEDED([&]() {
        // RPC calls are placed in a lambda to work around compiler error C2712, limiting use of try/except blocks
        // with structured exception handling.
        RpcTryExcept RETURN_IF_FAILED(MidiSrvTestRpc(bindingHandle.get(), endpointIdentifier, &requestedSettings, &allocatedSettings));
        RpcExcept(I_RpcExceptionFilter(RpcExceptionCode())) RETURN_IF_FAILED(HRESULT_FROM_WIN32(RpcExceptionCode()));
        RpcEndExcept
        return S_OK;
    }());

    LOG_OUTPUT(L"Validating RPC call return values");
    VERIFY_IS_TRUE(allocatedSettings != nullptr);

    if (allocatedSettings != nullptr)
    {
        VERIFY_IS_TRUE(allocatedSettings->Identifier == identifierGuid);
        VERIFY_IS_TRUE(allocatedSettings->Flag == TRUE);
        VERIFY_IS_TRUE(allocatedSettings->Request == RequestEntry1);
    }
}

void Midi2ServiceTests::TestMidiServiceClientRPC()
{
    WEX::TestExecution::SetVerifyOutput verifySettings(WEX::TestExecution::VerifyOutputSettings::LogOnlyFailures);

    MIDISRV_CLIENTCREATION_PARAMS creationParams {0};
    PMIDISRV_CLIENT client {nullptr};
    wil::unique_rpc_binding bindingHandle;
    MidiClientHandle clientHandle{ 0 };
    std::unique_ptr<CMidiXProc> midiPump;
    DWORD MmCssTaskId{ 0 };

    KSMidiDeviceEnum midiDeviceEnum;
    VERIFY_SUCCEEDED(midiDeviceEnum.EnumerateFilters());

    LPCWSTR midiDevice = GetBiDiMidiDevice(&midiDeviceEnum);

    auto cleanupOnExit = wil::scope_exit([&]() {

        if (midiPump)
        {
            midiPump->Cleanup();
        }

        if (client)
        {
            SAFE_CLOSEHANDLE(client->MidiInDataFileMapping);
            SAFE_CLOSEHANDLE(client->MidiInRegisterFileMapping);
            SAFE_CLOSEHANDLE(client->MidiInWriteEvent);
            SAFE_CLOSEHANDLE(client->MidiOutDataFileMapping);
            SAFE_CLOSEHANDLE(client->MidiOutRegisterFileMapping);
            SAFE_CLOSEHANDLE(client->MidiOutWriteEvent);

            MIDL_user_free(client);
            client = nullptr;
        }

        if (0 != clientHandle)
        {
            HRESULT hr = ([&]()
            {
                // RPC calls are placed in a lambda to work around compiler error C2712, limiting use of try/except blocks
                // with structured exception handling.
                RpcTryExcept RETURN_IF_FAILED(MidiSrvDestroyClient(bindingHandle.get(), clientHandle));
                RpcExcept(I_RpcExceptionFilter(RpcExceptionCode())) RETURN_IF_FAILED(HRESULT_FROM_WIN32(RpcExceptionCode()));
                RpcEndExcept
                    return S_OK;
            }());

            if (FAILED(hr))
            {
                LOG_OUTPUT(L"MidiSrvDestroyClient failed 0x%08x", hr);
            }
        }
    });

    creationParams.Protocol = MidiUMP;
    creationParams.Flow = MidiFlowBidirectional;
    creationParams.BufferSize = PAGE_SIZE;

    LOG_OUTPUT(L"Retrieving binding handle");
    VERIFY_SUCCEEDED(GetMidiSrvBindingHandle(&bindingHandle));

    VERIFY_SUCCEEDED([&]() {
        // RPC calls are placed in a lambda to work around compiler error C2712, limiting use of try/except blocks
        // with structured exception handling.
        RpcTryExcept RETURN_IF_FAILED(MidiSrvCreateClient(bindingHandle.get(), midiDevice, &creationParams, &client));
        RpcExcept(I_RpcExceptionFilter(RpcExceptionCode())) RETURN_IF_FAILED(HRESULT_FROM_WIN32(RpcExceptionCode()));
        RpcEndExcept
        return S_OK;
    }());

    clientHandle = client->ClientHandle;

    std::unique_ptr<MEMORY_MAPPED_PIPE> midiInPipe = std::unique_ptr<MEMORY_MAPPED_PIPE>(new (std::nothrow) MEMORY_MAPPED_PIPE);
    VERIFY_IS_TRUE(nullptr != midiInPipe);

    std::unique_ptr <MEMORY_MAPPED_PIPE> midiOutPipe = std::unique_ptr<MEMORY_MAPPED_PIPE>(new (std::nothrow) MEMORY_MAPPED_PIPE);
    VERIFY_IS_TRUE(nullptr != midiOutPipe);

    midiInPipe->DataBuffer.reset(new (std::nothrow) MEMORY_MAPPED_BUFFER);
    VERIFY_IS_TRUE(nullptr != midiInPipe->DataBuffer);

    midiInPipe->RegistersBuffer.reset(new (std::nothrow) MEMORY_MAPPED_BUFFER);
    VERIFY_IS_TRUE(nullptr != midiInPipe->RegistersBuffer);

    midiOutPipe->DataBuffer.reset(new (std::nothrow) MEMORY_MAPPED_BUFFER);
    VERIFY_IS_TRUE(nullptr != midiOutPipe->DataBuffer);

    midiOutPipe->RegistersBuffer.reset(new (std::nothrow) MEMORY_MAPPED_BUFFER);
    VERIFY_IS_TRUE(nullptr != midiOutPipe->RegistersBuffer);

    // Transfer the handles to local storage
    midiInPipe->DataBuffer->FileMapping.reset(client->MidiInDataFileMapping);
    client->MidiInDataFileMapping = NULL;
    midiInPipe->RegistersBuffer->FileMapping.reset(client->MidiInRegisterFileMapping);
    client->MidiInRegisterFileMapping = NULL;
    midiInPipe->WriteEvent.reset(client->MidiInWriteEvent);
    client->MidiInWriteEvent = NULL;
    midiInPipe->Data.BufferSize = client->MidiInBufferSize;
    midiOutPipe->DataBuffer->FileMapping.reset(client->MidiOutDataFileMapping);
    client->MidiOutDataFileMapping = NULL;
    midiOutPipe->RegistersBuffer->FileMapping.reset(client->MidiOutRegisterFileMapping);
    client->MidiOutRegisterFileMapping = NULL;
    midiOutPipe->WriteEvent.reset(client->MidiOutWriteEvent);
    client->MidiOutWriteEvent = NULL;
    midiOutPipe->Data.BufferSize = client->MidiOutBufferSize;

    MIDL_user_free(client);
    client = nullptr;

    // Midi in controls, buffering, and eventing.
    VERIFY_SUCCEEDED(CreateMappedDataBuffer(0, midiInPipe->DataBuffer.get(), &midiInPipe->Data));
    VERIFY_SUCCEEDED(CreateMappedRegisters(midiInPipe->RegistersBuffer.get(), &midiInPipe->Registers));

    // Midi out controls, buffering, and eventing
    VERIFY_SUCCEEDED(CreateMappedDataBuffer(0, midiOutPipe->DataBuffer.get(), &midiOutPipe->Data));
    VERIFY_SUCCEEDED(CreateMappedRegisters(midiOutPipe->RegistersBuffer.get(), &midiOutPipe->Registers));

    UINT midiMessagesReceived = 0;
    UINT messagesExpected{ 0 };
    wil::unique_event_nothrow allMessagesReceived;

    VERIFY_SUCCEEDED(allMessagesReceived.create());

    m_MidiInCallback = [&](PVOID payload, UINT32 payloadSize, LONGLONG payloadPosition)
    {
        PrintMidiMessage(payload, payloadSize, sizeof(UMP32), payloadPosition);

        midiMessagesReceived++;
        if (midiMessagesReceived == messagesExpected)
        {
            allMessagesReceived.SetEvent();
        }
    };

    midiPump.reset(new (std::nothrow) CMidiXProc());
    VERIFY_IS_TRUE(nullptr != midiPump);

    VERIFY_SUCCEEDED(midiPump->Initialize(&MmCssTaskId, midiInPipe, midiOutPipe, this));

    LOG_OUTPUT(L"Writing midi data");
    messagesExpected = 4;
    VERIFY_SUCCEEDED(midiPump->SendMidiMessage((void*)&g_MidiTestData, sizeof(UMP32), 0));
    VERIFY_SUCCEEDED(midiPump->SendMidiMessage((void*)&g_MidiTestData, sizeof(UMP64), 0));
    VERIFY_SUCCEEDED(midiPump->SendMidiMessage((void*)&g_MidiTestData, sizeof(UMP96), 0));
    VERIFY_SUCCEEDED(midiPump->SendMidiMessage((void*)&g_MidiTestData, sizeof(UMP128), 0));

    VERIFY_IS_TRUE(allMessagesReceived.wait(5000));

}

bool Midi2ServiceTests::TestSetup()
{
    m_MidiInCallback = nullptr;
    return true;
}

bool Midi2ServiceTests::TestCleanup()
{
    return true;
}

bool Midi2ServiceTests::ClassSetup()
{
    WEX::TestExecution::SetVerifyOutput verifySettings(WEX::TestExecution::VerifyOutputSettings::LogOnlyFailures);
    VERIFY_SUCCEEDED(Windows::Foundation::Initialize(RO_INIT_MULTITHREADED));

    return true;
}

bool Midi2ServiceTests::ClassCleanup()
{
    Windows::Foundation::Uninitialize();
    return true;
}

