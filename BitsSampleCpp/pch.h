#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bits.h>
#include <iostream>
#include <ostream>

// For the _com_ptr_t smart COM pointer
#include <comdef.h>
#include <comip.h>


// Functions that are uses here and there.
HRESULT EnumerateJobsAndFiles(_com_ptr_t<_com_IIID<IBackgroundCopyManager, &__uuidof(IBackgroundCopyManager)>> mgr);
HRESULT SpecifyTransferPolicy(_com_ptr_t<_com_IIID<IBackgroundCopyJob, &__uuidof(IBackgroundCopyJob)>> job);
HRESULT DisplayFileHeaders(_com_ptr_t<_com_IIID<IBackgroundCopyJob, &__uuidof(IBackgroundCopyJob)>> job);

#endif //PCH_H
