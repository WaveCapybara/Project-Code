#include "stdafx.h" 

#include "GameInstance.h"
#include "State_Avoid.h"
#include "Animation.h"
#include "Player.h"
#include "Theresa.h"
#include "Kiana.h"
#include "Transform.h"
#include "Model.h"
#include "Navigation.h"




CState_Avoid::CState_Avoid(CHARACTER eCharactorType)
	:CState(eCharactorType)
{
}

HRESULT CState_Avoid::Initialize_ProtoType(CTransform* pTransform)
{	
	m_pPlayerTransform = pTransform;
	Safe_AddRef(m_pPlayerTransform);

	m_vDashDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	return S_OK;
}

void CState_Avoid::Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	//__super::Update_State(pTransform, TimeDelta);

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

void CState_Avoid::Set_BackDash(CModel* pModel, _fvector Dir)
{
	if (CState::CHARACTER_THERESA == m_eCharacter)
	{
		if (CGameInstance::GetInstance()->DIKeyPress(DIK_UP) || CGameInstance::GetInstance()->DIKeyPress(DIK_DOWN)
			|| CGameInstance::GetInstance()->DIKeyPress(DIK_LEFT) || CGameInstance::GetInstance()->DIKeyPress(DIK_RIGHT))
		{
			pModel->Set_Animation(CTheresa::ANIM_EVADE_FORWARD);
			Is_BackDash = false;
			m_vDashDir = Dir;
		}

		else
		{
			pModel->Set_Animation(CTheresa::ANIM_EVADE_BACKWARD);
			Is_BackDash = true;
			m_vDashDir = Dir;
		}
	}

	else
	{
		if (CGameInstance::GetInstance()->DIKeyPress(DIK_UP) || CGameInstance::GetInstance()->DIKeyPress(DIK_DOWN)
			|| CGameInstance::GetInstance()->DIKeyPress(DIK_LEFT) || CGameInstance::GetInstance()->DIKeyPress(DIK_RIGHT))
		{
			pModel->Set_Animation_NonLerp(CKiana::ANIM_EVADE_FORWARD);
			Is_BackDash = false;
			m_vDashDir = Dir;
		}

		else
		{
			pModel->Set_Animation_NonLerp(CKiana::ANIM_EVADE_BACKWARD);
			Is_BackDash = true;
			m_vDashDir = Dir;
		}
	}
}

void CState_Avoid::Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CKiana* pKiana = static_cast<CKiana*>(Player);

	//89프레임			

	if (35.0 >= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		if (true == m_bSoundSwitch)
		{
			CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_EVADE);
			CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_Evade_SE2.ogg"), CSoundMgr::KIANA_EVADE, 0.11f);

			m_bSoundSwitch = false;
		}

		if (true == Is_BackDash)
			m_pPlayerTransform->Dash(TimeDelta, m_vDashDir, -1.f, pNavigation);

		else
			m_pPlayerTransform->Dash(TimeDelta, m_vDashDir, 1.f, pNavigation);
	}

	else if (45.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{	
		if (CGameInstance::GetInstance()->DIKeyPress(DIK_A))
		{
			pKiana->Set_State(CKiana::STATE_ATTACK);
			pModel->Set_Animation(CKiana::ANIM_ATTACK_1);			
			Evade_Clear();
			return;
		}

		if (pKiana->Check_DirInput())
		{
			pKiana->Set_State(CKiana::STATE_RUN);
			pModel->Set_Animation_NonLerp(CKiana::ANIM_RUN);
			Evade_Clear();
			return;
		}
	}

	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		pKiana->Set_State(CKiana::STATE_IDLE);
		pModel->Set_Animation(CKiana::ANIM_IDLE);
		Evade_Clear();
	}
}

void CState_Avoid::Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CTheresa* pTheresa = static_cast<CTheresa*>(Player);

	//89프레임	

	if (30.0 >= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		if (true == m_bSoundSwitch)
		{
			CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_EVADE);
			CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_Evade.ogg"), CSoundMgr::THERESA_EVADE, 0.09f);

			m_bSoundSwitch = false;
		}

		if (true == Is_BackDash)
			m_pPlayerTransform->Dash(TimeDelta, m_vDashDir, -1.f, pNavigation);

		else
			m_pPlayerTransform->Dash(TimeDelta, m_vDashDir, 1.f, pNavigation);
	}

	else if (38.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		if (CGameInstance::GetInstance()->DIKeyPress(DIK_SPACE))
		{
			__super::Update_State(pTransform, TimeDelta);

			pTheresa->Set_State(CTheresa::STATE_EVADE);
			Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
			Evade_Clear();
			return;
		}

		else if (pTheresa->Check_DirInput())
		{
			pTheresa->Set_State(CTheresa::STATE_RUN);
			pModel->Set_Animation(CTheresa::ANIM_RUN);
			Evade_Clear();
			return;
		}
	}

	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		pTheresa->Set_State(CTheresa::STATE_IDLE);
		pModel->Set_Animation_NonLerp(CTheresa::ANIM_IDLE);
		Evade_Clear();
	}
}

void CState_Avoid::Evade_Clear()
{
	m_bSoundSwitch = true;
	m_dDashTimer = 0.0;
}

CState_Avoid* CState_Avoid::Create(CTransform* pTransform, CHARACTER eCharactorType)
{
	CState_Avoid* pInstance = new CState_Avoid(eCharactorType);

	if (FAILED(pInstance->Initialize_ProtoType(pTransform)))
	{
		MSG_BOX("Failed to Created : CState_Avoid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Avoid::Free(void)
{
	__super::Free();

	Safe_Release(m_pPlayerTransform);
}
