#pragma once
#include <functional>
#include "NetDefine.h"

#include "TcpHostEX.h"
#include "TcpGuestEX.h"

#include "TcpClient.h"
#include "TcpClientEX.h"

#include "ICompletionItem.h"
#include "CompletionItemDefine.h"

#include "ErrCode.h"

#include "RecvItem.h"
#include "SendItem.h"
#include "SessionItem.h"

namespace gzz
{
	// CONNECT ���ϰ� �����ϱ����� Key DATA. 
	constexpr int CONNECT = -77;

	// Wait time = 120 ��
	constexpr int DEFAULT_WAIT_MILLITIME = 1200000;

	// Connection time = 5��, ������ �Ǿ����� ���� ��Ŷ�� 5�а� �߼۵��� �ʾ����� ���� ����. 
	constexpr int DEFAULT_CONNECTION_TIME = 300;

	// Accept time = 1��, ������ �Ǿ����� ���� ��Ŷ�� 1�а� �߻����� �ʾ����� ���� ����. �� ������ �� �� ª�� �� �� �ִ�.
	//				 DDOS�� ���, �ʴ����� Ŀ�ؼ��� ��ƹ����� ������.
	constexpr int DEFAULT_ACCEPT_TIME = 60;
	// 
	constexpr int CHECKER_CONNECT_CHECK_MILLITIME = 1000;



}