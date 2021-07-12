#include "BuildContext.h"

hos::nv::BuildContext::BuildContext()
{
}

void hos::nv::BuildContext::dumpLog(const char* format, ...)
{
}

int hos::nv::BuildContext::getLogCount() const
{
	return 0;
}

const char* hos::nv::BuildContext::getLogText(const int i) const
{
	return nullptr;
}

void hos::nv::BuildContext::doResetLog()
{
}

void hos::nv::BuildContext::doLog(const rcLogCategory category, const char* msg, const int len)
{
}

void hos::nv::BuildContext::doResetTimers()
{
}

void hos::nv::BuildContext::doStartTimer(const rcTimerLabel label)
{
}

void hos::nv::BuildContext::doStopTimer(const rcTimerLabel label)
{
}

int hos::nv::BuildContext::doGetAccumulatedTime(const rcTimerLabel label) const
{
	return 0;
}
