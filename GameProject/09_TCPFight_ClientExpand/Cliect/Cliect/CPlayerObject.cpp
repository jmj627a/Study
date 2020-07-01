#pragma once
#include "stdafx.h"
#include "CPlayerObject.h"
#include "CSpriteDib.h"
#include "CNetwork.h"
#include "CMap.h"
extern CSpriteDib g_cSprite;
extern CPlayerObject* g_pPlayerObject;
extern CNetwork *network;
extern CMap map;
void CPlayerObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	
	int startX = (m_iCurX - map.m_DrawX);
	int startY = (m_iCurY - map.m_DrawY);

	if (map.m_DrawX == dfRANGE_MOVE_RIGHT - dfSCREEN_WIDTH / 2)
		startX += dfSCREEN_WIDTH / 2;
	if (map.m_DrawY == dfRANGE_MOVE_BOTTOM - dfSCREEN_HEIGHT / 2)
		startY += dfSCREEN_HEIGHT / 2;

	if (startX < 0 || startX > dfSCREEN_WIDTH)
		return;

	if (startY < 0 || startY > dfSCREEN_HEIGHT)
		return;

	//�׸��� ��� 
	g_cSprite.DrawSprite50(eSHADOW, startX, startY + 3, bypDest, iDestWidth, iDestHeight, iDestPitch);

	//�÷��̾� ��ü ���
	if (m_iObjectID == g_pPlayerObject->GetObjectID())
		g_cSprite.DrawMySprite(GetSprite(), startX, startY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	else
		g_cSprite.DrawSprite(GetSprite(), startX, startY, bypDest, iDestWidth, iDestHeight, iDestPitch);

	//hp ������ �� ��� 
	g_cSprite.DrawSprite(eGUAGE_HP, startX - 30, startY + 10, bypDest, iDestWidth, iDestHeight, iDestPitch, GetHP());
	
	
}

//action
DWORD CPlayerObject::Run()
{
	//���� ���������� �ִϸ��̼�
	NextFrame(); //�����Ѿ�� �����Ѿ�� ������ �Ϻη� ����
	ActionProc();

	return false;
}

void CPlayerObject::SetActionAttack1()
{
	m_dwActionOld = m_dwActionCur; 
	m_dwActionCur = dfACTION_ATTACK1;

	if (m_dwActionOld == dfACTION_ATTACK1)
		return;

	if (m_iDirCur == dfACTION_MOVE_LL)
		SetSprite(ePLAYER_ATTACK1_L01, ePLAYER_ATTACK1_L_MAX, dfDELAY_ATTACK1);
	else
		SetSprite(ePLAYER_ATTACK1_R01, ePLAYER_ATTACK1_R_MAX, dfDELAY_ATTACK1);

	//���߰� ����
	if (IsPlayer() && m_dwActionOld != m_dwActionCur && m_dwActionOld != dfACTION_STAND)
	{
		st_PACKET_HEADER Header;
		//stPACKET_CS_MOVE_STOP Packet;
		CPacket Packet;
	
		network->mpMoveStop(&Header, &Packet, GetDirection(), GetCurX(), GetCurY());
		network->SendPacket(&Header, &Packet);
	}

	//sendPacket
	//������ �ൿ�� �ٲ������ ��Ŷ ������
	if (IsPlayer() && m_dwActionOld != m_dwActionCur)
	{
		st_PACKET_HEADER Header;
		//stPACKET_CS_ATTACK1 Packet;
		CPacket Packet;
	
		network->mpAttack1(&Header, &Packet, GetDirection(), GetCurX(), GetCurY());
		network->SendPacket(&Header, &Packet);
	}
}

