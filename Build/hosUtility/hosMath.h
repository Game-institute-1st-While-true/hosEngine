#ifndef _MATH_H
#define _MATH_H

// [2020/12/31 ������]
// ���� ���̺귯��
// �Ϻη� ut���ӽ����̽��� ���������ʾҴ�.

// [2020/12/20] ��ȿ�� : ���, �л�, ǥ������ �߰�
// [2021/03/10] ������ : ���̺귯�� ���� �߰������ʵ��� ����� ��ũ�� �߰�

#include "BaseTypes.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Color.h"
#include "Matrix.h"
#include "Rotator.h"

#if defined(_DEBUG)||defined(DEBUG)
#if _WIN64
#pragma comment(lib,"hosUtility_x64D")
#else
#pragma comment(lib,"hosUtility_x86D")
#endif // _WIN64
#else
#if _WIN64
#pragma comment(lib,"hosUtility_x64R")
#else
#pragma comment(lib,"hosUtility_x86R")
#endif // _WIN64
#endif // defined(_DEBUG)||defined(DEBUG)

namespace hos
{
	template<typename T>
	constexpr T PI = 3.141592654f;

	template<typename T>
	constexpr T __cdecl RadToDegree(T rad)
	{
		return (rad * 180 / PI<T>);
	}

	template<typename T>
	constexpr T __cdecl DegreeToRad(T degree)
	{
		return (degree * PI<T> / 180);
	}

	template <typename T>
	constexpr T __cdecl Max(T lhs, T rhs)
	{
		return lhs > rhs ? lhs : rhs;
	}

	template <typename T>
	constexpr T __cdecl Min(T lhs, T rhs)
	{
		return lhs < rhs ? lhs : rhs;
	}

