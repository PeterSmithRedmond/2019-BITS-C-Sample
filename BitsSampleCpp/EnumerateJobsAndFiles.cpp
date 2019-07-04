// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

// https://docs.microsoft.com/en-us/windows/desktop/bits/enumerating-jobs-in-the-transfer-queue
HRESULT EnumerateFiles(IBackgroundCopyJob* job);


HRESULT EnumerateJobsAndFiles(IBackgroundCopyManager* mgr)
{
	wil::com_ptr_nothrow<IEnumBackgroundCopyJobs> jobs;
	wil::com_ptr_t<IBackgroundCopyJob> job;

	ULONG jobCount = 0;

	HRESULT hr = mgr->EnumJobs(0, &jobs); // 0 means enumerate just for this user
	IFFAILRETURN(hr);
	 //TODO: make an enum for BG_JOB_ENUM_CURRENT_USER (file bug)

	jobs->GetCount(&jobCount);
	std::wcout << L"Enumerate: job count=" << jobCount << std::endl;

	for (ULONG jobIndex = 0; jobIndex < jobCount; jobIndex++)
	{
		hr = jobs->Next(1, &job, NULL);
		IFFAILRETURN(hr);

		wil::unique_cotaskmem_string jobName;
		hr = job->GetDisplayName(&jobName);
		IFFAILRETURN(hr);

		std::wcout << L"ENUMERATE: Job " << jobIndex << " name=" << &jobName << std::endl;
		EnumerateFiles(job.get());
	}

	return hr; // S_OK
}


HRESULT EnumerateFiles(IBackgroundCopyJob* job)
{
	// doc: Enumerating files in a job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/enumerating-files-in-a-job

	wil::com_ptr_nothrow<IEnumBackgroundCopyFiles> files;
	HRESULT hr = job->EnumFiles(&files);
	IFFAILRETURN(hr);

	ULONG fileCount = 0;
	hr = files->GetCount(&fileCount);
	IFFAILRETURN(hr);

	for (ULONG fileIndex = 0; fileIndex < fileCount; fileIndex++)
	{
		wil::com_ptr_nothrow<IBackgroundCopyFile> file;
		hr = files->Next(1, &file, NULL);
		IFFAILRETURN(hr);

		wil::unique_cotaskmem_string remoteName;
		hr = file->GetRemoteName(&remoteName); //handle hr.

		wil::unique_cotaskmem_string localName;
		hr = file->GetLocalName(&localName);
		std::wcout << L"    ENUMERATE: file " << fileIndex << " remote=" << &remoteName << " local=" << &localName << std::endl;
	}

	return hr;
}