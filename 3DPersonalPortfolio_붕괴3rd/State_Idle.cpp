#include "stdafx.h" 
#include "State_Idle.h"
#include "Theresa.h"
#include "Kiana.h"
#include "GameInstance.h"
#include "Player.h"
#include "GameObject.h"
#include "State_Avoid.h"
#include "Model.h"
#include "Navigation.h"
#include "Status.h"

CState_Idle::CState_Idle(CHARACTER eCharactorType)
	:CState(eCharactorType)
{
}

HRESULT CState_Idle::Initialize_ProtoType()
{
	return S_OK;
}

void CState_Idle::Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
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

	default:
		break;
	}	
}

void CState_Idle::Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CKiana* pKiana = static_cast<CKiana*>(Player);

	pKiana->Check_Anim(CKiana::ANIM_IDLE);

	if (pKiana->Check_DirInput())
	{
		pKiana->Set_State(CKiana::STATE_RUN);
		pModel->Set_Animation(CKiana::ANIM_RUN);
		return;
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_SPACE))
	{
		pKiana->Set_State(CKiana::STATE_EVADE);
		static_cast<CState_Avoid*>(pKiana->Get_StateCalss(CKiana::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		return;
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

	dynamic_cast<CKiana*>(Player)->Set_State(CKiana::STATE_IDLE);
}

void CState_Idle::Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CTheresa* pTheresa = static_cast<CTheresa*>(Player);

	pTheresa->Check_Anim(CTheresa::ANIM_IDLE);

	if (pTheresa->Check_DirInput())
	{
		pTheresa->Set_State(CTheresa::STATE_RUN);
		pModel->Set_Animation(CTheresa::ANIM_RUN);
		return;
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_SPACE))
	{
		pTheresa->Set_State(CTheresa::STATE_EVADE);
		static_cast<CState_Avoid*>(pTheresa->Get_StateCalss(CTheresa::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		return;
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

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_A))
	{
		pTheresa->Set_State(CTheresa::STATE_ATTACK);
		pModel->Set_Animation_NonLerp(CTheresa::ANIM_ATTACK_1);
		return;
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_R))
	{
		if (true == pTheresa->Check_CoolTime_Ult())
		{
			if (true == pTheresa->Get_Status()->Use_Skill(100.f))
			{
				pTheresa->Set_State(CTheresa::STATE_ULTIMATE);
				pModel->Set_Animation(CTheresa::ANIM_ULTIMATE);
				return;
			}
		}
	}

	//피격시 바로 피격상태로 전환

	pTheresa->Set_State(CTheresa::STATE_IDLE);
}

CState_Idle* CState_Idle::Create(CHARACTER eCharactorType)
{
	CState_Idle* pInstance = new CState_Idle(eCharactorType);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Failed to Created : CState_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle::Free(void)
{
	__super::Free();
}

