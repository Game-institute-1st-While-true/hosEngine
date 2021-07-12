#ifndef _BASE_STRING_H
#define _BASE_STRING_H

// [2020/12/31 ������]
// �⺻Ÿ�Ե��� ������ ���
// �Ϻη� ut���ӽ����̽��� ���������ʾҴ�.

// LOG
// [2021/02/25 ������] c++17���� ���� �����Ϸ����� string_view�� string���� ����ϵ��� ����

#include <limits>
#include <string>
#include <string_view>
#include <strsafe.h>
#include <cmath>

namespace hos
{
	//���̵幮�ڿ��� ������Ʈ�� �⺻���ڿ��� ����
	using string = std::wstring;
	using string_literal = const wchar_t*;

	//��Ƽ����Ʈ ��Ʈ��
	using mbstring = std::string;
	using mbstring_literal = const char*;

	//string_view
#if _HAS_CXX17
	using string_view = std::wstring_view;
	using mbstring_view = std::string_view;
#else
	using string_view = std::wstring;
	using mbstring_view = std::string;
#endif // _MSVC_LANG >=201703L

	//����
	using I8 = std::int8_t;
	using I16 = std::int16_t;
	using I32 = std::int32_t;
	using I64 = std::int64_t;

	//�ڿ���
	using U8 = std::uint8_t;
	using U16 = std::uint16_t;
	using U32 = std::uint32_t;
	using U64 = std::uint64_t;

	//�Ǽ�
	using F32 = float;
	using F64 = double;

	//max�Լ��� ��ũ�η� �ν��ϴ� ���������� ����
#ifdef max
#undef max
#endif // max

	//�ִ밪
	constexpr I8	I8_MAX = std::numeric_limits<I8>::max();
	constexpr I16 	I16_MAX = std::numeric_limits<I16>::max();
	constexpr I32 	I32_MAX = std::numeric_limits<I32>::max();
	constexpr I64 	I64_MAX = std::numeric_limits<I64>::max();
	constexpr U8 	U8_MAX = std::numeric_limits<U8>::max();
	constexpr U16 	U16_MAX = std::numeric_limits<U16>::max();
	constexpr U32 	U32_MAX = std::numeric_limits<U32>::max();
	constexpr U64 	U64_MAX = std::numeric_limits<U64>::max();
	constexpr F32 	F32_MAX = std::numeric_limits<F32>::max();
	constexpr F64 	F64_MAX = std::numeric_limits<F64>::max();

	//�ּҰ�
	constexpr I8 	I8_MIN = std::numeric_limits<I8>::lowest();
	constexpr I16 	I16_MIN = std::numeric_limits<I16>::lowest();
	constexpr I32 	I32_MIN = std::numeric_limits<I32>::lowest();
	constexpr I64 	I64_MIN = std::numeric_limits<I64>::lowest();
	constexpr U8 	U8_MIN = std::numeric_limits<U8>::lowest();
	constexpr U16 	U16_MIN = std::numeric_limits<U16>::lowest();
	constexpr U32 	U32_MIN = std::numeric_limits<U32>::lowest();
	constexpr U64 	U64_MIN = std::numeric_limits<U64>::lowest();
	constexpr F32 	F32_MIN = std::numeric_limits<F32>::lowest();
	constexpr F64 	F64_MIN = std::numeric_limits<F64>::lowest();
}

#if __has_include("DXTK\SimpleMath.h")
#include "DXTK\SimpleMath.h"
#ifndef DXTK_MATH
#define DXTK_MATH
#endif // !DXTK_MATH
#endif // __has_include("DXTK\SimpleMath.h")

#if __has_include("PxPhysicsAPI.h")
#include "PxPhysicsAPI.h"
#ifndef PHYSX_MATH
#define PHYSX_MATH
#endif // !PHYSX_MATH
#endif // __has_include()

#endif // !_BASE_STRING_H