#include "pch.h"

// https://docs.microsoft.com/en-us/windows/desktop/bits/how-to-get-the-last-set-of-http-headers-received-for-each-file-in-a-bits-download-job

// TODO: yeah, no, not a good practice. make one and use the one. Don't redefine.
struct CoTaskMemDeleter {
	void operator()(void *p) { ::CoTaskMemFree(p); }
};

// For each file in the job, obtain and display the HTTP header received server.
HRESULT DisplayErrors(_com_ptr_t<_com_IIID<IBackgroundCopyJob, &__uuidof(IBackgroundCopyJob)>> job)
{
	HRESULT hr = 0;
	HRESULT hrError = 0;
	WCHAR* remoteName = NULL;
	WCHAR* description = NULL;
	BG_ERROR_CONTEXT Context;
	std::unique_ptr<WCHAR, CoTaskMemDeleter> descriptionGuard;

	_com_ptr_t<_com_IIID<IBackgroundCopyError, &__uuidof(IBackgroundCopyError)>> error;
	hr = job->GetError(&error);
	if (FAILED(hr)) goto cleanup;

	//Retrieve the HRESULT associated with the error. The context tells you
	//where the error occurred, for example, in the transport, queue manager, the 
	//local file, or the remote file.
	error->GetError(&Context, &hrError);

	//Retrieve a description associated with the HRESULT value.
	hr = error->GetErrorDescription(LANGIDFROMLCID(GetThreadLocale()), &description);
	if (FAILED(hr)) goto cleanup;
	descriptionGuard.reset(description);

	if (BG_ERROR_CONTEXT_REMOTE_FILE == Context)
	{
		_com_ptr_t<_com_IIID<IBackgroundCopyFile, &__uuidof(IBackgroundCopyFile)>> file;
		hr = error->GetFile(&file);
		if (FAILED(hr)) goto cleanup;
		hr = file->GetRemoteName(&remoteName);
		if (FAILED(hr)) goto cleanup;
		std::unique_ptr<WCHAR, CoTaskMemDeleter> remoteNameGuard(remoteName);

		//Do something with the information.
		std::wcout << L"Error with remote file. Error=" << hrError << std::hex << L" description=" << description << std::endl;
	}
	else
	{
		std::wcout << L"Error in job. Error=" << hrError << std::hex << L" description=" << description << std::endl;
	}
	return S_OK;

cleanup:
	return hr;
}
