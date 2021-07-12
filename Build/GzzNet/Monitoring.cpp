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
		ProcessHandle,		// ���μ����� �ڵ�
		&fcreate,			// ���μ����� ���� �ð� 
		&fcreate,			// ���μ����� ���� �ð� ������� �ʾ��� ��� �������Ǹ� ���� �ʽ��ϴ�.
		&fkernel,			// Ŀ�� ��忡�� ����� �ð�
		&fuser);			// ���� ��忡�� ����� �ð�

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
		ProcessHandle,		// ���μ����� �ڵ�
		&fcreate,			// ���μ����� ���� �ð� 
		&fcreate,			// ���μ����� ���� �ð� ������� �ʾ��� ��� �������Ǹ� ���� �ʽ��ϴ�.
		&fkernel,			// Ŀ�� ��忡�� ����� �ð�
		&fuser);			// ���� ��忡�� ����� �ð�

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
	return pmc.WorkingSetSize;	// �۾���Ʈ ������(Working Set)
}

SIZE_T gzz::Monitoring::PhysicMemory()
{
	MEMORYSTATUSEX meminfo;
	meminfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&meminfo);

	return static_cast<SIZE_T>(meminfo.ullTotalPhys - meminfo.ullAvailPhys); // ���� ������ �޸��� �� - ���� ��� ������ ���� �޸� ��
}

