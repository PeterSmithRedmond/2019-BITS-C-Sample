// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BitsSampleMethods.h"
// https://docs.microsoft.com/en-us/windows/desktop/bits/enumerating-jobs-in-the-transfer-queue


// doc code:
// Sample for https://docs.microsoft.com/en-us/windows/win32/bits/enumerating-jobs-in-the-transfer-queue// These entirely replaces the code snippet.

HRESULT BitsSampleMethods::EnumerateJobsAndFiles(IBackgroundCopyManager* mgr)
{
	wil::com_ptr_nothrow<IEnumBackgroundCopyJobs> jobs;
	wil::com_ptr_t<IBackgroundCopyJob> job;

	ULONG jobCount = 0;

	RETURN_IF_FAILED(mgr->EnumJobs(0, &jobs)); // 0 means enumerate just for this user

	jobs->GetCount(&jobCount);
	std::wcout << L"ENUMERATE: job count=" << jobCount << std::endl;

	for (ULONG jobIndex = 0; jobIndex < jobCount; jobIndex++)
	{
		RETURN_IF_FAILED(jobs->Next(1, &job, NULL));

		wil::unique_cotaskmem_string jobName;
		RETURN_IF_FAILED(job->GetDisplayName(&jobName));

		// Keep going on failure.
		BitsSampleMethods::RetrieveJobProperties(job.get());

		std::wcout << L"ENUMERATE: Job " << jobIndex << " name=" << jobName.get() << std::endl;
		BitsSampleMethods::EnumerateFiles(job.get());
	}

	return S_OK;
}

// doc code:
// Sample for https://docs.microsoft.com/en-us/windows/win32/bits/enumerating-files-in-a-job// This entirely replaces the code snippet.

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

HRESULT BitsSampleMethods::RetrieveJobProperties(IBackgroundCopyJob* job)
{
	//TODO: convert to wil:: RAII style everywhere

	//Name of the job to use in the user interface. The name is set when you 
	//create the job. You can use the SetDisplayName method to change the name. 
	wil::unique_cotaskmem_string jobName;
	RETURN_IF_FAILED(job->GetDisplayName(&jobName));

	wil::unique_cotaskmem_string ownerSid;
	RETURN_IF_FAILED(job->GetOwner(&ownerSid));

	WCHAR* pszFullName = _wcsdup (L"(owner is not known)");

	PSID pSid = NULL;
	auto bResult = ConvertStringSidToSid(ownerSid.get(), &pSid);
	if (!bResult)
	{
		pszFullName = ownerSid.get();
	}
	else
	{
		//Call LookupAccountSid twice. The first call retrieves the buffer size 
		//for name and domain and the second call retrieves the actual name and domain.
		DWORD cbNameSize = 0;
		DWORD cbDomainSize = 0;

		SID_NAME_USE eNameUse;
		LookupAccountSidW(NULL, pSid, NULL, &cbNameSize,NULL, &cbDomainSize, &eNameUse);
		DWORD LastError = GetLastError();
		if (LastError == ERROR_INSUFFICIENT_BUFFER)
		{
			WCHAR* pszName = (WCHAR*)malloc(sizeof(WCHAR) * cbNameSize);
			WCHAR* pszDomain = (WCHAR*)malloc(sizeof(WCHAR) * cbDomainSize);
			if (pszName && pszDomain)
			{
				bResult = LookupAccountSid(NULL, pSid, pszName, &cbNameSize,pszDomain, &cbDomainSize, &eNameUse);
				if (bResult)
				{
					pszFullName = (WCHAR*)malloc(sizeof(WCHAR)*(cbDomainSize + 1 + cbNameSize + 1));
					if (pszFullName)
					{
						StringCchPrintf(pszFullName, cbDomainSize + 1 + cbNameSize + 1, L"%s\\%s", pszDomain, pszName);
						//TODO: actually free(pszFullName) -- in an RAII fashion...
					}
				}
			}
			if (pszDomain) free(pszDomain);
			if (pszName) free(pszName);
		}
		else
		{
			//Handle error - most likely ERROR_NONE_MAPPED, could not find the SID.
			pszFullName = ownerSid.get();
		}
		LocalFree(pSid);
	}

	BG_JOB_STATE state;
	const WCHAR *JobStates[] = { L"Queued", L"Connecting", L"Transferring",
					   L"Suspended", L"Error", L"Transient Error",
					   L"Transferred", L"Acknowledged", L"Canceled"
	};
	RETURN_IF_FAILED (job->GetState(&state));

	BG_JOB_PROGRESS progress;
	RETURN_IF_FAILED(job->GetProgress(&progress));
	std::wcout << L"Job owner=" << pszFullName << L" state=" << JobStates[state] << L" bytes transferred=" << progress.BytesTransferred << L" files transferred=" << progress.FilesTransferred << std::endl;

	return S_OK;
}
