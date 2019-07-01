// Doc plan:
// Most BITS mini-samples will be updated
// Not updated:
// Registering to execute a program: https://docs.microsoft.com/en-us/windows/desktop/bits/registering-to-execute-a-program

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
	auto pNotify = new CNotifyInterface(); // For part 5B; the new doesn't do an AddRef.
	pNotify->AddRef();
	BG_JOB_PROGRESS bitsProgress;


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

	// Enumerating jobs in the transfer queue
	// https://docs.microsoft.com/en-us/windows/desktop/bits/enumerating-jobs-in-the-transfer-queue
	// This code won't be part of the sample
	EnumerateJobsAndFiles(bcm);


	// Part 1: Connecting to the BITS Service
	// The following code shows how to use one of the symbolic class identifiers.
	// This will only work when the code is run on a system that includes BITS 10.2
	// TODO: TEAM: no point in doing this other thing. Just do the one standard way. This other way isn't adding anything of value.
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
	// https://docs.microsoft.com/en-us/windows/desktop/bits/adding-files-to-a-job
	std::wcout << L"Add a file" << std::endl;
	// The c:\temp directory must exist
	hr = job5->AddFile(L"http://www.msftconnecttest.com/ncsi.txt", L"c:\\TEMP\\bitssample-nsci.txt");
	if (FAILED(hr)) goto cleanup;

	// The following example shows how to add multiple files to the job
	paFiles = (BG_FILE_INFO*)malloc(sizeof(BG_FILE_INFO) * NFilesInSet);
	// // // TODO: use this other way. and make the strings work. BG_FILE_INFO fileset[1] = { { L"", L"" }  }; //.LocalName = ""; //TODO: make this 
	if (paFiles == NULL) goto cleanup;
	//Doc change: I'm using wcsdup here. I'm also using standard array indexing instead of pointer arithmetic.
	// TODO: maybe cast as a const? Can't keep as wcsdup because that's weird and doesn't get freed automatically.
	paFiles[0].RemoteName = _wcsdup(L"http://www.msftconnecttest.com/");
	paFiles[0].LocalName = _wcsdup(L"c:\\TEMP\\bitssample-page.txt");
	job->AddFileSet(NFilesInSet, paFiles);


	//TODo: team recommendation: this giant sample should just be a set of function calls, one for each logical part of the sample.
	// That wat the big sample is a short little thing with high level details only.
	//TOOD: move this into a seperate function.
	// The code for Part 5b comes before the code for part 4 (Start the job)
	// Part 5b: set up a notifications for job changes
	// https://docs.microsoft.com/en-us/windows/desktop/api/Bits/nf-bits-ibackgroundcopyjob-setnotifyinterface

	hr = job->SetNotifyInterface(pNotify);
	// BITS has taken a reference on the Notify object; remove our reference to it.
	// Remove our reference regardless of whether or not the notify succeeded or not.
	pNotify->Release();
	pNotify = NULL;
	if (FAILED(hr)) goto cleanup;

	hr = job->SetNotifyFlags(BG_NOTIFY_JOB_TRANSFERRED | BG_NOTIFY_JOB_ERROR | BG_NOTIFY_JOB_MODIFICATION);
	if (FAILED(hr)) goto cleanup;


	// CALL FOR OTHER SNIPPET:
	// How to control whether a BITS job is allowed to download over an expensive connection.
	// This function call snippet is not included in the docs
	// https://docs.microsoft.com/en-us/windows/desktop/bits/how-to-block-a-bits-job-from-downloading-over-an-expensive-connection
	

	std::wcout << L"Set the transfer policy" << std::endl;
	hr = SpecifyTransferPolicy(job);
	if (FAILED(hr)) goto cleanup;
	std::wcout << L"Set the transfer policy" << std::endl;





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

	// TODO: when we switch to not having goto cleanup, move the variable definitions closer to where they are used.
	BG_JOB_STATE State;
	do
	{
		Sleep(100); // 100 milliseconds
		hr = job->GetState(&State);
		if (FAILED(hr)) goto cleanup;
		std::wcout << L"POLLING LOOP: STATE=" << JobStates[State] << L"\n";

		// Part 5d: Determining the progress of a job.
		// https://docs.microsoft.com/en-us/windows/desktop/bits/determining-the-progress-of-a-job
		// doc changes: removing all of the StringCchPrintf stuff in favor of plain std::wcout


		hr = job->GetProgress(&bitsProgress);
		if (FAILED(hr))
		{
			//Handle error
			std::wcout << L"POLLING LOOP: Error: unable to get progress for job" << std::endl;
		}
		else
		{
			//Because the BytesTotal member can be 0 or BG_SIZE_UNKNOWN, you may not be able 
			//to determine a percentage value to display, such as 57%. It is best to display a 
			//string that shows the number of bytes transferred. For example, "123456 of 
			//999999" or "123456 of Unknown".
			if (bitsProgress.BytesTotal == BG_SIZE_UNKNOWN)
			{
				std::wcout << L"POLLING LOOP: Transferred " << bitsProgress.BytesTransferred << L" bytes of " << "unknown" << std::endl;
			}
			else
			{
				std::wcout << L"POLLING LOOP: Transferred " << bitsProgress.BytesTransferred << L" bytes of " << bitsProgress.BytesTotal << std::endl;
			}
			std::wcout << L"POLLING LOOP: Transferred " << bitsProgress.FilesTransferred << L" files of " << bitsProgress.FilesTotal << std::endl;
		}


	} while (State != BG_JOB_STATE_TRANSFERRED &&
		State != BG_JOB_STATE_ERROR &&
		State != BG_JOB_STATE_ACKNOWLEDGED); // ACKNOWLEDGED: someone else completed the job for us!
	

	// Part of the display file headers code snippet
	DisplayFileHeaders(job);


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

