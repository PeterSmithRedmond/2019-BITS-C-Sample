
#include "pch.h"
// // // iostream should be in pch.h
// // // #include <iostream>  // // // this is defined multiple times?

// // // Really -- all these globals?
IBackgroundCopyManager* g_pbcm = NULL;
GUID JobId;
IBackgroundCopyJob* pJob = NULL;
IBackgroundCopyJob5* pJob5 = NULL; // // // why not always set up pjob5? job4 is default in windows 7 job5 is windows 8. Windows 7 is EOL in January 14, 2020!
HRESULT hr; // // // seriously never have hr as a global!


bool ConnectToBITS()
{
	if (g_pbcm == NULL)
	{
		hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (SUCCEEDED(hr))
		{
			hr = CoCreateInstance(__uuidof(BackgroundCopyManager), NULL,
				CLSCTX_LOCAL_SERVER,
				__uuidof(IBackgroundCopyManager),
				(void**)&g_pbcm);
			if (SUCCEEDED(hr))
			{
				//Use g_pbcm to create, enumerate, or retrieve jobs from the queue.
			}
		}
	}
	return g_pbcm != NULL;
}

// https://docs.microsoft.com/en-us/windows/desktop/Bits/creating-a-job
bool CreateJob()
{
	bool Retval = false;
	//To create an upload job, replace BG_JOB_TYPE_DOWNLOAD with 
	//BG_JOB_TYPE_UPLOAD or BG_JOB_TYPE_UPLOAD_REPLY.
	hr = g_pbcm->CreateJob(L"MyJobName", BG_JOB_TYPE_DOWNLOAD, &JobId, &pJob);
	if (SUCCEEDED(hr))
	{
		//Save the JobId for later reference. 
		//Modify the job's property values.
		//Add files to the job.
		//Activate (resume) the job in the transfer queue.
		hr = pJob->QueryInterface(__uuidof(IBackgroundCopyJob5), (void**)&pJob5);
		pJob->Release();
		if (FAILED(hr)) // // //TODO: inconsitant use of SUCCESS and FAILURE
		{
			wprintf(L"pJob->QueryInterface failed with 0x%x.\n", hr);
			// // //TODO: goto cleanup;
		}

		Retval = SUCCEEDED(hr);
	}
	return Retval;
}

// // // https://docs.microsoft.com/en-us/windows/desktop/Bits/adding-files-to-a-job
bool AddFileToJob()
{
	bool Retval = false;
	//Replace parameters with variables that contain valid paths.
	// // //TODO: pick values that will "always" work!!
	hr = pJob->AddFile(L"https://ServerName/Path/File.Ext", L"d:\\Path\\File.Ext");
	hr = pJob->AddFile(L"http://www.msftconnecttest.com/ncsi.txt", L"c:\\TEMP\\File.Ext");
	if (SUCCEEDED(hr))
	{
		//Do something.
		Retval = true;
	}
	return Retval;
}

// // // https://docs.microsoft.com/en-us/windows/desktop/Bits/polling-for-the-status-of-the-job
bool PollJob()
{
	BG_JOB_STATE State;
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	// // // TODO: these had been all commented out; IMHO we should leave them be
	//IBackgroundCopyError* pError = NULL;
	//BG_JOB_PROGRESS Progress;
	// // // TODO: must be const WCHAR *! Otherwise the compiler complains
	const WCHAR *JobStates[] = { L"Queued", L"Connecting", L"Transferring",
	                       L"Suspended", L"Error", L"Transient Error",
	                       L"Transferred", L"Acknowledged", L"Canceled"
	                     };

	liDueTime.QuadPart = -10000000;  //Poll every 1 second
	liDueTime.QuadPart = -1000000;  //Poll every 1 second // // // TODO: poll 10x per second
	hTimer = CreateWaitableTimer(NULL, FALSE, L"MyTimer");
	SetWaitableTimer(hTimer, &liDueTime, 1000, NULL, NULL, 0);

	do
	{
		WaitForSingleObject(hTimer, INFINITE);

		//Use JobStates[State] to set the window text in a user interface.
		hr = pJob->GetState(&State);
		if (FAILED(hr))
		{
			//Handle error
		}
		// // // TODO: added this else
		else
		{
			// // // TODO: other uses of std::cout use regular strings, not wide strings. Using regular strings
			// // // here will cause the JobsStates[State] to print as a pointer!
			std::wcout << L"UPDATE: STATE=" << JobStates[State] << L"\n";
		if (BG_JOB_STATE_TRANSFERRED == State) { // // // TODO: added braces everywhere here
			//Call pJob->Complete(); to acknowledge that the transfer is complete
			//and make the file available to the client.
		}
		else if (BG_JOB_STATE_ERROR == State || BG_JOB_STATE_TRANSIENT_ERROR == State) { // // // TODO: added braces everywhere here
			//Call pJob->GetError(&pError); to retrieve an IBackgroundCopyError interface 
			//pointer which you use to determine the cause of the error.
		}
		else if (BG_JOB_STATE_TRANSFERRING == State) { // // // TODO: added braces everywhere here
		 //Call pJob->GetProgress(&Progress); to determine the number of bytes 
		 //and files transferred.
		}
	}
	} while (BG_JOB_STATE_TRANSFERRED != State &&
		BG_JOB_STATE_ERROR != State &&
		BG_JOB_STATE_TRANSIENT_ERROR != State);
	CancelWaitableTimer(hTimer);
	CloseHandle(hTimer);
	return true; // // // TODO: return a more useful value for success/failure
}


int main()
{
	std::cout << "Hello World!\n";
	bool connectOK = ConnectToBITS();
	std::cout << "ConnectToBITS ok=" << connectOK << std::endl; // // //  "\n";
	if (!connectOK) return 1;

	bool createOK = CreateJob();
	std::cout << "CreateJob ok=" << createOK << "\n";
	if (!createOK) return 2;

	bool addFileOK = AddFileToJob();
	std::cout << "AddFile ok=" << addFileOK << "\n";
	if (!addFileOK) return 3;

	// // // TODO: we never tell people to RESUME the job!!
	hr = pJob->Resume();
	std::cout << "RESUME hr=" << hr << "\n";
	if (!SUCCEEDED(hr)) return 4;

	std::cout << "Starting to wait..." << "\n";
	PollJob();
	std::cout << "...Wait complete" << "\n";

	// // // TODO: no code sample for completing a job!
	hr = pJob->Complete();
	std::cout << "COMPLETE hr=" << hr << "\n";

	std::cout << "ALL DONE\n";
	return 0;
}

