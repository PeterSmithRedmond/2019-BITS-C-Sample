// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"


// doc code:
// Sample for https://docs.microsoft.com/en-us/windows/win32/bits/adding-files-to-a-job
// These two methods are the two snippets in the docs.


HRESULT BitsSampleMethods::AddFileToJob(IBackgroundCopyJob* job)
{
	// https://docs.microsoft.com/en-us/windows/desktop/bits/adding-files-to-a-job

	std::wcout << L"Add a file" << std::endl;
	// The c:\temp directory must exist
	RETURN_IF_FAILED(job->AddFile(L"http://www.msftconnecttest.com/ncsi.txt", L"c:\\TEMP\\bitssample-nsci.txt"));

	return S_OK;
}


HRESULT BitsSampleMethods::AddFilesToJob(IBackgroundCopyJob* job)
{
	// The following example shows how to add multiple files to the job

	// The BG_FILE_INFO definition is that it takes in a WSTR; string literals
	// are const pointers; they must be cast into the correct type.
	BG_FILE_INFO fileArray[1] = {
		{(LPWSTR)L"http://www.msftconnecttest.com/", (LPWSTR)L"c:\\TEMP\\bitssample-page.txt" }
	};
	ULONG nFilesInSet = sizeof(fileArray) / sizeof(fileArray[0]);
	RETURN_IF_FAILED(job->AddFileSet(nFilesInSet, fileArray));

	return S_OK;
}