// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Doc plan:
// Most BITS mini-samples will be updated
// Not updated:
// Registering to execute a program: https://docs.microsoft.com/en-us/windows/desktop/bits/registering-to-execute-a-program
// Using WIL wrappers https://github.com/microsoft/wil/wiki/WinRT-and-COM-wrappers


#include "pch.h"
#include "BitsSampleMethods.h"

// doc update: mention that you need the bits.lib in order to get the CLSID values.
#pragma comment (lib, "bits.lib")
#pragma warning (error: 4706) // Catch error 4706 assignment in conditional expression


// From IBackgroundCopyCallback Interface
// https://docs.microsoft.com/en-us/windows/desktop/api/Bits/nn-bits-ibackgroundcopycallback



HRESULT DemonstrateBitsUsage()
{
	// Part 1: Connecting to the BITS Service
	// https://docs.microsoft.com/en-us/windows/desktop/bits/connecting-to-the-bits-service
	// Doc update: In the "Before your application exits" section, update the documentation to mention
	// that the _com_ptr_t will automatically release.

	std::wcout << L"Initialize COM" << std::endl;
	// TODO: what are the allowed values for the coinitialize?
	// Started as COINIT_APARTMENTTHREADED but COINIT_MULTITHREADED is the 'default'
	auto uninitialize = wil::CoInitializeEx(COINIT_MULTITHREADED);
	
	std::wcout << L"Create the BackgroundCopyManager" << std::endl;
	// BITS runs in a seperate process, so you have to specify CLSCTX_LOCAL_SERVER
	wil::com_ptr_nothrow<IBackgroundCopyManager> bcm 
		= wil::CoCreateInstance<BackgroundCopyManager, IBackgroundCopyManager>(CLSCTX_LOCAL_SERVER);
	if (!bcm)
	{
		return REGDB_E_CLASSNOTREG; // wil: will swallow the original HRESULT value.
	}


	// Enumerating jobs in the transfer queue
	// https://docs.microsoft.com/en-us/windows/desktop/bits/enumerating-jobs-in-the-transfer-queue
	RETURN_IF_FAILED (BitsSampleMethods::EnumerateJobsAndFiles(bcm.get()));

	// Part 1: Connecting to the BITS Service
	// The following code shows how to use one of the symbolic class identifiers.
	// This will only work when the code is run on a system that includes BITS 10.2
	// doc change: TEAM: no point in doing this other thing. Just do the one standard way. This other way isn't adding anything of value.


	// Part 2: Creating a job
	// https://docs.microsoft.com/en-us/windows/desktop/bits/creating-a-job
	std::wcout << L"Create the job" << std::endl;
	GUID JobId;
	wil::com_ptr_nothrow<IBackgroundCopyJob> job;
	RETURN_IF_FAILED (bcm->CreateJob(L"My simple job", BG_JOB_TYPE_DOWNLOAD, &JobId, &job));


	// Part 3: Adding Files to a Job
	RETURN_IF_FAILED(BitsSampleMethods::AddFileToJob(job.get()));

	RETURN_IF_FAILED(BitsSampleMethods::AddFilesToJob(job.get()));
	
	// The code for Part 5b comes before the code for part 4 (Start the job)

	// doc change: we need another snippet here:
	// How to control whether a BITS job is allowed to download over an expensive connection.
	// This function call snippet is not included in the docs
	// https://docs.microsoft.com/en-us/windows/desktop/bits/how-to-block-a-bits-job-from-downloading-over-an-expensive-connection
	
	std::wcout << L"Set the transfer policy" << std::endl;
	RETURN_IF_FAILED(BitsSampleMethods::SpecifyTransferPolicy(job.get()));

	// Part 4: Start the job



	BG_JOB_STATE state;
	RETURN_IF_FAILED(BitsSampleMethods::PollJobState(job.get(), &state));

	
	BitsSampleMethods::DisplayFileHeaders(job.get());

	if (state != BG_JOB_STATE_ACKNOWLEDGED)
	{
		RETURN_IF_FAILED(BitsSampleMethods::CompleteJob(job.get()));
	}

	std::wcout << L"All done" << std::endl;

	return S_OK;
}

int main()
{
	std::cout << "Hello World!\n";
	DemonstrateBitsUsage();
	return 0;
}

