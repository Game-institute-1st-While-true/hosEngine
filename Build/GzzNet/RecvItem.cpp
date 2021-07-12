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
		// �������� �ִ�ġ�� ���� ���.
		if (ret > -1)
		{
			if (ret == 0)
			{
				// �����۰� ������ ó���� ���ߴٴ� �Ǵ�, �������� ũ�⺸�� ��Ŷ�� ũ�Ⱑ ū ��� ���� ���ɼ��� �ִ�.
				// ����� Close ��Ŵ.
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

	//// �������� �ִ�ġ�� ���� ���.

	//if (ret > -1)
	//{
	//	if (ret == 0)
	//	{
	//		// �����۰� ������ ó���� ���ߴٴ� �Ǵ�, �������� ũ�⺸�� ��Ŷ�� ũ�Ⱑ ū ��� ���� ���ɼ��� �ִ�.
	//		// ����� Close ��Ŵ.
	//		Socket->Close(other);
	//	}
	//	else
	//	{
	//		// �����۰� �׿��ְ� ó���� �� ���� ���ѻ���. 
	//		// ��Ŷ�� ũ�Ⱑ �������� ũ���ϼ� �� �����Ƿ� �ʿ��� ��Ŷ��ŭ ���ĺ��� ������.
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
