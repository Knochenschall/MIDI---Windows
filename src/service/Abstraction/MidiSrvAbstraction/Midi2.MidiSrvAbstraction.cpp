// Copyright (c) Microsoft Corporation. All rights reserved.

#include "pch.h"

_Use_decl_annotations_
HRESULT
CMidi2MidiSrvAbstraction::Activate(
    REFIID Iid,
    void **Interface
)
{
    RETURN_HR_IF(E_INVALIDARG, nullptr == Interface);

    if (__uuidof(IMidiIn) == Iid)
    {
        TraceLoggingWrite(
            MidiSrvAbstractionTelemetryProvider::Provider(),
            __FUNCTION__ "- Midi in",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(__FUNCTION__),
            TraceLoggingPointer(this, "this")
            );

        wil::com_ptr_nothrow<IMidiIn> midiIn;
        RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<CMidi2MidiSrvIn>(&midiIn));
        *Interface = midiIn.detach();
    }
    else if (__uuidof(IMidiOut) == Iid)
    {
        TraceLoggingWrite(
            MidiSrvAbstractionTelemetryProvider::Provider(),
            __FUNCTION__ "- Midi Out",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(__FUNCTION__),
            TraceLoggingPointer(this, "this")
            );

        wil::com_ptr_nothrow<IMidiOut> midiOut;
        RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<CMidi2MidiSrvOut>(&midiOut));
        *Interface = midiOut.detach();
    }
    else if (__uuidof(IMidiBiDi) == Iid)
    {
        TraceLoggingWrite(
            MidiSrvAbstractionTelemetryProvider::Provider(),
            __FUNCTION__ "- Midi BiDi",
            TraceLoggingLevel(WINEVENT_LEVEL_INFO),
            TraceLoggingValue(__FUNCTION__),
            TraceLoggingPointer(this, "this")
            );

        wil::com_ptr_nothrow<IMidiBiDi> midiBiDi;
        RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<CMidi2MidiSrvBiDi>(&midiBiDi));
        *Interface = midiBiDi.detach();
    }

    else
    {
        return E_NOINTERFACE;
    }

    return S_OK;
}

_Use_decl_annotations_
void __RPC_FAR* __RPC_USER midl_user_allocate(size_t ByteCount
)
{
    return (void*)new (std::nothrow) BYTE[ByteCount];
}

_Use_decl_annotations_
void __RPC_USER midl_user_free(void __RPC_FAR* Pointer
)
{
    delete[](BYTE*)Pointer;
}

// using the protocol and endpoint, retrieve the midisrv
// rpc binding handle
_Use_decl_annotations_
HRESULT
GetMidiSrvBindingHandle(handle_t* BindingHandle
)
{
    RETURN_HR_IF(E_INVALIDARG, nullptr == BindingHandle);
    *BindingHandle = NULL;

    RPC_WSTR stringBinding = nullptr;
    auto cleanupOnExit = wil::scope_exit([&]()
    {
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
