
#include "pch.h"
#include "NotifyInterface.h"

// doc update: mention that you need the bits.lib in order to get the CLSID values.
#pragma comment (lib, "bits.lib")
#pragma warning (error: 4706) // Catch error 4706 assignment in conditional expression


// From IBackgroundCopyCallback Interface
// https://docs.microsoft.com/en-us/windows/desktop/api/Bits/nn-bits-ibackgroundcopycallback



void DownloadFile()
{
	_COM_SMARTPTR_TYPEDEF(IBackgroundCopyManager, __uuidof(IBackgroundCopyManager));
	_COM_SMARTPTR_TYPEDEF(IBackgroundCopyJob, __uuidof(IBackgroundCopyJob));
	_com_ptr_t<_com_IIID<IBackgroundCopyManager, &__uuidof(IBackgroundCopyManager)>> bcm = NULL;
	_com_ptr_t<_com_IIID<IBackgroundCopyManager, &__uuidof(IBackgroundCopyManager)>> bcm10_2 = NULL;
	_com_ptr_t<_com_IIID<IBackgroundCopyJob, &__uuidof(IBackgroundCopyJob)>> job = NULL;
	_com_ptr_t<_com_IIID<IBackgroundCopyJob5, &__uuidof(IBackgroundCopyJob5)>> job5 = NULL;

	const ULONG NFilesInSet = 1;
	BG_FILE_INFO* paFiles = NULL;


	const WCHAR *JobStates[] = { L"Queued", L"Connecting", L"Transferring",
						   L"Suspended", L"Error", L"Transient Error",
						   L"Transferred", L"Acknowledged", L"Canceled"
	};

	// Part 1: Connecting to the BITS Service
	// https://docs.microsoft.com/en-us/windows/desktop/bits/connecting-to-the-bits-service
	// Doc update: In the "Before your application exits" section, update the documentation to mention
	// that the _com_ptr_t will automatically release.

	std::wcout << L"Initialize COM" << std::endl;
	// // // TODO: what are the allowed values for the coinitialize?
	// // // Started as COINIT_APARTMENTTHREADED but COINIT_MULTITHREADED is the 'default'
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr)) goto cleanup;

	std::wcout << L"Create the BackgroundCopyManager" << std::endl;
	hr = bcm.CreateInstance(__uuidof(BackgroundCopyManager));
	if (FAILED(hr)) goto cleanup;

	// Part 1: Connecting to the BITS Service
	// The following code shows how to use one of the symbolic class identifiers.
	// This will only work when the code is run on a system that includes BITS 10.2
	std::wcout << L"Create the BackgroundCopyManager for BITS 10.2" << std::endl;
	hr = bcm10_2.CreateInstance(CLSID_BackgroundCopyManager10_2);
	if (FAILED(hr)) goto cleanup;

	// Part 2: Creating a job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/creating-a-job
	std::wcout << L"Create the job" << std::endl;
	GUID JobId;
	hr = bcm->CreateJob(L"My simple job", BG_JOB_TYPE_DOWNLOAD, &JobId, &job);
	if (FAILED(hr)) goto cleanup;

	// To get the latest IBackgroundCopyJob, call the IBackgroundCopyJob::QueryInterface
	// IBackgroundCopyJob5 cindlues the GetProperty and SetProperty interfaces.
	std::wcout << L"Create the IBackgroundCopyJob5 version of the job" << std::endl;
	hr = job->QueryInterface<IBackgroundCopyJob5>(&job5);
	if (FAILED(hr)) goto cleanup;

	// Part 3: Adding Files to a Job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/adding-files-to-a-job	std::wcout << L"Add a file" << std::endl;
	// The c:\temp directory must exist
	hr = job5->AddFile(L"http://www.msftconnecttest.com/ncsi.txt", L"c:\\TEMP\\bitssample-nsci.txt");
	if (FAILED(hr)) goto cleanup;

	// The following example shows how to add multiple files to the job
	paFiles = (BG_FILE_INFO*)malloc(sizeof(BG_FILE_INFO) * NFilesInSet);
	if (paFiles == NULL) goto cleanup;
	//Doc change: I'm using wcsdup here. I'm also using proper array indexing
	paFiles[0].RemoteName = _wcsdup(L"http://www.msftconnecttest.com/");
	paFiles[0].LocalName = _wcsdup(L"c:\\TEMP\\bitssample-page.txt");
	job->AddFileSet(NFilesInSet, paFiles);


	// Part 4: Start the job
	// The link jumps straight to the ::Resume
	// Doc change: change the link to go to a NEW page that will include this mini-sample

 	std::wcout << L"Resume the job to start it" << std::endl;
	hr = job->Resume();
	if (FAILED(hr)) goto cleanup;



	// Part 5: Determining the status of a job
	// Method a: Poll for the status of a job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/polling-for-the-status-of-the-job
	// Doc changes: I uncommented the JobStates so they are fully usable (and fixed the compiler error)
	// Doc changes: I also use the Sleep() instead of a waitable timer (and need to update the docs accordingly)
	// Doc changes: I don't have a switch based on the state any more
	// Doc changes: the exit condition is correct now (includes the ACK state and doesn't include TRANSIENT_ERROR)
	// Doc changes: later on I'll only Complete() the job if it's not in the ACKNOWLEDGED state.
	// Doc changes: I use the State != <value> compare and not <value> != State. The <value> != State always seems totally backwards
	// Doc changes: I also enable #pragma warning (error: 4706) // Catch error 4706 assignment in conditional expression

	BG_JOB_STATE State;
	do
	{
		Sleep(100); // 100 milliseconds
		hr = job->GetState(&State);
		if (FAILED(hr)) goto cleanup;
		std::wcout << L"UPDATE: STATE=" << JobStates[State] << L"\n";
	} while (State != BG_JOB_STATE_TRANSFERRED &&
		State != BG_JOB_STATE_ERROR &&
		State != BG_JOB_STATE_ACKNOWLEDGED); // ACKNOWLEDGED: someone else completed the job for us!
	
	// Part 6: Complete the job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/completing-and-canceling-a-job
	// Doc changes: this section did not include documentation.
	// Doc changes: all this code is new.
	if (State != BG_JOB_STATE_ACKNOWLEDGED)
	{
		std::wcout << L"Complete the job" << std::endl;
		hr = job->Complete();
		if (FAILED(hr)) goto cleanup;
	}
	std::wcout << L"All done" << std::endl;

	return;
cleanup:
	// If the job exists, then cancel it. If cancelling fails, do nothing
	if (job != NULL)
	{
		job->Cancel();
	}
	// Free is guaranteed to handle NULL pointers.
	free(paFiles);
	paFiles = NULL;


	// Part 1: Connecting to the BITS Service
	// Call CoUninitialize()
	CoUninitialize();
	std::wcout << L"ERROR: HRESULT=" << std::hex << hr << std::endl;
}

int main()
{
	std::cout << "Hello World!\n";
	DownloadFile();
	return 0;
}

