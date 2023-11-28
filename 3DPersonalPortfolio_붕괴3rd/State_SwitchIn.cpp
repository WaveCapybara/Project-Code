#include "stdafx.h" 
#include "State_SwitchIn.h"
#include "Theresa.h"
#include "Kiana.h"
#include "Kiana.h"
#include "GameInstance.h"
#include "Player.h"
#include "GameObject.h"
#include "State_Avoid.h"
#include "Model.h"
#include "Navigation.h"
#include "Animation.h"

CState_SwitchIn::CState_SwitchIn(CHARACTER eCharactorType)
	:CState(eCharactorType)
{
}

HRESULT CState_SwitchIn::Initialize_ProtoType()
{
	return S_OK;
}

void CState_SwitchIn::Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
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

void CState_SwitchIn::Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	if (true == m_bSoundSwitch)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_SWITCHIN);

		_uint i = (_uint(GetRandomFloat(0.f, 1000.f)) % 2);

		switch (i)
		{
		case 0:
			CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_SwitchIn2.ogg"), CSoundMgr::KIANA_SWITCHIN, 0.1f);
			break;

		case 1:
			CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_SwitchIn1.ogg"), CSoundMgr::KIANA_SWITCHIN, 0.1f);	
			break;

		default:
			break;
		}

		m_bSoundSwitch = false;
	}

	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		SwitchIn_Clear();
		static_cast<CKiana*>(Player)->Set_State(CKiana::STATE_IDLE);
		pModel->Set_Animation_NonLerp(CKiana::ANIM_IDLE);
		return;
	}

}

void CState_SwitchIn::Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	if (true == m_bSoundSwitch)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_SWITCHIN);

		_uint i = (_uint(GetRandomFloat(0.f, 1000.f)) % 2);

		switch (i)
		{
		case 0:
			CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_SwitchIn1.ogg"), CSoundMgr::THERESA_SWITCHIN, 0.1f);
			break;

		case 1:
			CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_SwitchIn2.ogg"), CSoundMgr::THERESA_SWITCHIN, 0.1f);
			break;

		default:
			break;
		}

		m_bSoundSwitch = false;
	}
	
	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		SwitchIn_Clear();
		static_cast<CTheresa*>(Player)->Set_State(CTheresa::STATE_IDLE);
		pModel->Set_Animation_NonLerp(CTheresa::ANIM_IDLE);
		return;
	}
}

void CState_SwitchIn::SwitchIn_Clear()
{
	m_bSoundSwitch = true;
}

CState_SwitchIn* CState_SwitchIn::Create(CHARACTER eCharactorType)
{
	CState_SwitchIn* pInstance = new CState_SwitchIn(eCharactorType);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Failed to Created : CState_SwitchIn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_SwitchIn::Free(void)
{
	__super::Free();
}

