#pragma once

class BitsSampleMethods
{
public:
	static HRESULT AddFileToJob(IBackgroundCopyJob* job);
	static HRESULT AddFilesToJob(IBackgroundCopyJob* job);
	static HRESULT StartJob(IBackgroundCopyJob* job);
	static HRESULT CompleteJob(IBackgroundCopyJob* job);
	static HRESULT PollJobState(IBackgroundCopyJob* job, BG_JOB_STATE* pState);
	static HRESULT SetNotifyInterface(IBackgroundCopyJob* job);

	static HRESULT EnumerateFiles(IBackgroundCopyJob* job);
	static HRESULT EnumerateJobsAndFiles(IBackgroundCopyManager* mgr);
	static HRESULT SpecifyTransferPolicy(IBackgroundCopyJob* job);
	static HRESULT DisplayFileHeaders(IBackgroundCopyJob* job);
};
