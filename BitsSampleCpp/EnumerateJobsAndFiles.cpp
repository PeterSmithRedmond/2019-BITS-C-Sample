#include "pch.h"

// https://docs.microsoft.com/en-us/windows/desktop/bits/enumerating-jobs-in-the-transfer-queue

// doc changes: uses a passed-in BackgroundCopyManager instead of a global.
// doc changes: uses _com_ptr_t
// doc changes: use this fancy scheme for the 'raw' pointers we get from GetLocalName, etc.

struct CoTaskMemDeleter {
	void operator()(void *p) { ::CoTaskMemFree(p); }
};

HRESULT EnumerateJobsAndFiles(_com_ptr_t<_com_IIID<IBackgroundCopyManager, &__uuidof(IBackgroundCopyManager)>> mgr)
{
	_com_ptr_t<_com_IIID<IEnumBackgroundCopyJobs, &__uuidof(IEnumBackgroundCopyJobs)>> jobs;
	_com_ptr_t<_com_IIID<IBackgroundCopyJob, &__uuidof(IBackgroundCopyJob)>> job;

	_com_ptr_t<_com_IIID<IEnumBackgroundCopyFiles, &__uuidof(IEnumBackgroundCopyFiles)>> files;
	_com_ptr_t<_com_IIID<IBackgroundCopyFile, &__uuidof(IBackgroundCopyFile)>> file;

	ULONG jobCount = 0;

	HRESULT hr = mgr->EnumJobs(0, &jobs); // 0 means enumerate just for this user
	if (FAILED(hr)) goto cleanup;
	 //TODO: make an enum for BG_JOB_ENUM_CURRENT_USER (file bug)

	jobs->GetCount(&jobCount);
	std::wcout << L"Enumerate: job count=" << jobCount << std::endl;

	for (ULONG jobIndex = 0; jobIndex < jobCount; jobIndex++)
	{
		hr = jobs->Next(1, &job, NULL);
		if (FAILED(hr)) goto cleanup;

		WCHAR* pszJobName = NULL;
		hr = job->GetDisplayName(&pszJobName);
		if (FAILED(hr)) continue;
		std::unique_ptr<WCHAR, CoTaskMemDeleter> jobNameGuard(pszJobName);

		std::wcout << L"ENUMERATE: Job " << jobIndex << " name=" << pszJobName << std::endl;

		//TODO: convert ;this section to be its own function.
		// doc: Enumerating files in a job
		// https://docs.microsoft.com/en-us/windows/desktop/bits/enumerating-files-in-a-job

		hr = job->EnumFiles(&files);
		if (FAILED(hr)) goto cleanup;
		ULONG fileCount = 0;
		hr = files->GetCount(&fileCount);
		if (FAILED(hr)) goto cleanup;

		for (ULONG fileIndex = 0; fileIndex < fileCount; fileIndex++)
		{
			hr = files->Next(1, &file, NULL);
			if (FAILED(hr)) goto cleanup;

			WCHAR *remoteName;
			// WCHAR *localName;
			hr = file->GetRemoteName(&remoteName); //handle hr.
			std::unique_ptr<WCHAR, CoTaskMemDeleter> remoteNameGuard(remoteName);

			std::unique_ptr<LPWSTR, CoTaskMemDeleter> localName; //TODO: figure out the way to use the WIL stuff for this
			hr = file->GetLocalName(localName.get());
			// std::unique_ptr<WCHAR, CoTaskMemDeleter> localNameGuard(localName);

			std::wcout << L"    ENUMERATE: file " << fileIndex << " remote=" << remoteName << " local=" << localName << std::endl;
		}

		// doc: End of enumerating files in a job
	}

	return hr; // S_OK
cleanup:
	return hr;
}