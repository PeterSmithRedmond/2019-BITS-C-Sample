// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"


// doc code:
// Sample for https://docs.microsoft.com/en-us/windows/win32/bits/registering-a-com-callback// https://docs.microsoft.com/en-us/windows/desktop/api/bits/nn-bits-ibackgroundcopycallback
// Also needed is the NotifyInterface.h and NotifyInterface.cpp files
// The CLSID based snippet in the docmentation will be unchanged.

HRESULT BitsSampleMethods::SetNotifyInterface(IBackgroundCopyJob* job)
{
	// Part 5b: set up a notifications for job changes
	// https://docs.microsoft.com/en-us/windows/desktop/api/Bits/nf-bits-ibackgroundcopyjob-setnotifyinterface

	wil::com_ptr_nothrow<CNotifyInterface> notify(new CNotifyInterface());
	RETURN_IF_FAILED(job->SetNotifyInterface(notify.get()));
	RETURN_IF_FAILED(job->SetNotifyFlags(BG_NOTIFY_JOB_TRANSFERRED | BG_NOTIFY_JOB_ERROR | BG_NOTIFY_JOB_MODIFICATION));

	return S_OK;
}
