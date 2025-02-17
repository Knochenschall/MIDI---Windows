// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once

class CMidi2MidiSrvBiDi : 
    public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        IMidiBiDi>
{
public:
    STDMETHOD(Initialize(_In_ LPCWSTR, _In_ DWORD *, _In_opt_ IMidiCallback *));
    STDMETHOD(SendMidiMessage(_In_ PVOID message, _In_ UINT size, _In_ LONGLONG));
    STDMETHOD(Cleanup)();

private:
    std::unique_ptr<CMidi2MidiSrv> m_MidiSrv;
};


