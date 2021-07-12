#include "IoDefine.h"
#include "SendItem.h"
#include "AsyncTcpSend.h"

//gzz::SendItem::~SendItem()
//{
//}
//
//int gzz::SendItem::SendMsg(PACKET pack)
//{
//	return Item->SendMsg(Token->GetSock(), static_cast<OVERLAPPED*>(this), pack);
//}
//
//int gzz::SendItem::Process(const int completionBytes)
//{
//	int err = Item->SendProcess(completionBytes);
//
//	switch (err)
//	{
//		case static_cast<int>(NET_ERR::SEND_ERROR_NEED_MORE_DATA) :
//		{
//			PACKET pack{ 0, };
//			Item->SendMsg(Token->GetSock(), static_cast<OVERLAPPED*>(this), pack);	}
//		break;
//		case static_cast<int>(static_cast<int>(NET_ERR::SEND_ERROR_MORE_DATA)) :
//			Token->Close();
//			return static_cast<int>(NET_ERR::SEND_PROCESS_FAIL);
//	}
//	return static_cast<int>(NET_ERR::SUCESS);
//}
//

gzz::SendItemEX::SendItemEX(int id)
	:ICompletionItemEX(id), Socket(nullptr), Send(nullptr)
{
}

gzz::SendItemEX::SendItemEX(int id, TcpSocketEX* sock, ISend* item)
	: ICompletionItemEX(id), Socket(sock), Send(item)
{
}

gzz::SendItemEX::~SendItemEX()
{
}

int gzz::SendItemEX::SendMsg(PACKET pack)
{
	return Send->SendMsg(Socket->GetSock(), static_cast<OVERLAPPED*>(this), pack);
}

int gzz::SendItemEX::Process(const int completionBytes, LPOVERLAPPED other)
{
	if (completionBytes <= 0)
	{
		Socket->Close(other);
	}
	else
	{
		int err = Send->SendProcess(completionBytes);

		if (err == static_cast<int>(NET_ERR::SEND_ERROR_MORE_DATA))
		{
			Socket->Close(other);
		}
		
		// 여기서 남은 데이터를 보내준다.
		static_cast<AsyncTcpSend*>(Send)->RemainingMsg(Socket->GetSock(), static_cast<OVERLAPPED*>(this));
	}
	return static_cast<int>(NET_ERR::SUCESS);
}

void gzz::SendItemEX::SettingItem(TcpSocketEX* sock, ISend* item)
{
	Socket = sock;
	Send = item;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//gzz::SendItemEX::SendItemEX(int id, LockedCQ<int>* ring)
//	:ICompletionItemEX(id), IdQueue(ring), Send(nullptr), Socket(nullptr)
//{
//	Packet.head.protocol = 0;
//	Packet.head.size = 0;
//}
//gzz::SendItemEX::~SendItemEX()
//{
//}
//
//
//int gzz::SendItemEX::SendMsg(PACKET pack)
//{
//	Packet = pack;
//	return Send->SendMsg(Socket->GetSock(), static_cast<OVERLAPPED*>(this), Packet);
//}
//
//void gzz::SendItemEX::SettingItem(TcpSocketEX* sock, ISend* item)
//{
//	Socket = sock;
//	SockId = Socket->GetId();
//	Send = item;
//}
//
//int gzz::SendItemEX::Process(const int completionBytes, LPOVERLAPPED other)
//{	
//	if (completionBytes <= 0)
//	{
//		Socket->Close(other);
//	}
//	else
//	{
//		if (Packet.head.size != completionBytes)
//		{
//			Socket->Close(other);
//		}
//
//
//		/*int err = Send->SendProcess(completionBytes);
//
//		if (err != static_cast<int>(NET_ERR::SUCESS))
//		{
//			Socket->Close(other);
//		}*/
//	}
//
//	Packet.head.protocol = 0;
//	Packet.head.size = 0;
//	
//	Socket = nullptr;
//	Send = nullptr;
//	
//	IdQueue->Enqueue(Id);
//	
//	return static_cast<int>(NET_ERR::SUCESS);
//}
