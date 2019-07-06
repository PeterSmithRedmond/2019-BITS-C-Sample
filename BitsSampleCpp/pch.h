// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bits.h>
#include <sddl.h> // needed for ConvertStringSidToSid
#include <iostream>
#include <ostream>

#include <wil\com.h>
#include <wil\Resource.h>
// https://github.com/microsoft/wil/wiki/Error-handling-helpers
#include <wil\Result.h>


#include "NotifyInterface.h"


