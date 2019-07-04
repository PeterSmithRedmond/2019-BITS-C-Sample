// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

// https://docs.microsoft.com/en-us/windows/desktop/bits/how-to-get-the-last-set-of-http-headers-received-for-each-file-in-a-bits-download-job

// For each file in the job, obtain and display the HTTP header received server.
HRESULT DisplayErrors(IBackgroundCopyJob* job)
{
	HRESULT hr = 0;
	wil::com_ptr_nothrow<IBackgroundCopyError> error;
	hr = job->GetError(&error);
	IFFAILRETURN(hr);

	//Retrieve the HRESULT associated with the error. The context tells you
	//where the error occurred, for example, in the transport, queue manager, the 
	//local file, or the remote file.
	HRESULT hrError = 0;
	BG_ERROR_CONTEXT Context;
	error->GetError(&Context, &hrError);

	//Retrieve a description associated with the HRESULT value.
	wil::unique_cotaskmem_string description;
	hr = error->GetErrorDescription(LANGIDFROMLCID(GetThreadLocale()), &description);
	IFFAILRETURN(hr);

	if (BG_ERROR_CONTEXT_REMOTE_FILE == Context)
	{
		wil::com_ptr_nothrow<IBackgroundCopyFile> file;
		hr = error->GetFile(&file);
		IFFAILRETURN(hr);

		wil::unique_cotaskmem_string remoteName;
		hr = file->GetRemoteName(&remoteName);
		IFFAILRETURN(hr);

		//Do something with the information.
		std::wcout << L"Error with remote file. Error=" << hrError << std::hex << L" description=" << &description << L" remote name=" << &remoteName << std::endl;
	}
	else
	{
		std::wcout << L"Error in job. Error=" << hrError << std::hex << L" description=" << &description << std::endl;
	}
	return hr;
}
