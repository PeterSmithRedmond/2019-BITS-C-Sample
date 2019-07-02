#include "pch.h"

// https://docs.microsoft.com/en-us/windows/desktop/bits/how-to-get-the-last-set-of-http-headers-received-for-each-file-in-a-bits-download-job


// For each file in the job, obtain and display the HTTP header received server.
HRESULT DisplayFileHeaders(IBackgroundCopyJob* job)
{
	HRESULT hr;
	wil::com_ptr_nothrow<IEnumBackgroundCopyFiles> fileEnumerator;
	wil::com_ptr_nothrow<IBackgroundCopyFile5> file5;

	hr = job->EnumFiles(&fileEnumerator);
	IFFAILRETURN(hr);

	ULONG count;
	hr = fileEnumerator->GetCount(&count);

	for (ULONG i = 0; i < count; ++i)
	{
		wil::com_ptr_nothrow<IBackgroundCopyFile> file;
		hr = fileEnumerator->Next(1, &file, NULL);
		IFFAILRETURN(hr);

		// The IBackgroundCopyJob5 interface was added in BITS 5 as part of Windows 8
		hr = file.query_to<IBackgroundCopyFile5>(&file5);
		IFFAILRETURN(hr);

		WCHAR* remoteFileName;
		hr = file5->GetRemoteName(&remoteFileName);
		IFFAILRETURN(hr);
		wil::unique_cotaskmem_ptr<WCHAR> remoteFileNameGuard(remoteFileName);
		std::wcout << L"File URL: " << remoteFileName << std::endl; // doc change: use std::wcout

		BITS_FILE_PROPERTY_VALUE value;
		hr = file5->GetProperty(BITS_FILE_PROPERTY_ID_HTTP_RESPONSE_HEADERS, &value);
		IFFAILRETURN(hr);
		if (value.String)
		{
			wil::unique_cotaskmem_ptr<WCHAR> valueStringGuard(value.String);
			std::wcout << L"Headers: " << value.String << std::endl; //doc change: use correct name and switch to wcout
		}
	}

	return hr;
}