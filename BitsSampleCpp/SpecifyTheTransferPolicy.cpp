// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"

HRESULT BitsSampleMethods::SpecifyTransferPolicy(IBackgroundCopyJob* job)
{
	// The IBackgroundCopyJob5 interface was added in BITS 5 as part of Windows 8
	wil::com_ptr_nothrow<IBackgroundCopyJob5> job5;
	RETURN_IF_FAILED(job->QueryInterface<IBackgroundCopyJob5>(&job5));

	//TODO: pick the most useful set of bits
	BITS_JOB_PROPERTY_VALUE propval;
	propval.Dword = BITS_COST_STATE_USAGE_BASED
		| BITS_COST_STATE_OVERCAP_THROTTLED
		| BITS_COST_STATE_BELOW_CAP
		| BITS_COST_STATE_CAPPED_USAGE_UNKNOWN
		| BITS_COST_STATE_UNRESTRICTED;

	RETURN_IF_FAILED(job5->SetProperty(BITS_JOB_PROPERTY_ID_COST_FLAGS, propval));
	return S_OK;
}