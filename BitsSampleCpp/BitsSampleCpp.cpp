// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Doc plan:
// Most BITS mini-samples will be updated
// Not updated:
// Registering to execute a program: https://docs.microsoft.com/en-us/windows/desktop/bits/registering-to-execute-a-program
// Using WIL wrappers https://github.com/microsoft/wil/wiki/WinRT-and-COM-wrappers


#include "pch.h"


// doc update: mention that you need the bits.lib in order to get the CLSID values.
#pragma comment (lib, "bits.lib")
#pragma warning (error: 4706) // Catch error 4706 assignment in conditional expression


// From IBackgroundCopyCallback Interface
// https://docs.microsoft.com/en-us/windows/desktop/api/Bits/nn-bits-ibackgroundcopycallback



HRESULT DownloadFile()
{
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
	//TODO: what's the WIL way to uninitialize?
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	IFFAILRETURN(hr);

	std::wcout << L"Create the BackgroundCopyManager" << std::endl;
	// BITS runs in a seperate process, so you have to specify CLSCTX_LOCAL_SERVER
	wil::com_ptr_nothrow<IBackgroundCopyManager> bcm 
		= wil::CoCreateInstance<BackgroundCopyManager, IBackgroundCopyManager>(CLSCTX_LOCAL_SERVER);

	IFFAILRETURN(hr); //TODO: error; hr hasn't been set by this call.

	// Enumerating jobs in the transfer queue
	// https://docs.microsoft.com/en-us/windows/desktop/bits/enumerating-jobs-in-the-transfer-queue
	// This code won't be part of the sample
	EnumerateJobsAndFiles(bcm.get());


	// Part 1: Connecting to the BITS Service
	// The following code shows how to use one of the symbolic class identifiers.
	// This will only work when the code is run on a system that includes BITS 10.2
	// TODO: TEAM: no point in doing this other thing. Just do the one standard way. This other way isn't adding anything of value.


	// Part 2: Creating a job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/creating-a-job
	std::wcout << L"Create the job" << std::endl;
	GUID JobId;
	wil::com_ptr_nothrow<IBackgroundCopyJob> job;
	hr = bcm->CreateJob(L"My simple job", BG_JOB_TYPE_DOWNLOAD, &JobId, &job);
	IFFAILRETURN(hr);

	// To get the latest IBackgroundCopyJob, call the IBackgroundCopyJob::QueryInterface
	// IBackgroundCopyJob5 cindlues the GetProperty and SetProperty interfaces.
	std::wcout << L"Create the IBackgroundCopyJob5 version of the job" << std::endl;
	wil::com_ptr_nothrow<IBackgroundCopyJob5> job5;
	hr = job->QueryInterface<IBackgroundCopyJob5>(&job5);
	IFFAILRETURN(hr);

	// Part 3: Adding Files to a Job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/adding-files-to-a-job
	std::wcout << L"Add a file" << std::endl;
	// The c:\temp directory must exist
	hr = job5->AddFile(L"http://www.msftconnecttest.com/ncsi.txt", L"c:\\TEMP\\bitssample-nsci.txt");
	IFFAILRETURN(hr);

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
	job->AddFileSet(NFilesInSet, paFiles);


	//TODO: team recommendation: this giant sample should just be a set of function calls, one for each logical part of the sample.
	// That way the big sample is a short little thing with high level details only.
	//TOOD: move this into a seperate function.
	// The code for Part 5b comes before the code for part 4 (Start the job)
	// Part 5b: set up a notifications for job changes
	// https://docs.microsoft.com/en-us/windows/desktop/api/Bits/nf-bits-ibackgroundcopyjob-setnotifyinterface

	auto pNotify = new CNotifyInterface(); // For part 5B; the new doesn't do an AddRef.
	pNotify->AddRef();
	hr = job->SetNotifyInterface(pNotify);
	// BITS has taken a reference on the Notify object; remove our reference to it.
	// Remove our reference regardless of whether or not the notify succeeded or not.
	pNotify->Release();
	pNotify = NULL;
	IFFAILRETURN(hr);

	hr = job->SetNotifyFlags(BG_NOTIFY_JOB_TRANSFERRED | BG_NOTIFY_JOB_ERROR | BG_NOTIFY_JOB_MODIFICATION);
	IFFAILRETURN(hr);


	// CALL FOR OTHER SNIPPET:
	// How to control whether a BITS job is allowed to download over an expensive connection.
	// This function call snippet is not included in the docs
	// https://docs.microsoft.com/en-us/windows/desktop/bits/how-to-block-a-bits-job-from-downloading-over-an-expensive-connection
	

	std::wcout << L"Set the transfer policy" << std::endl;
	hr = SpecifyTransferPolicy(job.get());
	IFFAILRETURN(hr);
	std::wcout << L"Set the transfer policy" << std::endl;





	// Part 4: Start the job
	// The link jumps straight to the ::Resume
	// Doc change: change the link to go to a NEW page that will include this mini-sample

 	std::wcout << L"Resume the job to start it" << std::endl;
	hr = job->Resume();
	IFFAILRETURN(hr);



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
		IFFAILRETURN(hr);
		std::wcout << L"POLLING LOOP: STATE=" << JobStates[State] << L"\n";

		// Part 5d: Determining the progress of a job.
		// https://docs.microsoft.com/en-us/windows/desktop/bits/determining-the-progress-of-a-job
		// doc changes: removing all of the StringCchPrintf stuff in favor of plain std::wcout


		BG_JOB_PROGRESS bitsProgress;
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
	DisplayFileHeaders(job.get());


	// Part 6: Complete the job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/completing-and-canceling-a-job
	// Doc changes: this section did not include documentation.
	// Doc changes: all this code is new.
	if (State != BG_JOB_STATE_ACKNOWLEDGED)
	{
		std::wcout << L"Complete the job" << std::endl;
		hr = job->Complete();
		IFFAILRETURN(hr);
	}
	std::wcout << L"All done" << std::endl;

	return hr;
}

int main()
{
	std::cout << "Hello World!\n";
	DownloadFile();
	return 0;
}

