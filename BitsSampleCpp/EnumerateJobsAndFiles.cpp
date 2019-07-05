// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"
// https://docs.microsoft.com/en-us/windows/desktop/bits/enumerating-jobs-in-the-transfer-queue



HRESULT BitsSampleMethods::EnumerateJobsAndFiles(IBackgroundCopyManager* mgr)
{
	wil::com_ptr_nothrow<IEnumBackgroundCopyJobs> jobs;
	wil::com_ptr_t<IBackgroundCopyJob> job;

	ULONG jobCount = 0;

	RETURN_IF_FAILED(mgr->EnumJobs(0, &jobs)); // 0 means enumerate just for this user

	jobs->GetCount(&jobCount);
	std::wcout << L"Enumerate: job count=" << jobCount << std::endl;

	for (ULONG jobIndex = 0; jobIndex < jobCount; jobIndex++)
	{
		RETURN_IF_FAILED(jobs->Next(1, &job, NULL));

		wil::unique_cotaskmem_string jobName;
		RETURN_IF_FAILED(job->GetDisplayName(&jobName));

		std::wcout << L"ENUMERATE: Job " << jobIndex << " name=" << jobName.get() << std::endl;
		BitsSampleMethods::EnumerateFiles(job.get());
	}

	return S_OK;
}

HRESULT BitsSampleMethods::EnumerateFiles(IBackgroundCopyJob* job)
{
	// doc: Enumerating files in a job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/enumerating-files-in-a-job

	wil::com_ptr_nothrow<IEnumBackgroundCopyFiles> files;
	RETURN_IF_FAILED(job->EnumFiles(&files));

	ULONG fileCount = 0;
	RETURN_IF_FAILED(files->GetCount(&fileCount));

	for (ULONG fileIndex = 0; fileIndex < fileCount; fileIndex++)
	{
		wil::com_ptr_nothrow<IBackgroundCopyFile> file;
		RETURN_IF_FAILED(files->Next(1, &file, NULL));

		wil::unique_cotaskmem_string remoteName;
		RETURN_IF_FAILED(file->GetRemoteName(&remoteName));

		wil::unique_cotaskmem_string localName;
		RETURN_IF_FAILED(file->GetLocalName(&localName));
		std::wcout << L"    ENUMERATE: file " << fileIndex << " remote=" << remoteName.get() << " local=" << localName.get() << std::endl;
	}

	return S_OK;
}