	inline Vector3 __cdecl QuaterniontoEulerAngle(Quaternion quaternion)
	{
		float _roll, _pitch, _yaw;
	#pragma region First
		/*	1��
		/// https://m.blog.naver.com/PostView.nhn?blogId=spinx85&logNo=140120555548&proxyReferer=https:%2F%2Fwww.google.com%2F
		float _w = quaternion.w * quaternion.w;
		float _x = quaternion.x * quaternion.x;
		float _y = quaternion.y * quaternion.y;
		float _z = quaternion.z * quaternion.z;

		//y��
		float _pitch = asinf(2.0f * (quaternion.w * quaternion.x - quaternion.y * quaternion.z));
		//z��
		float _yaw = atan2f(2.0f * (quaternion.x * quaternion.z + quaternion.w * quaternion.y), (-_w - _y + _z + _w));
		//x��
		float _roll = atan2f(2.0f * (quaternion.x * quaternion.y + quaternion.w * quaternion.z), (-_x + _y - _z + _w));
		//*/
	#pragma endregion

	#pragma region Second
		/*	2��
		/// wikipedia
		//roll
		double sinr_cosp = 2 * (quaternion.w * quaternion.x + quaternion.y * quaternion.z);
		double cosr_cosp = 1 - 2 * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);
		float _roll = atan2(sinr_cosp, cosr_cosp);
		// pitch (y-axis rotation)
		double sinp = 2 * (quaternion.w * quaternion.y - quaternion.z * quaternion.x);
		float _pitch;
		if (std::abs(sinp) >= 1)
			_pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
		else
			_pitch = std::asin(sinp);

		// yaw (z-axis rotation)
		double siny_cosp = 2 * (quaternion.w * quaternion.z + quaternion.x * quaternion.y);
		double cosy_cosp = 1 - 2 * (quaternion.y * quaternion.y + quaternion.z * quaternion.z);
		float _yaw = std::atan2(siny_cosp, cosy_cosp);
		//*/
	#pragma endregion

	#pragma region Third
		/* 3��
		/// http://quat.zachbennett.com/ <- �������� �ҽ��ڵ�
		float qw2 = quaternion.w * quaternion.w;
		float qx2 = quaternion.x * quaternion.x;
		float qy2 = quaternion.y * quaternion.y;
		float qz2 = quaternion.z * quaternion.z;
		float test = quaternion.x * quaternion.y + quaternion.z * quaternion.w;
		if (test > 0.499f) {
			_pitch = 360.f / PI<float> * atan2f(quaternion.x, quaternion.w);
			_yaw = 90.f;
			_roll = 0.f;
			return Vector3(_roll, _pitch, _yaw);
		}
		if (test < -0.499f) {
			_pitch = -360 / PI<float> * atan2f(quaternion.x, quaternion.w);
			_yaw = -90.f;
			_roll = 0.f;
			return Vector3(_roll, _pitch, _yaw);
		}
		_pitch = atan2f(2 * quaternion.y * quaternion.w - 2 * quaternion.x * quaternion.z, 1 - 2 * qy2 - 2 * qz2);
		_yaw = asinf(2 * quaternion.x * quaternion.y + 2 * quaternion.z * quaternion.w);
		_roll = atan2f(2 * quaternion.x * quaternion.w - 2 * quaternion.y * quaternion.z, 1 - 2 * qx2 - 2 * qz2);
		//*/
	#pragma endregion

	#pragma region Fourth
		/*	Unreal

		const float SingularityTest = quaternion.z * quaternion.x - quaternion.w * quaternion.y;
		const float YawY = 2.f * (quaternion.w * quaternion.z + quaternion.x * quaternion.y);
		const float YawX = (1.f - 2.f * ((quaternion.y * quaternion.y) + (quaternion.z * quaternion.z)));

		// reference
		// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

		// this value was found from experience, the above websites recommend different values
		// but that isn't the case for us, so I went through different testing, and finally found the case
		// where both of world lives happily.
		const float SINGULARITY_THRESHOLD = 0.4999995f;
		const float RAD_TO_DEG = (180.f) / PI<float>;
		float temp = 360.f;

		if (SingularityTest < -SINGULARITY_THRESHOLD)
		{
			_pitch = -90.f;
			_yaw = atan2f(YawY, YawX) * RAD_TO_DEG;
			float t = -_yaw - (2.f * atan2f(quaternion.x, quaternion.w) * RAD_TO_DEG);
			t = modf(t, &temp);
			if (t < 0.f)
			{
				t += 360.f;
			}
			if (t > 180.f)
			{
				t -= 360.f;
			}
			_roll = t;
		}
		else if (SingularityTest > SINGULARITY_THRESHOLD)
		{
			_pitch = 90.f;
			_yaw = atan2f(YawY, YawX) * RAD_TO_DEG;
			float t = -_yaw - (2.f * atan2f(quaternion.x, quaternion.w) * RAD_TO_DEG);
			t = modf(t, &temp);
			if (t < 0.f)
			{
				t += 360.f;
			}
			if (t > 180.f)
			{
				t -= 360.f;
			}
			_roll = t;
		}
		else
		{
			_pitch = asinf(2.f * (SingularityTest)) * RAD_TO_DEG;
			_yaw = atan2f(YawY, YawX) * RAD_TO_DEG;
			_roll = atan2f(-2.f * (quaternion.w * quaternion.x + quaternion.y * quaternion.z), (1.f - 2.f * ((quaternion.x * quaternion.x) + (quaternion.y * quaternion.y)))) * RAD_TO_DEG;
		}

		//*/
	#pragma endregion

	#pragma region final
		float sqw = quaternion.w * quaternion.w;
		float sqx = quaternion.x * quaternion.x;
		float sqy = quaternion.y * quaternion.y;
		float sqz = quaternion.z * quaternion.z;
		float unit = sqx + sqy + sqz + sqw;
		float test = quaternion.x * quaternion.w - quaternion.y * quaternion.z;

		if (test > 0.4995f * unit)
		{
			_pitch = 2.f * atan2f(quaternion.y, quaternion.w);
			_roll = PI<float> / 2.f;
			_yaw = 0;

			_roll = RadToDegree(_roll);
			_pitch = RadToDegree(_pitch);
			_yaw = RadToDegree(_yaw);
			return  Vector3(_roll, _pitch, _yaw);
		}
		if (test < -0.4995f * unit)
		{
			_pitch = -2.f * atan2f(quaternion.y, quaternion.x);
			_roll = -PI<float> / 2.f;
			_yaw = 0;

			_roll = RadToDegree(_roll);
			_pitch = RadToDegree(_pitch);
			_yaw = RadToDegree(_yaw);
			return Vector3(_roll, _pitch, _yaw);
		}

		Quaternion rot = Quaternion(quaternion.w, quaternion.z, quaternion.x, quaternion.y);
		_pitch = atan2f(2.f * rot.x * rot.w + 2.f * rot.y * rot.z, 1 - 2.f * (rot.z * rot.z + rot.w * rot.w));
		_roll = asinf(2.f * (rot.x * rot.z - rot.w * rot.y));												
		_yaw = atan2f(2.f * rot.x * rot.y + 2.f * rot.z * rot.w, 1 - 2.f * (rot.y * rot.y + rot.z * rot.z));  

		_roll = RadToDegree(_roll);
		_pitch = RadToDegree(_pitch);
		_yaw = RadToDegree(_yaw);
	#pragma endregion

		return Vector3(_roll, _pitch, _yaw);
	}

