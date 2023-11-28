#include "stdafx.h" 
#include "State_Move.h"
#include "Theresa.h"
#include "Kiana.h"
#include "GameInstance.h"
#include "Player.h"
#include "Transform.h"
#include "StaticCamera.h"
#include "State_Avoid.h"
#include "Model.h"
#include "Navigation.h"
#include "Status.h"

CState_Move::CState_Move(CHARACTER eCharactorType)
	:CState(eCharactorType)
{	
}

HRESULT CState_Move::Initialize_ProtoType()
{
	return S_OK;
}

void CState_Move::Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	__super::Update_State(pTransform, TimeDelta);	

	switch (m_eCharacter)
	{
	case Client::CState::CHARACTOR_KIANA:
		Update_Kiana(Player, pModel, pTransform, pNavigation, TimeDelta);
		break;
	case Client::CState::CHARACTER_THERESA:
		Update_Theresa(Player, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case Client::CState::CHARACTER_END:		
		break;

	default:
		break;
	}	
	
}

void CState_Move::Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CTheresa* pTheresa = static_cast<CTheresa*>(Player);

	pTheresa->Check_Anim(CTheresa::ANIM_RUN);

	m_dSoundTimer += TimeDelta;

	if (0.31 < m_dSoundTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_WALK);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_Walk.ogg"), CSoundMgr::THERESA_WALK, 0.1f);

		m_dSoundTimer = 0.0;
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_W))
	{
		if (true == pTheresa->Check_CoolTime_Skill())
		{
			if (true == pTheresa->Get_Status()->Use_Skill(25.f))
			{
				pTheresa->Set_State(CTheresa::STATE_SKILL);
				pModel->Set_Animation(CTheresa::ANIM_SKILL);
				return;
			}
		}
	}

	else if (CGameInstance::GetInstance()->DIKeyDown(DIK_R))
	{
		if (true == pTheresa->Check_CoolTime_Ult())
		{
			if (true == pTheresa->Get_Status()->Use_Skill(100.f))
			{
				pTheresa->Set_State(CTheresa::STATE_ULTIMATE);
				pModel->Set_Animation_NonLerp(CTheresa::ANIM_ULTIMATE);
				return;
			}
		}
	}

	else if (CGameInstance::GetInstance()->DIKeyDown(DIK_A))
	{		
		pTheresa->Set_State(CTheresa::STATE_ATTACK);
		pModel->Set_Animation(CTheresa::ANIM_ATTACK_1);
		return;		
	}

	else if (CGameInstance::GetInstance()->DIKeyDown(DIK_SPACE))
	{
		pTheresa->Set_State(CTheresa::STATE_EVADE);
		static_cast<CState_Avoid*>(pTheresa->Get_StateCalss(CTheresa::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		return;
	}

	else if (CGameInstance::GetInstance()->DIKeyPress(DIK_UP) || CGameInstance::GetInstance()->DIKeyPress(DIK_RIGHT)
		|| CGameInstance::GetInstance()->DIKeyPress(DIK_DOWN) || CGameInstance::GetInstance()->DIKeyPress(DIK_LEFT))
		pTransform->Go_Straight(TimeDelta, pNavigation);

	else
	{
		pTheresa->Set_State(CTheresa::STATE_IDLE);
		pModel->Set_Animation(CTheresa::ANIM_IDLE);
	}
}

void CState_Move::Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_dSoundTimer += TimeDelta;

	CKiana* pKiana = static_cast<CKiana*>(Player);

	pKiana->Check_Anim(CKiana::ANIM_RUN);

	if (0.32 < m_dSoundTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_WALK);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_Walk.ogg"), CSoundMgr::KIANA_WALK, 0.1f);

		m_dSoundTimer = 0.0;
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_R))
	{
		if (true == pKiana->Check_CoolTime_Ult())
		{
			if (true == pKiana->Get_Status()->Use_Skill(100.f))
			{
				pKiana->Set_State(CKiana::STATE_ULTIMATE);
				pModel->Set_Animation_NonLerp(CKiana::ANIM_ULTIMATE);
				return;
			}
		}
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_A))
	{
		pKiana->Set_State(CKiana::STATE_ATTACK);
		pModel->Set_Animation(CKiana::ANIM_ATTACK_1);
		return;
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_W))
	{
		if (true == pKiana->Check_CoolTime_Skill())
		{
			if (true == pKiana->Get_Status()->Use_Skill(25.f))
			{
				pKiana->Set_State(CKiana::STATE_SKILL);
				pModel->Set_Animation_NonLerp(CKiana::ANIM_SKILL);
				return;
			}
		}
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_SPACE))
	{
		pKiana->Set_State(CKiana::STATE_EVADE);
		static_cast<CState_Avoid*>(pKiana->Get_StateCalss(CKiana::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		return;
	}

	if (CGameInstance::GetInstance()->DIKeyPress(DIK_UP) || CGameInstance::GetInstance()->DIKeyPress(DIK_RIGHT)
		|| CGameInstance::GetInstance()->DIKeyPress(DIK_DOWN) || CGameInstance::GetInstance()->DIKeyPress(DIK_LEFT))
		pTransform->Go_Straight(TimeDelta, pNavigation);

	else
	{
		pKiana->Set_State(CKiana::STATE_IDLE);
		pModel->Set_Animation(CKiana::ANIM_IDLE);
	}
}

CState_Move* CState_Move::Create(CHARACTER eCharactorType)
{
	CState_Move* pInstance = new CState_Move(eCharactorType);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Failed to Created : CState_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Move::Free(void)
{
	__super::Free();
}
