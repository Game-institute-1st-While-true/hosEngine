#pragma once

// ������ ��ũ�� ����
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(pBuff) if((pBuff)){ (pBuff)->Release(); (pBuff) = NULL; }
#define SAFE_DELETE(pBuff)	if((pBuff)){ delete (pBuff); (pBuff) = NULL; }
#define SAFE_DELARRY(pBuff) if((pBuff)){ delete [] (pBuff); (pBuff) = NULL; }
#endif

#define CELARMEMORY(pBuff) memset(pBuff, 0, sizeof(pBuff));

// ���� ����Ʈ ����
#define SAFE_DELETE_VECTOR(vList) for (int i = 0; i < vList.size(); i++){ SAFE_DELETE(vList[i]);} vList.clear(); vList.shrink_to_fit();
#define SAFE_CLEAR_VECTOR(vList) vList.clear(); vList.shrink_to_fit();
