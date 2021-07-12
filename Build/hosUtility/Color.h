#ifndef _COLOR_H
#define _COLOR_H

// [2020/12/31 ������]
// ���� ǥ���ϴ� Ŭ����
// �������� �÷� Ÿ���� ������ ������ �� ColorŬ����
// �Ϻη� ut���ӽ����̽��� ���������ʾҴ�.

// LOG
// [2020/12/31] ������ : SimpleMath�� Color�� �״�� ��Ӹ� �Ѵ�.
// [2021/03/24] ������ : using ���� ����

#include "BaseTypes.h"
#include "DXTK\DirectXTK.h"

namespace hos
{
	using Color = DirectX::SimpleMath::Color;
	/*class Color :public DirectX::SimpleMath::Color
	{
	public:
		Color(F32 r, F32 g, F32 b, F32 a = 1.f);
		~Color();
	};*/
}

#endif // !_COLOR_H