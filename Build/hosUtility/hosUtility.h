#ifndef _UTILITY_H
#define _UTILITY_H

// [2020/10/15 ������]
// ��ƿ��Ƽ ���
// ��ƿ��Ƽ ���̺귯������ �ܺη� �����ϰԵ� ����Դϴ�.

// warning : ��ƿ��Ƽ ���̺귯���� ���ο� �۾��� �� ��� ��� ����� �̰��� ���Խ��� ���ƾ� �մϴ�.

// LOG
// [2020/10/22] ������ : STL �����̳� ���ø�������� ���� ���� ���̺귯���� ����
// [2020/11/16] ������ : ��Ʈ�� �κ� BaseString.h ���Ϸ� �̵�
// [2020/12/10] ������ : Utility���� DX���Ӽ��� �����ϱ����� MathHelper.h����
// [2020/12/20] ��ȿ�� : format utility �߰�
// [2020/12/32] ������ : BaseString.h ���� BaseTypes.h�߰� ���������� BaseTypes.h�� ����
// [2021/03/10] ������ : ���̺귯�� ���� �߰������ʵ��� ����� ��ũ�� �߰�

#include "BaseTypes.h"
#include "FormatUtils.h"
#include "Singleton.h"
#include "Delegate.hpp"
#include "Console.h"
#include "Logger.hpp"
#include "Timer.hpp"
#include "Memory.h"

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


//[2020/10/23 ������] max�� min ��ũ�θ� �����ϱ����� ��ó���� ����Ʒ��� ������ ��
//#ifdef max
//#undef max
//#endif // max
//
//#ifdef min
//#undef min
//#endif // min

//#ifdef far
//#undef far
//#endif // !far
//
//#ifdef near
//#undef near
//#endif // near

#endif // !_UTILITY_H