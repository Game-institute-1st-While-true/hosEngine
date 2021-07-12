// <Concept>
// Source Name		: Ang3DEngine.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.03.15

// <Explanation>
// Graphics Engine ���̺귯������ �ܺη� �����ϰԵ� ����Դϴ�.
// warning : ���̺귯���� ���ο� �۾��� �� ��� ��� ����� �̰��� ���Խ��� ���ƾ� �մϴ�.

// Working Log
// 2021.03.15.Hosik	: ����

// Header List
#include "Renderer.h"
#include "RendererDesc.h"
#include "VertexDataType.h"
#include "RenderQue.h"

#ifndef _3DENGINE_H
#define _3DENGINE_H

#if defined(_DEBUG)||defined(DEBUG)
#if _WIN64
#pragma comment(lib,"Ang3DEngine_x64D")
#else
#pragma comment(lib,"Ang3DEngine_x86D")
#endif // _WIN64
#else
#if _WIN64
#pragma comment(lib,"Ang3DEngine_x64R")
#else
#pragma comment(lib,"Ang3DEngine_x86R")
#endif // _WIN64
#endif // defined(_DEBUG)||defined(DEBUG)


#endif // !_3DENGINE_H