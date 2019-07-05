// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "BitsSampleMethods.h"

// https://docs.microsoft.com/en-us/windows/desktop/bits/how-to-get-the-last-set-of-http-headers-received-for-each-file-in-a-bits-download-job


// For each file in the job, obtain and display the HTTP header received server.
HRESULT BitsSampleMethods::DisplayFileHeaders(IBackgroundCopyJob* job)
{
	wil::com_ptr_nothrow<IEnumBackgroundCopyFiles> fileEnumerator;
	RETURN_IF_FAILED(job->EnumFiles(&fileEnumerator));

	ULONG count;
	RETURN_IF_FAILED(fileEnumerator->GetCount(&count));

	for (ULONG i = 0; i < count; ++i)
	{
		wil::com_ptr_nothrow<IBackgroundCopyFile> file;
		RETURN_IF_FAILED(fileEnumerator->Next(1, &file, NULL));

		// The IBackgroundCopyJob5 interface was added in BITS 5 as part of Windows 8
		wil::com_ptr_nothrow<IBackgroundCopyFile5> file5;
		RETURN_IF_FAILED(file.query_to<IBackgroundCopyFile5>(&file5));

		wil::unique_cotaskmem_string remoteFileName;
		RETURN_IF_FAILED(file5->GetRemoteName(&remoteFileName));
		std::wcout << L"File URL: " << remoteFileName.get() << std::endl;

		BITS_FILE_PROPERTY_VALUE value;
		RETURN_IF_FAILED(file5->GetProperty(BITS_FILE_PROPERTY_ID_HTTP_RESPONSE_HEADERS, &value));
		if (value.String)
		{
			wil::unique_cotaskmem_ptr<WCHAR> valueStringGuard(value.String);
			std::wcout << L"Headers: " << value.String << std::endl;
		}
	}

	return S_OK;
}