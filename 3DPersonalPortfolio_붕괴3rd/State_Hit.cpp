#include "stdafx.h" 
#include "State_Hit.h"
#include "Theresa.h"
#include "Transform.h"
#include "Animation.h"
#include "GameInstance.h"
#include "State_Avoid.h"
#include "Navigation.h"
#include "Player.h"
#include "Kiana.h"
#include "Theresa.h"

CState_Hit::CState_Hit(CHARACTER eCharactorType)
	:CState(eCharactorType)
{
}

HRESULT CState_Hit::Initialize_ProtoType()
{
	return S_OK;
}

void CState_Hit::Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	TimaAcc += TimeDelta;

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

void CState_Hit::Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CKiana* pKiana = static_cast<CKiana*>(Player);

	if (0.2 < TimaAcc && CGameInstance::GetInstance()->DIKeyPress(DIK_SPACE))
	{
		pKiana->Set_State(CKiana::STATE_EVADE);
		dynamic_cast<CState_Avoid*>(pKiana->Get_StateCalss(CKiana::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		TimaAcc = 0.0;
		return;
	}

	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		pKiana->Set_State(CKiana::STATE_IDLE);
		pModel->Set_Animation(CKiana::ANIM_IDLE);
		TimaAcc = 0.0;
	}
}

void CState_Hit::Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CTheresa* pTheresa = static_cast<CTheresa*>(Player);

	if (0.2 < TimaAcc && CGameInstance::GetInstance()->DIKeyPress(DIK_SPACE))
	{
		pTheresa->Set_State(CTheresa::STATE_EVADE);
		dynamic_cast<CState_Avoid*>(pTheresa->Get_StateCalss(CTheresa::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		TimaAcc = 0.0;
		return;
	}

	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		pTheresa->Set_State(CTheresa::STATE_IDLE);
		pModel->Set_Animation(CTheresa::ANIM_IDLE);
		TimaAcc = 0.0;
	}
}

CState_Hit* CState_Hit::Create(CHARACTER eCharactorType)
{
	CState_Hit* pInstance = new CState_Hit(eCharactorType);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Failed to Created : CState_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Hit::Free(void)
{
	__super::Free();
}

