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

	BG_FILE_INFO* paFiles = NULL;
	paFiles = (BG_FILE_INFO*)malloc(sizeof(BG_FILE_INFO) * NFilesInSet);
	// // // TODO: use this other way. and make the strings work. BG_FILE_INFO fileset[1] = { { L"", L"" }  }; //.LocalName = ""; //TODO: make this 
	if (paFiles == NULL)
	{
		//TODO: pick the right error 
		return 1; //TODO: definitely the wrong error where.
	}
	//Doc change: I'm using wcsdup here. I'm also using standard array indexing instead of pointer arithmetic.
	// TODO: maybe cast as a const? Can't keep as wcsdup because that's weird and doesn't get freed automatically.
	paFiles[0].RemoteName = _wcsdup(L"http://www.msftconnecttest.com/");
	paFiles[0].LocalName = _wcsdup(L"c:\\TEMP\\bitssample-page.txt");
	RETURN_IF_FAILED(job->AddFileSet(NFilesInSet, paFiles));
	return S_OK;
}