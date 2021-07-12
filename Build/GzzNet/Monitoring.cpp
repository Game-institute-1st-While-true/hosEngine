#include "Monitoring.h"

#define FixInRange(minnum, x, maxnum) min(maxnum, max(x, minnum))

gzz::Monitoring::Monitoring()
	:CpuQuery(nullptr), CpuTotal(nullptr)
{
	SYSTEM_INFO sysInfo;
	FILETIME fcreate, fkernel, fuser;

	PdhOpenQuery(NULL, NULL, &CpuQuery);

	if (CpuTotal)
	{
		PdhRemoveCounter(CpuTotal);
	}

	PdhAddCounter(CpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &CpuTotal);
	PdhCollectQueryData(CpuQuery);

	GetSystemInfo(&sysInfo);
	ProcessCount = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&fcreate);
	memcpy(&LastCpu, &fcreate, sizeof(FILETIME));
	ProcessHandle = GetCurrentProcess();

	GetProcessTimes(
		ProcessHandle,		// 프로세스의 핸들
		&fcreate,			// 프로세스의 생성 시간 
		&fcreate,			// 프로세스의 종료 시간 종료되지 않았을 경우 내용정의를 하지 않습니다.
		&fkernel,			// 커널 모드에서 실행된 시간
		&fuser);			// 유저 모드에서 실행된 시간

	memcpy(&LastKernelCpu, &fkernel, sizeof(FILETIME));
	memcpy(&LastUserCpu, &fuser, sizeof(FILETIME));
}

gzz::Monitoring::~Monitoring()
{
}

double gzz::Monitoring::ThisProcessCpuUsage()
{
	SYSTEM_INFO sysinfo;
	FILETIME fcreate, fkernel, fuser;
	ULARGE_INTEGER now;
	ULARGE_INTEGER kernel;
	ULARGE_INTEGER user;

	double percent = 0.0;

	GetSystemInfo(&sysinfo);
	int procsscount = sysinfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&fcreate);
	memcpy(&now, &fcreate, sizeof(FILETIME));

	ProcessHandle = GetCurrentProcess();

	GetProcessTimes(
		ProcessHandle,		// 프로세스의 핸들
		&fcreate,			// 프로세스의 생성 시간 
		&fcreate,			// 프로세스의 종료 시간 종료되지 않았을 경우 내용정의를 하지 않습니다.
		&fkernel,			// 커널 모드에서 실행된 시간
		&fuser);			// 유저 모드에서 실행된 시간

	memcpy(&kernel, &fkernel, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));

	percent = (double)((kernel.QuadPart - LastKernelCpu.QuadPart) + user.QuadPart - LastUserCpu.QuadPart);

	percent /= (now.QuadPart - LastCpu.QuadPart);
	percent /= ProcessCount;
	percent *= 100;

	return percent;
	return FixInRange(0, percent, 100);
}

double gzz::Monitoring::CpuUsage()
{
	PDH_FMT_COUNTERVALUE counterVal;
	PdhCollectQueryData(CpuQuery);
	PdhGetFormattedCounterValue(CpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
	return counterVal.doubleValue;
}

SIZE_T gzz::Monitoring::WorkingSet()
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	return pmc.WorkingSetSize;	// 작업세트 사이즈(Working Set)
}

SIZE_T gzz::Monitoring::PhysicMemory()
{
	MEMORYSTATUSEX meminfo;
	meminfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&meminfo);

	return static_cast<SIZE_T>(meminfo.ullTotalPhys - meminfo.ullAvailPhys); // 실제 물리적 메모리의 양 - 현재 사용 가능한 실제 메모리 양
}

