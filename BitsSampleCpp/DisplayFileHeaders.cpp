#include "pch.h"

// https://docs.microsoft.com/en-us/windows/desktop/bits/how-to-get-the-last-set-of-http-headers-received-for-each-file-in-a-bits-download-job


struct CoTaskMemDeleter {
	void operator()(void *p) { ::CoTaskMemFree(p); }
};

// TODO: team concensus is to not have any GOTO CLEANUP at all anywhere; instead just return the hr.

// For each file in the job, obtain and display the HTTP header received server.
//TODO: the _com_ptr_t is totally uneeded here and should be just IBackgroundCopyJob*
HRESULT DisplayFileHeaders(_com_ptr_t<_com_IIID<IBackgroundCopyJob, &__uuidof(IBackgroundCopyJob)>> job)
{
	HRESULT hr;
	IEnumBackgroundCopyFiles *fileEnumerator;
	_com_ptr_t<_com_IIID<IBackgroundCopyFile5, &__uuidof(IBackgroundCopyFile5)>> file5;

	hr = job->EnumFiles(&fileEnumerator);
	if (FAILED(hr)) goto cleanup;

	ULONG count;
	hr = fileEnumerator->GetCount(&count);
	if (FAILED(hr)) goto cleanup;

	for (ULONG i = 0; i < count; ++i)
	{
		// doc change: switched to smart pointers
		// doc change: rename from TempFile to plain file
		_com_ptr_t<_com_IIID<IBackgroundCopyFile, &__uuidof(IBackgroundCopyFile)>> file;

		// doc question: should I try to get more files at once?
		hr = fileEnumerator->Next(1, &file, NULL);
		if (FAILED(hr)) goto cleanup;

		// The IBackgroundCopyJob5 interface was added in BITS 5 as part of Windows 8
		//Doc change: switched to smart pointers
		hr = file->QueryInterface(__uuidof(IBackgroundCopyFile5), (void **)&file5);
		if (FAILED(hr)) goto cleanup;

		LPWSTR remoteFileName;
		hr = file5->GetRemoteName(&remoteFileName);
		if (FAILED(hr)) goto cleanup;
		std::unique_ptr<WCHAR, CoTaskMemDeleter> remoteNameGuard(remoteFileName);
		std::wcout << L"File URL: " << remoteFileName << std::endl; // doc change: use std::wcout

		BITS_FILE_PROPERTY_VALUE value;
		hr = file5->GetProperty(BITS_FILE_PROPERTY_ID_HTTP_RESPONSE_HEADERS, &value);
		if (FAILED(hr)) goto cleanup;
		if (value.String)
		{
			std::unique_ptr<WCHAR, CoTaskMemDeleter> valueStringGuard(value.String);
			std::wcout << L"Headers: " << value.String << std::endl; //doc change: use correct name and switch to wcout
		}
	}

	return S_OK;
cleanup:
	return hr;
}