// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"


HRESULT BitsSampleMethods::SetNotifyInterface(IBackgroundCopyJob* job)
{
	// Part 5b: set up a notifications for job changes
	// https://docs.microsoft.com/en-us/windows/desktop/api/Bits/nf-bits-ibackgroundcopyjob-setnotifyinterface

	auto pNotify = new CNotifyInterface(); // TODO: use wil: for this? For part 5B; the new doesn't do an AddRef.
	pNotify->AddRef();
	RETURN_IF_FAILED(job->SetNotifyInterface(pNotify));
	// BITS has taken a reference on the Notify object; remove our reference to it.
	// Remove our reference regardless of whether or not the notify succeeded or not.
	pNotify->Release();
	pNotify = NULL;

	RETURN_IF_FAILED(job->SetNotifyFlags(BG_NOTIFY_JOB_TRANSFERRED | BG_NOTIFY_JOB_ERROR | BG_NOTIFY_JOB_MODIFICATION));

	return S_OK;
}
