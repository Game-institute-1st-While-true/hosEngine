#include "RecvItem.h"

//gzz::RecvItem::~RecvItem()
//{
//}
//
//int gzz::RecvItem::Process(const int completionBytes)
//{
//	// 
//	int size = completionBytes;
//	int readbytes = 0;
//	bool IsPackage = true;
//
//	while (IsPackage)
//	{
//		PACKET _packet = { 0, };
//		_packet = Item->GetPacket(size, readbytes);
//
//		switch (static_cast<RECV_STATUS>(_packet.head.size))
//		{
//		case RECV_STATUS::OK:
//			IsPackage = false;
//			break;
//		case RECV_STATUS::MORE:
//			IsPackage = false;
//			break;
//		case RECV_STATUS::FAIL:
//			IsPackage = false;
//			Token->ForceClose();
//			return static_cast<int>(NET_ERR::RECV_PROCESS_FAIL_DATA);
//		default:
//			PACKAGE pack(Token->GetID(), _packet);
//			Queue->Enqueue(pack);
//			size -= _packet.head.size;
//			readbytes += _packet.head.size;
//			break;
//		}
//	}
//	int result = Item->Receive(Token->GetSock(), static_cast<LPOVERLAPPED>(this));
//	return result;
//}

gzz::RecvItemEX::RecvItemEX(int id)
	: ICompletionItemEX(id), Socket(nullptr), Recv(nullptr), Queue(nullptr)
{
	Buf = new RingBuffer(MAX_RECV_BUF_SIZE * 2);
}

gzz::RecvItemEX::RecvItemEX(int id, TcpSocketEX* sock, IReceive* item, JobQueue<PACKAGE>* queue)
	: ICompletionItemEX(id), Socket(sock), Recv(item), Queue(queue)
{
	Buf = new RingBuffer(MAX_RECV_BUF_SIZE * 2);
}

gzz::RecvItemEX::~RecvItemEX()
{
	if (Buf)
	{
		delete Buf;
	}
}

void gzz::RecvItemEX::SettingItem(TcpSocketEX* sock, IReceive* item, JobQueue<PACKAGE>* queue)
{
	Socket = sock;
	SockId = Socket->GetId();
	Recv = item;
	Queue = queue;
}

void gzz::RecvItemEX::AddMessage(PACKAGE pack)
{
	Queue->Enqueue(pack);
}

int gzz::RecvItemEX::Process(const int completionBytes, LPOVERLAPPED other)
{
	if (completionBytes <= 0)
	{
		return 	Socket->Close(other);
	}

	int size = completionBytes;
	int readbytes = 0;
	bool IsPackage = true;
	char* buf = Recv->GetBuf();
	PACKET packet;
	unsigned short packet_size = 0;
	int ret = 0;

	while (size > 0)
	{
		 ret = Buf->Push(buf + ret, completionBytes);
		// 링버퍼의 최대치를 넘은 경우.
		if (ret > -1)
		{
			if (ret == 0)
			{
				// 링버퍼가 꽉차고 처리도 못했다는 판단, 링버퍼의 크기보다 패킷의 크기가 큰 경우 생길 가능성이 있다.
				// 현재는 Close 시킴.
				Socket->Close(other);
			}
			else
			{
				ret = Buf->Push(buf, ret);
				//if (ret > -1)
				//{
				//	Socket->Close(other);
				//}
			}
			size -= ret;
		}
		else
		{
			size = 0;
		}

		while (true)
		{
			if (Buf->CurrentSize() < PACKET_HEADER_SIZE)
			{
				break;
			}

			Buf->Get(reinterpret_cast<char*>(&packet_size), sizeof(unsigned short));

			if (Buf->CurrentSize() < packet_size)
			{
				break;
			}
			else
			{
				Buf->Pop(packet.memory, packet_size);
				PACKAGE pack(Socket->GetId(), packet);
				Queue->Enqueue(pack);
			}
		}
	}

	return Recv->Receive(Socket->GetSock(), static_cast<LPOVERLAPPED>(this));
	//ret = Buf->Push(buf, completionBytes);

	//// 링버퍼의 최대치를 넘은 경우.

	//if (ret > -1)
	//{
	//	if (ret == 0)
	//	{
	//		// 링버퍼가 꽉차고 처리도 못했다는 판단, 링버퍼의 크기보다 패킷의 크기가 큰 경우 생길 가능성이 있다.
	//		// 현재는 Close 시킴.
	//		Socket->Close(other);
	//	}
	//	else
	//	{
	//		// 링버퍼가 쌓여있고 처리를 다 하지 못한상태. 
	//		// 패킷의 크기가 링버퍼의 크기일수 도 있으므로 필요한 패킷만큼 합쳐보고 돌린다.
	//		ret = Buf->Push(buf, ret);

	//		if (ret > -1)
	//		{
	//			Socket->Close(other);
	//		}
	//		size -= ret;
	//	}
	//}

	//if (completionBytes < PACKET_HEADER_SIZE)
	//{
	//	int result = Recv->Receive(Socket->GetSock(), static_cast<LPOVERLAPPED>(this));
	//	return result;
	//}
	//else
	//{
	//	while (true)
	//	{
	//
	//	}

	//	if (size != 0)
	//	{

	//	}
	//}

	/*while (IsPackage)
	{
		if (size == 0)
		{
			break;
		}

		PACKET _packet = { 0, };
		_packet = Recv->GetPacket(size, readbytes);

		switch (static_cast<RECV_STATUS>(_packet.head.size))
		{
		case RECV_STATUS::OK:
			IsPackage = false;
			break;
		case RECV_STATUS::MORE:
			IsPackage = false;
			break;
		case RECV_STATUS::FAIL:
			IsPackage = false;
			Socket->ForceClose();
			return static_cast<int>(NET_ERR::RECV_PROCESS_FAIL_DATA);
		default:
			PACKAGE pack(Socket->GetId(), _packet);
			Queue->Enqueue(pack);
			size -= _packet.head.size;
			readbytes += _packet.head.size;
			break;
		}
	}
	int result = Recv->Receive(Socket->GetSock(), static_cast<LPOVERLAPPED>(this));
	return result;*/
}
