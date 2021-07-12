#include "Clock.h"

gzz::Clock::Clock()
{
	StartTime = std::chrono::steady_clock::now();
	EndTime = StartTime;
	PreTime = StartTime;

	Delta = EndTime - PreTime;
}

gzz::Clock::~Clock()
{
}

void gzz::Clock::Start()
{
	StartTime = std::chrono::steady_clock::now();
	EndTime = StartTime;
	PreTime = StartTime;

	Delta = EndTime - PreTime;
}

void gzz::Clock::Stop()
{
	EndTime = std::chrono::steady_clock::now();
	StartTime = EndTime;
}

double gzz::Clock::Restart()
{
	StartTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> result = StartTime - EndTime;
	return result.count();
}

void gzz::Clock::Tick()
{
	EndTime = std::chrono::steady_clock::now();
	Delta = EndTime - PreTime;
	PreTime = EndTime;
}


const double gzz::Clock::DeltaTime()
{
	return Delta.count();
}

const int gzz::Clock::DeltaTime_Min()
{
	return std::chrono::duration_cast<std::chrono::minutes>(Delta).count();
}

const int gzz::Clock::DeltaTime_Hour()
{
	return std::chrono::duration_cast<std::chrono::hours>(Delta).count();
}

const __int64 gzz::Clock::DeltaTime_Nano()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(Delta).count();
}

const __int64 gzz::Clock::DeltaTime_Micro()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(Delta).count();
}

const __int64 gzz::Clock::DeltaTime_Milli()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(Delta).count();
}

const __int64 gzz::Clock::DeltaTime_Second()
{
	return std::chrono::duration_cast<std::chrono::seconds>(Delta).count();
}

const double gzz::Clock::Now()
{
	std::chrono::duration<double> duration = std::chrono::steady_clock::now() - StartTime;
	return duration.count();
}

const __int64 gzz::Clock::Now_Nano()
{
	std::chrono::nanoseconds nano = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - StartTime);
	return nano.count();
}

const __int64 gzz::Clock::Now_Micro()
{
	std::chrono::microseconds micro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - StartTime);
	return micro.count();
}

const __int64 gzz::Clock::Now_Milli()
{
	std::chrono::milliseconds milli = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - StartTime);
	return milli.count();
}

const __int64 gzz::Clock::Now_Seccond()
{
	std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - StartTime);
	return sec.count();
}

const int gzz::Clock::Now_Min()
{
	std::chrono::minutes min = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - StartTime);
	return min.count();
}

const int gzz::Clock::Now_Hour()
{
	std::chrono::hours hour = std::chrono::duration_cast<std::chrono::hours>(std::chrono::steady_clock::now() - StartTime);
	return hour.count();
}