void CPlayerObject::SetActionAttack2()
{
	m_dwActionOld = m_dwActionCur; 
	m_dwActionCur = dfACTION_ATTACK2;

	if (m_dwActionOld == dfACTION_ATTACK2)
		return;

	if (m_iDirCur == dfACTION_MOVE_LL)
		SetSprite(ePLAYER_ATTACK2_L01, ePLAYER_ATTACK2_L_MAX, dfDELAY_ATTACK2);
	else
		SetSprite(ePLAYER_ATTACK2_R01, ePLAYER_ATTACK2_R_MAX, dfDELAY_ATTACK2);

	//���߰� ����
	if (IsPlayer() && m_dwActionOld != m_dwActionCur && m_dwActionOld != dfACTION_STAND)
	{
		st_PACKET_HEADER Header;
		//stPACKET_CS_MOVE_STOP Packet;
		CPacket Packet;

		network->mpMoveStop(&Header, &Packet, GetDirection(), GetCurX(), GetCurY());
		network->SendPacket(&Header, &Packet);
	}

	//sendPacket
	//������ �ൿ�� �ٲ������ ��Ŷ ������
	if (IsPlayer() && m_dwActionOld != m_dwActionCur)
	{
		st_PACKET_HEADER Header;
		//stPACKET_CS_ATTACK2 Packet;
		CPacket Packet;

		network->mpAttack2(&Header, &Packet, GetDirection(), GetCurX(), GetCurY());
		network->SendPacket(&Header, &Packet);
	}
}

void CPlayerObject::SetActionAttack3()
{
	m_dwActionOld = m_dwActionCur; 
	m_dwActionCur = dfACTION_ATTACK3;

	if (m_dwActionOld == dfACTION_ATTACK3)
		return;

	if (m_iDirCur == dfACTION_MOVE_LL)
		SetSprite(ePLAYER_ATTACK3_L01, ePLAYER_ATTACK3_L_MAX, dfDELAY_ATTACK3);
	else
		SetSprite(ePLAYER_ATTACK3_R01, ePLAYER_ATTACK3_R_MAX, dfDELAY_ATTACK3);

	//���߰� ����
	if (IsPlayer() && m_dwActionOld != m_dwActionCur && m_dwActionOld != dfACTION_STAND)
	{
		st_PACKET_HEADER Header;
		//stPACKET_CS_MOVE_STOP Packet;
		CPacket Packet;

		network->mpMoveStop(&Header, &Packet, GetDirection(), GetCurX(), GetCurY());
		network->SendPacket(&Header, &Packet);
	}

	//sendPacket
	//������ �ൿ�� �ٲ������ ��Ŷ ������
	if (IsPlayer() && m_dwActionOld != m_dwActionCur)
	{
		st_PACKET_HEADER Header;
		//stPACKET_CS_ATTACK3 Packet;
		CPacket Packet;

		network->mpAttack3(&Header, &Packet, GetDirection(), GetCurX(), GetCurY());
		network->SendPacket(&Header, &Packet);
	}
}

CPlayerObject::CPlayerObject(bool _isPlayerCharacter, int _dir)
{
	m_bPlayerCharacter = _isPlayerCharacter;
	m_chHP = 100;
	m_dwActionCur = dfACTION_STAND;
	m_dwActionOld = dfACTION_STAND;
	m_iDirCur = _dir;	//�� �Ʒ��� �� ������ �˾ƾ���. ���ΰ��µ� ���ʺ����� �׷���
	m_iDirOld = _dir;
}

CPlayerObject::~CPlayerObject()
{
}

