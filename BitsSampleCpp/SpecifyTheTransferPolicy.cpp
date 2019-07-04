// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"


// TODO: put these functions into a simple class.
HRESULT SpecifyTransferPolicy(IBackgroundCopyJob* job)
{
	// The IBackgroundCopyJob5 interface was added in BITS 5 as part of Windows 8
	wil::com_ptr_nothrow<IBackgroundCopyJob5> job5;
	HRESULT hr = job->QueryInterface<IBackgroundCopyJob5>(&job5);
	IFFAILRETURN(hr);

	//TODO: pick the most useful set of bits
	BITS_JOB_PROPERTY_VALUE propval;
	propval.Dword = BITS_COST_STATE_USAGE_BASED
		| BITS_COST_STATE_OVERCAP_THROTTLED
		| BITS_COST_STATE_BELOW_CAP
		| BITS_COST_STATE_CAPPED_USAGE_UNKNOWN
		| BITS_COST_STATE_UNRESTRICTED;

	hr = job5->SetProperty(BITS_JOB_PROPERTY_ID_COST_FLAGS, propval);
	IFFAILRETURN(hr);
	return hr;
}