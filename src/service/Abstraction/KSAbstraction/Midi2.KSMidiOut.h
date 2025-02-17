// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once

class CMidi2KSMidiOut : 
    public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        IMidiOut>
{
public:

    STDMETHOD(Initialize(_In_ LPCWSTR, _In_ DWORD *));
    STDMETHOD(SendMidiMessage(_In_ PVOID, _In_ UINT, _In_ LONGLONG));
    STDMETHOD(Cleanup)();

private:

    std::unique_ptr<CMidi2KSMidi> m_MidiDevice;
};