	template <typename T>
	void AddCountNumber(double& out, T rhs, int& cnt)
	{
		cnt++;
		out += rhs;
	}

	// ���� ����
	template <typename T>
	void Deviation_Squared(double& out, T num, double average)
	{
		double input = num - average;
		input = num - average;
		out += (input * input);
	}

	// ���
	template<typename T>
	double Average(std::vector<T> vec)
	{
		double res = 0;
		int size = vec.size();
		for (int i = 0; i < size; i++)
		{
			res += vec[i];
		}
		res /= size;
		return res;
	}

	template<typename T>
	double Variance(std::vector<T> vec)
	{
		double average = 0;
		int cnt = vec.size();
		average = Average(vec);

		double ret = 0;
		for (int i = 0; i < cnt; i++)
		{
			Deviation_Sqaured(ret, vec[i], average);
		}
		return ret / cnt;
	}

	template <typename T>
	double StandardDeviation(std::vector<T> vec)
	{
		double average = 0;
		double variance = 0;
		int cnt = vec.size();

		average = Average(vec);

		for (int i = 0; i < cnt; i++)
		{
			Deviation_Squared(variance, vec[i], average);
		}
		variance /= cnt;
		return sqrt(variance);
	}

	template<typename ...Args>
	double Average(Args... numbers)
	{
		double res = 0;
		int cnt = 0;
		(AddCountNumber(res, numbers, cnt), ...);
		res /= cnt;

		return res;
	}

	// �л�
	template <typename ...Args>
	double Variance(Args... numbers)
	{
		double average = 0;
		int cnt = 0;
		(AddCountNumber(average, numbers, cnt), ...);
		average /= cnt;

		double ret = 0;
		(Deviation_Squared(ret, numbers, average), ...);
		return ret / cnt;
	}

	// ǥ�� ����
	template <typename ...Args>
	double StandardDeviation(Args... numbers)
	{
		double average = 0;
		int cnt = 0;
		(AddCountNumber(average, numbers, cnt), ...);
		average /= cnt;

		double variance = 0;
		(Deviation_Squared(variance, numbers, average), ...);
		variance /= cnt;

		return sqrt(variance);
	}

	__forceinline double RoundDigit(double num, int digit)
	{
		double d = pow(10.0, digit - 1);
		return (round(num * d) / d);
	}
}

#endif // !_MATH_H