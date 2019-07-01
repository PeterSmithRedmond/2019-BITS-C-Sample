#include "pch.h"


// TODO: put these functions into a simple class.
HRESULT SpecifyTransferPolicy(_com_ptr_t<_com_IIID<IBackgroundCopyJob, &__uuidof(IBackgroundCopyJob)>> job)
{
	// The IBackgroundCopyJob5 interface was added in BITS 5 as part of Windows 8
	_com_ptr_t<_com_IIID<IBackgroundCopyJob5, &__uuidof(IBackgroundCopyJob5)>> job5;
	HRESULT hr = job->QueryInterface<IBackgroundCopyJob5>(&job5);
	if (FAILED(hr)) goto cleanup;

	//TODO: pick the most useful set of bits
	BITS_JOB_PROPERTY_VALUE propval;
	propval.Dword = BITS_COST_STATE_USAGE_BASED
		| BITS_COST_STATE_OVERCAP_THROTTLED
		| BITS_COST_STATE_BELOW_CAP
		| BITS_COST_STATE_CAPPED_USAGE_UNKNOWN
		| BITS_COST_STATE_UNRESTRICTED;

	hr = job5->SetProperty(BITS_JOB_PROPERTY_ID_COST_FLAGS, propval);
	if (FAILED(hr)) goto cleanup;
	return hr;
cleanup:
	std::wcout << L"ERROR: unable to set job Cost property" << std::hex << std::endl;
	return hr;
}