void CPlayerObject::ActionProc()
{
	//��� ���� ��(����)�� ��� ���������� �ش� ���� ó���� �Ϸ��ؾ߸� �Ѵ�
	//�ٸ� �÷��̾��� �����̶�� ������ �ȳ���� �ݿ��ϱ� 
	if (g_pPlayerObject->GetObjectID() == GetObjectID())
	{
		switch (m_dwActionCur)
		{
			//���ݵ��� �Ǵ� ������ ������ �ִϸ��̼��� ���� ������ ���������� �ִϸ��̼��� �Ǿ�߸� �ϸ�
			//�ִϸ��̼��� ���� �� �⺻�������� �ڵ����� ���ư��� �Ѵ�.

		case dfACTION_ATTACK1:
		case dfACTION_ATTACK2:
		case dfACTION_ATTACK3:
			if (IsEndFrame())
			{
				//��������Ʈ �ٲ��ֱ�
				SetActionStand();
				//������ �������� �׼��� �ٲ� �������� ������ �� �� �� ������ �����ϵ���
				m_dwActionInput = dfACTION_STAND;
			}
			break;

		default:
			//�̿��� ��쿡�� ����� �Է� ó���� ���ش�
			InputActionProc();
			break;
		}
	}
	else
	{
		switch (m_dwActionCur)
		{
		//���ݵ��� �Ǵ� ������ ������ �ִϸ��̼��� ���� ������ ���������� �ִϸ��̼��� �Ǿ�߸� �ϸ�
		//�ִϸ��̼��� ���� �� �⺻�������� �ڵ����� ���ư��� �Ѵ�.

		case dfACTION_ATTACK1:
		case dfACTION_ATTACK2:
		case dfACTION_ATTACK3:
			if (IsEndFrame())
			{
				//������ �������� �׼��� �ٲ� �������� ������ �� �� �� ������ �����ϵ���
				m_dwActionInput = dfACTION_STAND;
			}
			break;
		}
		InputActionProc();

	}
}

int CPlayerObject::GetDirection()
{
	return m_iDirCur;
}

int CPlayerObject::GetCurAction()
{
	return m_dwActionCur;
}

char CPlayerObject::GetHP()
{
	return m_chHP;
}

void CPlayerObject::InputActionProc()
{
	//������Ʈ�� �׼��� �����°� 
	//m_dwActionOld = m_dwActionCur;

	//���������� �������� �� �޼����� ����. �ൿ�� ��ȭ����������

	switch (m_dwActionInput)
	{
	case dfACTION_MOVE_DD:
		SetActionMove(dfACTION_MOVE_DD);
		if(m_iCurY + 2 < dfRANGE_MOVE_BOTTOM)
			m_iCurY += 2;
		break;

	case dfACTION_MOVE_LD:
		SetDirection(dfACTION_MOVE_LL);
		SetActionMove(dfACTION_MOVE_LD);
		if(m_iCurX - 3 > dfRANGE_MOVE_LEFT)
			m_iCurX -= 3;
		if (m_iCurY + 2 < dfRANGE_MOVE_BOTTOM)
			m_iCurY += 2;
		break;

	case dfACTION_MOVE_LL:
		SetDirection(dfACTION_MOVE_LL);
		SetActionMove(dfACTION_MOVE_LL);
		if (m_iCurX - 3 > dfRANGE_MOVE_LEFT)
			m_iCurX -= 3;
		break;

	case dfACTION_MOVE_LU:
		SetDirection(dfACTION_MOVE_LL);
		SetActionMove(dfACTION_MOVE_LU);
		if (m_iCurX - 3 > dfRANGE_MOVE_LEFT)
			m_iCurX -= 3;
		if (m_iCurY - 2 > dfRANGE_MOVE_TOP)
			m_iCurY -= 2;
		break;

	case dfACTION_MOVE_RD:
		SetDirection(dfACTION_MOVE_RR);
		SetActionMove(dfACTION_MOVE_RD);
		if (m_iCurX + 3 < dfRANGE_MOVE_RIGHT)
			m_iCurX += 3;
		if (m_iCurY + 2 < dfRANGE_MOVE_BOTTOM)
			m_iCurY += 2;
		break;

	case dfACTION_MOVE_RR:
		SetDirection(dfACTION_MOVE_RR);
		SetActionMove(dfACTION_MOVE_RR);
		if (m_iCurX + 3 < dfRANGE_MOVE_RIGHT)
			m_iCurX += 3;
		break;

	case dfACTION_MOVE_RU:
		SetDirection(dfACTION_MOVE_RR);
		SetActionMove(dfACTION_MOVE_RU);
		if (m_iCurX + 3 < dfRANGE_MOVE_RIGHT)
			m_iCurX += 3;
		if (m_iCurY - 2 > dfRANGE_MOVE_TOP)
			m_iCurY -= 2;
		break;

	case dfACTION_MOVE_UU:
		SetActionMove(dfACTION_MOVE_UU);
		if (m_iCurY - 2 > dfRANGE_MOVE_TOP)
			m_iCurY -= 2;
		break;

	case dfACTION_STAND:
		SetActionStand();
		break;

	case dfACTION_ATTACK1:
		SetActionAttack1();
		break;

	case dfACTION_ATTACK2:
		SetActionAttack2();
		break;

	case dfACTION_ATTACK3:
		SetActionAttack3();
		break;
	}
}

