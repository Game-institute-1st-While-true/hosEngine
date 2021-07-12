
// 자기 헤더
#include "BoneBuffer.h"

// 네임스페이스
using namespace DirectX::SimpleMath;


hos::cg::BoneBuffer::BoneBuffer()
{

}

hos::cg::BoneBuffer::~BoneBuffer()
{
}

void hos::cg::BoneBuffer::SetBoneTM(Matrix* matrixArray, unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
	{
		this->BoneTM[i] = matrixArray[i];
	}
}
