// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"


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
	const ULONG NFilesInSet = 1;

	// The BG_FILE_INFO definition is that it takes in a WSTR; string literals
	// are const pointers. Just cast from one to the other.
	BG_FILE_INFO fileArray[1] = { 
		{(LPWSTR)L"http://www.msftconnecttest.com/", (LPWSTR)L"c:\\TEMP\\bitssample-page.txt" } 
	};

	RETURN_IF_FAILED(job->AddFileSet(NFilesInSet, fileArray));
	return S_OK;
}