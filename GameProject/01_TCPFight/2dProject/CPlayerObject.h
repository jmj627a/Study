#pragma once
#include <Windows.h>
#include "CBaseObject.h"

class CPlayerObject : public CBaseObject
{
private:
	bool m_bPlayerCharacter;
	int m_chHP;
	DWORD m_dwActionCur;
	DWORD m_dwActionOld;
	int m_iDirCur;	//위 아래는 얼굴 방향을 알아야함. 위로가는데 왼쪽보는지 그런거
	int m_iDirOld;

public:
	CPlayerObject();
	~CPlayerObject();

	void ActionProc();
	int GetDirection();
	int GetHP();
	void InputActionProc();
	bool IsPlayer();

	void Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);
	DWORD Run();

	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();

	void SetActionMove();
	void SetActionStand();
	
	void SetDirection();
	void SetHP();
};

