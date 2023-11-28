#include "stdafx.h" 
#include "State_Ultimate.h"
#include "Theresa.h"
#include "Kiana.h"
#include "GameInstance.h"
#include "Player.h"
#include "GameObject.h"
#include "State_Avoid.h"
#include "Model.h"
#include "Animation.h"
#include "Navigation.h"
#include "Theresa_Attack_Ultimate.h"
#include <Effect_Thorn.h>
#include <StaticCamera.h>
#include <Effect_Circle.h>
#include <Particle_Default.h>

CState_Ultimate::CState_Ultimate(CHARACTER eCharactorType)
	:CState(eCharactorType)
{
}

HRESULT CState_Ultimate::Initialize_ProtoType()
{
	return S_OK;
}

void CState_Ultimate::Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
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

	case Client::CState::CHARACTER_END:
		break;

	default:
		break;
	}
}

void CState_Ultimate::Kiana_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_dAttackTimer += TimeDelta;

	CKiana* pKiana = static_cast<CKiana*>(Player);			

	if (true == m_bCameraSwitch)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_ULTIMATE_START);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_Ultimate.ogg"), CSoundMgr::KIANA_ULTIMATE_START, 0.15f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_ULTIMATE_BGM);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_Ultimate_Back.ogg"), CSoundMgr::KIANA_ULTIMATE_BGM, 0.1f);		

		CGameInstance::GetInstance()->Start_Radial(0.5, 7, 0.015f);

		CStaticCamera* pCamera = static_cast<CStaticCamera*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_StaticCamera")));
		pCamera->Zoom_On(0.1, 0.25, 2.f);

		m_bCameraSwitch = false;
	}

	if (0.35 < m_dAttackTimer && true == m_bAttackSwitch)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_SKILL_FIRE);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_Ultimate_BurstSound.ogg"), CSoundMgr::KIANA_SKILL_FIRE, 0.1f);

		CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_ValkyrieBurst")
			, &pTransform->Get_State(CTransform::STATE_POSITION));

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
		pKiana->Ultimate_On();
		pKiana->Set_State(CKiana::STATE_IDLE);
		pModel->Set_Animation(CKiana::ANIM_IDLE);
		Ultimate_Clear();
	}
}

void CState_Ultimate::Theresa_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_dAttackTimer += TimeDelta;

	if (0.15 < m_dAttackTimer && true == m_bEffectSwitch)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_ULTIMATE_DIALOGUE);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_Ultimate_Start_Dialogue.ogg"), CSoundMgr::THERESA_ULTIMATE_DIALOGUE, 0.15f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_ULTIMATE_START);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_Ultimate_Start_ES.ogg"), CSoundMgr::THERESA_ULTIMATE_START, 0.1f);

		CGameInstance::GetInstance()->Start_Radial(0.5, 7, 0.015f);

		CEffect_Circle::EFFECTDESC EffectDesc;
		ZeroMemory(&EffectDesc, sizeof CEffect_Circle::EFFECTDESC);
		EffectDesc.vPos = pTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f);
		EffectDesc.fDestSize = 15.5f;
		EffectDesc.fSizeY = 4.f;
		EffectDesc.fdecreaseSpeed = 0.999f;
		EffectDesc.fSizeAcc = 4.8f;

		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"),
			TEXT("Prototype_GameObject_Effect_Circle"), &EffectDesc)))
			return;

		CParticle_Default::PARTICLEDESC ParticleDesc;
		ParticleDesc.fParticleSize = 0.1f;
		ParticleDesc.MaxSpeed = 10.f;
		ParticleDesc.MinSpeed = 7.f;
		ParticleDesc.vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);
		ParticleDesc.vPos = pTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f);

		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"),
			TEXT("Prototype_GameObject_Particle_Default"), &ParticleDesc)))
			return;

		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"),
			TEXT("Prototype_GameObject_Particle_Default"), &ParticleDesc)))
			return;

		static_cast<CTheresa*>(Player)->Set_Tick_UltimateSpear();

		m_bEffectSwitch = false;
	}

	CTheresa* pTheresa = static_cast<CTheresa*>(Player);

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_SPACE))
	{
		pTheresa->Set_State(CTheresa::STATE_EVADE);
		dynamic_cast<CState_Avoid*>(pTheresa->Get_StateCalss(CTheresa::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		return;
	}

	if (1.23 < m_dAttackTimer && true == m_bEffectSwitch2)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_ULTIMATE_THROW);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_Ultimate_Throw_ES.ogg"), CSoundMgr::THERESA_ULTIMATE_THROW, 0.1f);
				
		static_cast<CTheresa*>(Player)->Set_StopTick_UltimateSpear();

		CEffect_Thorn::EFFECTDESC EffectDesc;
		ZeroMemory(&EffectDesc, sizeof CEffect_Thorn::EFFECTDESC);
		EffectDesc.matParent = pTransform->Get_WorldMatrix();
		EffectDesc.Scale = _float3(4.5f, 4.5f, 1.5f);

		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect")
			, TEXT("Prototype_GameObject_Effect_Thorn"), &EffectDesc)))
			return;

		EffectDesc.Scale = _float3(2.5f, 2.5f, 1.f);

		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect")
			, TEXT("Prototype_GameObject_Effect_Thorn"), &EffectDesc)))
			return;

		m_bEffectSwitch2 = false;
	}

	if (1.4 < m_dAttackTimer)
	{
		if (m_bAttackSwitch)
		{
			CTheresa_Attack_Ultimate::ULTIMATEDESC UltimateDesc;
			ZeroMemory(&UltimateDesc, sizeof CTheresa_Attack_Ultimate::ULTIMATEDESC);			
			UltimateDesc.iAttackCount = 9;
			UltimateDesc.matPlayerWorld = pTransform->Get_WorldMatrix();

			for (_int i = 0; i < 9; ++i)
			{
				CGameInstance* pGameInstance = CGameInstance::GetInstance();
				Safe_AddRef(pGameInstance);

				UltimateDesc.dDelay = (_double)GetRandomFloat(0.f, 0.5f);
				XMStoreFloat3(&UltimateDesc.vLocalPos, XMVectorSet(_float(-6.f + (1.5f * i)), 6.f, 2.f, 0.f));

				pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Skill_Ultimate")
					, &UltimateDesc);

				Safe_Release(pGameInstance);

			}

			m_bAttackSwitch = false;
			m_dAttackTimer = 0.0;
		}		
	}

	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		pTheresa->Set_State(CTheresa::STATE_IDLE);
		pModel->Set_Animation(CTheresa::ANIM_IDLE);
		Ultimate_Clear();
	}
}

void CState_Ultimate::Ultimate_Clear()
{
	m_bAttackSwitch = true;
	m_bCameraSwitch = true;
	m_bEffectSwitch = true;
	m_bEffectSwitch2 = true;
	m_dAttackTimer = 0.0;
}

CState_Ultimate* CState_Ultimate::Create(CHARACTER eCharactorType)
{
	CState_Ultimate* pInstance = new CState_Ultimate(eCharactorType);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Failed to Created : CState_Ultimate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Ultimate::Free(void)
{
	__super::Free();
}

