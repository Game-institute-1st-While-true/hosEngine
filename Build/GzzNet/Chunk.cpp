#include "Chunk.h"

gzz::Chunk::Chunk()
	:Position(0), Offset(0), Buf{ 0, }
{
}

gzz::Chunk::~Chunk()
{
}

gzz::Chunk& gzz::Chunk::AddChunk(Chunk& ch)
{
	char* buf = ch.GetBuf();
	int size = ch.Size();
	if (CheckWrite(size) == false)
	{
		return *this;
	}
	memcpy_s(Buf + Offset, MAX_STREAM_SIZE - Offset, buf, size);
	Offset += size;
	return *this;
}
