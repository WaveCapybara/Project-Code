#include "stdafx.h" 
#include "State_Skill.h"
#include "Theresa.h"
#include "Kiana.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Player.h"
#include "State_Avoid.h"
#include "Model.h"
#include "Animation.h"
#include "Navigation.h"
#include "Theresa_Attack_Skill.h"
#include "Effect_Kiana_GunFire.h"
#include "Kiana_Attack_Skill.h"

CState_Skill::CState_Skill(CHARACTER eCharactorType)
	:CState(eCharactorType)
{
}

HRESULT CState_Skill::Initialize_ProtoType()
{
	return S_OK;
}

void CState_Skill::Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	__super::Update_State(pTransform, TimeDelta);

	switch (m_eCharacter)
	{
	case Client::CState::CHARACTOR_KIANA:
		Kiana_Update(Player, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case Client::CState::CHARACTER_THERESA:
		Theresa_Update(Player, pModel, pTransform, pNavigation, TimeDelta);
		break;	

	default:
		break;
	}
}

void CState_Skill::Kiana_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CKiana* pKiana = static_cast<CKiana*>(Player);			

	m_dAttackTimer += TimeDelta;

	if (0.5 < m_dAttackTimer && true == m_bAttackSwitch)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_SKILL_FIRE);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("AUDIO_Default_4067.ogg"), CSoundMgr::KIANA_SKILL_FIRE, 0.1f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_SKILL_DIALOGUE);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("AUDIO_Default_4080.ogg"), CSoundMgr::KIANA_SKILL_DIALOGUE, 0.1f);

		CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_GunFire")
			, &pTransform->Get_WorldMatrix());

		CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Skill")
			, &pTransform->Get_WorldMatrix());

		m_bAttackSwitch = false;
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_SPACE))
	{
		pKiana->Set_State(CKiana::STATE_EVADE);
		dynamic_cast<CState_Avoid*>(pKiana->Get_StateCalss(CKiana::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		return;
	}		

	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		pKiana->Set_State(CKiana::STATE_IDLE);
		pModel->Set_Animation(CKiana::ANIM_IDLE);
		Skill_Clear();
	}
}

void CState_Skill::Theresa_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CTheresa* pTheresa = static_cast<CTheresa*>(Player);

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_SPACE))
	{
		pTheresa->Set_State(CTheresa::STATE_EVADE);
		dynamic_cast<CState_Avoid*>(pTheresa->Get_StateCalss(CTheresa::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		return;
	}

	if (true == m_bAttackSwitch && 5.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CTheresa_Attack_Skill::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CTheresa_Attack_Skill::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Attack_Skill")
			, &AttackDesc);

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		pTheresa->Set_State(CTheresa::STATE_IDLE);
		pModel->Set_Animation(CTheresa::ANIM_IDLE);
		Skill_Clear();
	}
}

CState_Skill* CState_Skill::Create(CHARACTER eCharactorType)
{
	CState_Skill* pInstance = new CState_Skill(eCharactorType);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Failed to Created : CState_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Skill::Free(void)
{
	__super::Free();
}

