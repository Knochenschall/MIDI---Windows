// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App SDK and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#include "pch.h"
#include "MidiDeviceInformation.h"
#include "MidiDeviceInformation.g.cpp"

namespace winrt::Microsoft::Devices::Midi2::implementation
{
    winrt::Microsoft::Devices::Midi2::MidiDeviceInformation MidiDeviceInformation::CreateFromId(hstring const& deviceId)
    {
        throw hresult_not_implemented();
    }
    winrt::Microsoft::Devices::Midi2::MidiDeviceInformation MidiDeviceInformation::FromDeviceInformation(winrt::Windows::Devices::Enumeration::DeviceInformation const& deviceInformation)
    {
        throw hresult_not_implemented();
    }
    winrt::Windows::Devices::Enumeration::DeviceWatcher MidiDeviceInformation::CreateWatcher()
    {
        throw hresult_not_implemented();
    }
    winrt::Windows::Devices::Enumeration::DeviceWatcher MidiDeviceInformation::CreateWatcher(winrt::Microsoft::Devices::Midi2::MidiEndpointDataFormat const& midiEndpointDataFormat)
    {
        throw hresult_not_implemented();
    }
    hstring MidiDeviceInformation::Id()
    {
        throw hresult_not_implemented();
    }
    hstring MidiDeviceInformation::Name()
    {
        throw hresult_not_implemented();
    }
    hstring MidiDeviceInformation::Description()
    {
        throw hresult_not_implemented();
    }
    hstring MidiDeviceInformation::HasPersistentUniqueIdentifier()
    {
        throw hresult_not_implemented();
    }
    hstring MidiDeviceInformation::UniqueIdentifier()
    {
        throw hresult_not_implemented();
    }
    winrt::Microsoft::Devices::Midi2::MidiEndpointDataFormat MidiDeviceInformation::EndpointDataFormat()
    {
        throw hresult_not_implemented();
    }
    bool MidiDeviceInformation::HasGroupTerminalBlocks()
    {
        throw hresult_not_implemented();
    }
    winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::Devices::Midi2::MidiGroupTerminalBlock> MidiDeviceInformation::GroupTerminalBlocks()
    {
        throw hresult_not_implemented();
    }
    winrt::Windows::Devices::Enumeration::DeviceThumbnail MidiDeviceInformation::DeviceThumbnail()
    {
        throw hresult_not_implemented();
    }
}
