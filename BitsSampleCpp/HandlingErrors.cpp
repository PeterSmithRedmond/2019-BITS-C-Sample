// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

// https://docs.microsoft.com/en-us/windows/desktop/bits/how-to-get-the-last-set-of-http-headers-received-for-each-file-in-a-bits-download-job

// For each file in the job, obtain and display the HTTP header received server.
HRESULT DisplayErrors(IBackgroundCopyJob* job)
{
	wil::com_ptr_nothrow<IBackgroundCopyError> error;
	RETURN_IF_FAILED(job->GetError(&error));

	//Retrieve the HRESULT associated with the error. The context tells you
	//where the error occurred, for example, in the transport, queue manager, the 
	//local file, or the remote file.
	HRESULT hrError = 0;
	BG_ERROR_CONTEXT Context;
	error->GetError(&Context, &hrError);

	//Retrieve a description associated with the HRESULT value.
	wil::unique_cotaskmem_string description;
	RETURN_IF_FAILED(error->GetErrorDescription(LANGIDFROMLCID(GetThreadLocale()), &description));

	if (BG_ERROR_CONTEXT_REMOTE_FILE == Context)
	{
		wil::com_ptr_nothrow<IBackgroundCopyFile> file;
		RETURN_IF_FAILED(error->GetFile(&file));

		wil::unique_cotaskmem_string remoteName;
		RETURN_IF_FAILED(file->GetRemoteName(&remoteName));

		//Do something with the information.
		std::wcout << L"Error with remote file. Error=" << hrError << std::hex << L" description=" << description.get() << L" remote name=" << remoteName.get() << std::endl;
	}
	else
	{
		std::wcout << L"Error in job. Error=" << hrError << std::hex << L" description=" << description.get() << std::endl;
	}
}
