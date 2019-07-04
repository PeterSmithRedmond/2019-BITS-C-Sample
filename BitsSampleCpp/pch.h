// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bits.h>
#include <iostream>
#include <ostream>

#include "NotifyInterface.h"
#include <wil\com.h>
#include <wil\Resource.h>
#define IFFAILRETURN(hr) if (FAILED (hr)) return hr;

// Functions that are uses here and there.
HRESULT EnumerateJobsAndFiles(IBackgroundCopyManager* mgr);
HRESULT SpecifyTransferPolicy(IBackgroundCopyJob* job);
HRESULT DisplayFileHeaders(IBackgroundCopyJob* job);
HRESULT DisplayErrors(IBackgroundCopyJob* job);

