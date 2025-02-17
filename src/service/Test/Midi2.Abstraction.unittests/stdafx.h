// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once

#include <ntstatus.h>
            
#define WIN32_NO_STATUS
#include <windows.h>
#include <winternl.h>
#undef WIN32_NO_STATUS
            
#include <hstring.h>
            
#include <Windows.Devices.Enumeration.h>
#include <assert.h>
#include <devioctl.h>
#include <wrl\implements.h>
#include <wrl\module.h>
#include <wrl\event.h>
#include <ks.h>
#include <ksmedia.h>
#include <avrt.h>
#include <wil\com.h>
#include <wil\resource.h>
#include <wil\result_macros.h>
#include <ppltasks.h>
#include <WexTestClass.h>

#include "MidiTestCommon.h"