bool CPlayerObject::IsPlayer()
{
	if (GetObjectID() == g_pPlayerObject->GetObjectID())
		return true;
	else
		return false;
}

void CPlayerObject::SetActionMove(int action)
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = action;

	switch (m_dwActionCur)
	{
	case dfACTION_MOVE_DD:
		//���� �ൿ�� ����Ÿ� �̵�
		if (m_dwActionOld == dfACTION_STAND)
		{
			if(m_iDirCur == dfACTION_MOVE_LL)
				SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
			else
				SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		}
		break;

	case dfACTION_MOVE_LD:
		//������ �ٸ��� ������ ��������Ʈ ó������
		if (m_iDirOld != dfACTION_MOVE_LL)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		//������ ���ٸ� �ȱ����� �ƴϾ��ٸ� ó������. �� �������� stand���ٸ� ó������ 
		if(m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		break;

	case dfACTION_MOVE_LL:
		if (m_iDirOld != dfACTION_MOVE_LL)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		break;

	case dfACTION_MOVE_LU:
		if (m_iDirOld != dfACTION_MOVE_LL)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE); 
		break;

	case dfACTION_MOVE_RD:
		if(m_iDirOld != dfACTION_MOVE_RR)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		break;

	case dfACTION_MOVE_RR:
		if (m_iDirOld != dfACTION_MOVE_RR)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE); 
		break;

	case dfACTION_MOVE_RU:
		if (m_iDirOld != dfACTION_MOVE_RR)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE); 
		break;

	case dfACTION_MOVE_UU:
		if (m_dwActionOld == dfACTION_STAND)
		{
			if (m_iDirCur == dfACTION_MOVE_LL)
				SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
			else
				SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		}
		break;
	}

	//������ �ൿ�� �ٲ������ ��Ŷ ������
	if (IsPlayer() && m_dwActionOld != m_dwActionCur && m_dwActionCur != dfACTION_STAND)
	{
		st_PACKET_HEADER Header;
		//stPACKET_CS_MOVE_START Packet;
		CPacket Packet;

		network->mpMoveStart(&Header, &Packet, GetCurAction(), GetCurX(), GetCurY());
		network->SendPacket(&Header, &Packet);
	}

}

void CPlayerObject::SetActionStand()
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = dfACTION_STAND;

	if (m_dwActionOld == dfACTION_STAND)
		return;

	if (m_iDirCur == dfACTION_MOVE_LL)
		SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	else if (m_iDirCur == dfACTION_MOVE_RR)
		SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);


	if (IsPlayer() && m_dwActionOld != m_dwActionCur &&
		m_dwActionOld != dfACTION_ATTACK1 && m_dwActionOld != dfACTION_ATTACK2 && m_dwActionOld != dfACTION_ATTACK3)
	{
		st_PACKET_HEADER Header;
		//stPACKET_CS_MOVE_STOP Packet;
		CPacket Packet;

		network->mpMoveStop(&Header, &Packet, GetDirection(), GetCurX(), GetCurY());
		network->SendPacket(&Header, &Packet);
	}
}

void CPlayerObject::SetDirection(int _dir)
{
	m_iDirOld = m_iDirCur;
	m_iDirCur = _dir;

	//m_iDirOld = m_iDirCur;
	//
	////����
	//if (_dir == dfDIR_LEFT)
	//	m_iDirCur = dfDIR_LEFT;
	////������
	//else if (_dir == dfDIR_RIGHT)
	//	m_iDirCur = dfDIR_RIGHT;
}


void CPlayerObject::SetHP(char _hp)
{
	m_chHP = _hp;
}