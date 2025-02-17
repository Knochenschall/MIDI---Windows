// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App SDK and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================


#pragma once
#include "MidiSessionSettings.g.h"

namespace winrt::Microsoft::Devices::Midi2::implementation
{
    struct MidiSessionSettings : MidiSessionSettingsT<MidiSessionSettings>
    {
        MidiSessionSettings() = default;

        static winrt::Microsoft::Devices::Midi2::MidiSessionSettings Default();

    };
}
namespace winrt::Microsoft::Devices::Midi2::factory_implementation
{
    struct MidiSessionSettings : MidiSessionSettingsT<MidiSessionSettings, implementation::MidiSessionSettings>
    {
    };
}