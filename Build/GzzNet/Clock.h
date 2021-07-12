#pragma once
#include <chrono>
#include <string>	
#include <string_view>

// [2020/10/26 ��ȿ��]
// ���� Ÿ�̸� � ���� Ÿ�̸� �Դϴ�. 
// Nano�� ���е��� �����ϴ�.

namespace gzz
{
	class Clock
	{
		std::chrono::steady_clock::time_point		StartTime;
		std::chrono::steady_clock::time_point		EndTime;
		std::chrono::steady_clock::time_point		PreTime;

		std::chrono::duration<double> Delta;

	public:
		Clock();
		~Clock();

	public:
		// �ð��� ���������� �����մϴ�.
		void Start();
		void Stop();

		// Pause Time (�����ִ� �ð�) ����
		double Restart();

		void Tick();

		// get delta
		const double DeltaTime();			// base delta
		const int DeltaTime_Min();			// to min
		const int DeltaTime_Hour();			// to hour
		const __int64 DeltaTime_Nano();		// to nano
		const __int64 DeltaTime_Micro();	// to micro
		const __int64 DeltaTime_Milli();	// to milli
		const __int64 DeltaTime_Second();	// to sec	

		// ���� Ÿ�� ī��Ʈ, ���е� ��������.
		const double Now();				// base time
		const int Now_Min();			// to min
		const int Now_Hour();			// to hour
		const __int64 Now_Nano();		// to nano
		const __int64 Now_Micro();		// to micro
		const __int64 Now_Milli();	// to milli
		const __int64 Now_Seccond();	// to sec	

	private:
	};
}