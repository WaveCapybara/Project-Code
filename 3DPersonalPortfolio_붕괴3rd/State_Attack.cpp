#include "stdafx.h" 
#include "GameInstance.h"
#include "State_Attack.h"
#include "State_Avoid.h"
#include "Player.h"
#include "Status.h"
#include "Theresa.h"
#include "Layer.h"
#include "Kiana.h"
#include "Model.h"
#include "Monster.h"
#include "StaticCamera.h"
#include "Animation.h"
#include "Navigation.h"
#include "Theresa_Attack_Normal.h"
#include "Kiana_Attack_Normal.h"
#include "Kiana_Attack_Ultimate.h"
#include "Effect_Kiana_NormalAttack.h"
#include "Effect_Theresa_NormalAttack.h"
#include "Effect_Kiana_Ultra.h"
#include "UI_MonsterHp.h"
#include "UI_LockOn.h"
#include "Effect_Dust.h"
#include <Theresa_Attack_Charge.h>
#include <Particle_Lightning.h>


CState_Attack::CState_Attack(CHARACTER eCharactorType)
	:CState(eCharactorType)
{
}

HRESULT CState_Attack::Initialize_ProtoType()
{
	return S_OK;
}

void CState_Attack::Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	//__super::Update_State(pTransform, TimeDelta);

	//87ÇÁ·¹ÀÓ

	if (true == m_bAimingSwitch)
	{
		Set_Target(pTransform);
		m_bAimingSwitch = false;
		m_bLerpSwitch = true;
		m_dLerpTimer = { 0.0 };
	}

	if (nullptr != m_pTargetPos)
	{
		vTargetPos = m_pTargetPos->Get_State(CTransform::STATE_POSITION);
		m_fTargetMinDistance = m_pTargetPos->Get_MinDistance();
	}

	if (true == m_bLerpSwitch)
		Aiming_Target(pTransform, TimeDelta);

	if (nullptr != m_pTargetPos)
	{
		CStaticCamera* pCamera = static_cast<CStaticCamera*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_StaticCamera")));
		pCamera->BattleCam_On(vTargetPos);
	}

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

void CState_Attack::Attack_Clear()
{
	m_CurCombo = ATTACK_1;
	m_eCurCombo_Kiana = KIANA_ATTACK_1;
	m_dAttackTimer = 0.0;
	m_dChargeTimer = 0.0;
	m_dEffectTimer = 0.0;
	m_fAcc = 2.f;
	m_fLowAcc = 10.f;
	m_fAccSpeed = 0.f;
	m_bAttackSwitch = true;
	m_bAttackSwitch2 = true;
	m_bAimingSwitch = true;

	Safe_Release(m_pTargetPos);
	m_pTargetPos = nullptr;

	CUI_MonsterHp* pMonsterUI = static_cast<CUI_MonsterHp*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_MonsterUI"), TEXT("Prototype_GameObject_UI_MonsterHp")));
	pMonsterUI->Target_Clear();

	CUI_LockOn* pLockOn = static_cast<CUI_LockOn*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_LockOn"), TEXT("Prototype_GameObject_UI_LockOn")));
	pLockOn->Target_Clear();

	CStaticCamera* pCamera = static_cast<CStaticCamera*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_StaticCamera")));
	pCamera->BattleCam_Off();
}

void CState_Attack::Theresa_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CTheresa* pTheresa = static_cast<CTheresa*>(Player);

	if (1.0 < m_dChargeTimer && m_CurCombo != ATTACK_CHARGE)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_CHARGE_START);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_ChargeAttack_Intrance_ES.ogg"), CSoundMgr::THERESA_CHARGE_START, 0.1f);
		
		pTheresa->Set_State(CTheresa::STATE_ATTACK);
		pModel->Set_Animation(CTheresa::ANIM_CHARGE_ATTACK);
		m_CurCombo = ATTACK_CHARGE;
		m_dChargeTimer = 0.0;
		m_dAttackTimer = 0.0;
		m_dEffectTimer = 0.0;
		m_bAttackSwitch = true;
		return;
	}

	if (CGameInstance::GetInstance()->DIKeyPress(DIK_SPACE))
	{
		pTheresa->Set_State(CTheresa::STATE_EVADE);
		dynamic_cast<CState_Avoid*>(pTheresa->Get_StateCalss(CTheresa::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		Attack_Clear();
		return;
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_R))
	{
		if (true == pTheresa->Check_CoolTime_Ult())
		{
			if (true == pTheresa->Get_Status()->Use_Skill(100.f))
			{
				pTheresa->Set_State(CTheresa::STATE_ULTIMATE);
				pModel->Set_Animation_NonLerp(CTheresa::ANIM_ULTIMATE);
				Attack_Clear();
				return;
			}
		}
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_W))
	{
		if (true == pTheresa->Check_CoolTime_Skill())
		{
			if (true == pTheresa->Get_Status()->Use_Skill(25.f))
			{
				pTheresa->Set_State(CTheresa::STATE_SKILL);
				pModel->Set_Animation_NonLerp(CTheresa::ANIM_SKILL);
				Attack_Clear();
				return;
			}
		}
	}

	if (CGameInstance::GetInstance()->DIKeyPress(DIK_A) && m_CurCombo != ATTACK_CHARGE)
	{
		m_dChargeTimer += TimeDelta;
	}	

	if (!CGameInstance::GetInstance()->DIKeyPress(DIK_A) && m_CurCombo != ATTACK_CHARGE)
	{
		m_dChargeTimer = 0.0;
	}

	switch (m_CurCombo)
	{
	case Client::CState_Attack::ATTACK_1:
		Theresa_Update_Attack_1(pTheresa, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case Client::CState_Attack::ATTACK_2:
		Theresa_Update_Attack_2(pTheresa, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case Client::CState_Attack::ATTACK_3:
		Theresa_Update_Attack_3(pTheresa, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case Client::CState_Attack::ATTACK_CHARGE:
		Theresa_Update_Attack_Charge(pTheresa, pModel, pTransform, pNavigation, TimeDelta);
		break;

	default:
		break;
	}

	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		pTheresa->Set_State(CTheresa::STATE_IDLE);
		pModel->Set_Animation_NonLerp(CTheresa::ANIM_IDLE);
		Attack_Clear();
		return;
	}
}

HRESULT CState_Attack::Theresa_Update_Attack_1(CTheresa* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_bLerpSwitch = true;

	m_dAttackTimer += TimeDelta;

	if (0.07 > m_dAttackTimer)
	{
		if (m_fTargetMinDistance * 2.f < XMVectorGetX(XMVector3Length(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			/*m_fAcc = (powf(m_fAcc, 2.f) * 0.5f);
			m_fAccSpeed += m_fAcc;*/
			pTransform->Go_SetDir(CTransform::STATE_LOOK, 15.f, TimeDelta);
		}
	}

	if (true == m_bAttackSwitch && 0.2 <= m_dAttackTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_ATTACK1);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_NormalAttack_Dialogue1.ogg"), CSoundMgr::THERESA_ATTACK1, 0.1f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_SWING);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_EvadeSound.ogg"), CSoundMgr::THERESA_SWING, 0.1f);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CTheresa_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CTheresa_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CTheresa_Attack_Normal::ATTACK_1;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Attack_Normal")
			, &AttackDesc);

		CEffect_Theresa_NormalAttack::ATTACKDESC EffectAttackDesc;
		ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_NormalAttack::ATTACKDESC);
		EffectAttackDesc.m_pPlayerTransform = pTransform;
		EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		EffectAttackDesc.AttackType = CEffect_Theresa_NormalAttack::ATTACK_1;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Theresa_NormalAttack")
			, &EffectAttackDesc);

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (0.4 <= m_dAttackTimer)
	{
		if (CGameInstance::GetInstance()->DIKeyDown(DIK_A))
		{
			Player->Set_State(CTheresa::STATE_ATTACK);
			pModel->Set_Animation_NonLerp(CTheresa::ANIM_ATTACK_2);
			m_CurCombo = ATTACK_2;
			m_bAttackSwitch = true;
			m_dAttackTimer = 0.0;
			return S_OK;
		}
	}

	if (0.7 <= m_dAttackTimer)
	{
		if (Player->Check_DirInput())
		{
			Player->Set_State(CTheresa::STATE_RUN);
			pModel->Set_Animation(CTheresa::ANIM_RUN);
			Attack_Clear();
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CState_Attack::Theresa_Update_Attack_2(CTheresa* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_bLerpSwitch = true;

	m_dAttackTimer += TimeDelta;

	if (0.07 > m_dAttackTimer)
	{
		if (m_fTargetMinDistance * 2.f < XMVectorGetX(XMVector3Length(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			/*m_fAcc = (powf(m_fAcc, 2.f) * 0.5f);
			m_fAccSpeed += m_fAcc;*/
			pTransform->Go_SetDir(CTransform::STATE_LOOK, 15.f, TimeDelta);
		}
	}

	if (true == m_bAttackSwitch && 0.2 <= m_dAttackTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_ATTACK2);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_NormalAttack_Dialogue2.ogg"), CSoundMgr::THERESA_ATTACK2, 0.1f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_SWING);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_EvadeSound.ogg"), CSoundMgr::THERESA_SWING, 0.1f);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CTheresa_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CTheresa_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CTheresa_Attack_Normal::ATTACK_2;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Attack_Normal")
			, &AttackDesc);

		CEffect_Theresa_NormalAttack::ATTACKDESC EffectAttackDesc;
		ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_NormalAttack::ATTACKDESC);
		EffectAttackDesc.m_pPlayerTransform = pTransform;
		EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		EffectAttackDesc.AttackType = CEffect_Theresa_NormalAttack::ATTACK_2;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Theresa_NormalAttack")
			, &EffectAttackDesc);

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (0.5 <= m_dAttackTimer)
	{
		if (CGameInstance::GetInstance()->DIKeyDown(DIK_A))
		{
			Player->Set_State(CTheresa::STATE_ATTACK);
			pModel->Set_Animation_NonLerp(CTheresa::ANIM_ATTACK_3);
			m_CurCombo = ATTACK_3;
			m_bAttackSwitch = true;
			m_dAttackTimer = 0.0;
			return S_OK;
		}
	}

	if (73.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		if (Player->Check_DirInput())
		{
			Player->Set_State(CTheresa::STATE_RUN);
			pModel->Set_Animation(CTheresa::ANIM_RUN);
			Attack_Clear();
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CState_Attack::Theresa_Update_Attack_3(CTheresa* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_bLerpSwitch = true;

	m_dAttackTimer += TimeDelta;

	if (0.15 < m_dAttackTimer && 0.45 > m_dAttackTimer)
	{
		if (m_fTargetMinDistance * 2.f < XMVectorGetX(XMVector3Length(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION))))
		{			
			pTransform->Go_SetDir(CTransform::STATE_LOOK, 7.f, TimeDelta);
		}
	}

	if (true == m_bAttackSwitch && 20.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CTheresa_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CTheresa_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CTheresa_Attack_Normal::ATTACK_3;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Attack_Normal")
			, &AttackDesc);

		CEffect_Theresa_NormalAttack::ATTACKDESC EffectAttackDesc;
		ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_NormalAttack::ATTACKDESC);
		EffectAttackDesc.m_pPlayerTransform = pTransform;
		EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		EffectAttackDesc.AttackType = CEffect_Theresa_NormalAttack::ATTACK_3;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Theresa_NormalAttack")
			, &EffectAttackDesc);

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (0.4 < m_dAttackTimer && true == m_bAttackSwitch2)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_ATTACK3);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_NormalAttack_Dialogue1.ogg"), CSoundMgr::THERESA_ATTACK3, 0.1f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_SWING);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_EvadeSound.ogg"), CSoundMgr::THERESA_SWING, 0.1f);

		m_bAttackSwitch2 = false;
	}

	if (80.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		if (Player->Check_DirInput())
		{
			Player->Set_State(CTheresa::STATE_RUN);
			pModel->Set_Animation(CTheresa::ANIM_RUN);
			Attack_Clear();
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CState_Attack::Theresa_Update_Attack_Charge(CTheresa* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_bLerpSwitch = true;

	m_dAttackTimer += TimeDelta;

	if (0.3 > m_dAttackTimer)
	{
		m_fAcc = (powf(m_fAcc, 2.f) * 0.5f);
		m_fLowAcc -= m_fAcc;
		pTransform->Go_SetDir(CTransform::STATE_LOOK, m_fLowAcc, TimeDelta);
	}

	if (0.8 < m_dAttackTimer && true == m_bAttackSwitch)
	{		
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::THERESA_CHARGE_DIALOGUE);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Theresa_ChargeAttack_Dialogue.ogg"), CSoundMgr::THERESA_CHARGE_DIALOGUE, 0.1f);

		CTheresa_Attack_Charge::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CTheresa_Attack_Charge::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();

		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"),
			TEXT("Prototype_GameObject_Theresa_Attack_Charge"), &AttackDesc)))
			return NO_EVENT;

		m_bAttackSwitch = false;
	}	

	if (0.8 < m_dAttackTimer && 1.3 > m_dAttackTimer)
	{
		m_dEffectTimer += TimeDelta;

		if (0.05 < m_dEffectTimer)
		{
			CParticle_Lightning::PARTICLEDESC ParticleDesc;
			ZeroMemory(&ParticleDesc, sizeof CParticle_Lightning::PARTICLEDESC);
			ParticleDesc.pBone = pModel->Get_Bone("Bip001 R Finger0");
			ParticleDesc.pParentTransform = pTransform;
			ParticleDesc.vColor = XMVectorSet(0.1f, 0.1f, 0.1f, 1.f);
			ParticleDesc.fParticleSize = 1.0f;

			if (FAILED(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Particle_Lightning"), &ParticleDesc)))
				return E_FAIL;

			m_dEffectTimer = 0;
		}
	}

	return S_OK;
}

void CState_Attack::Kiana_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	CKiana* pKiana = static_cast<CKiana*>(Player);
	
	if (CGameInstance::GetInstance()->DIKeyDown(DIK_R))
	{
		if (true == pKiana->Check_CoolTime_Ult())
		{
			if (true == pKiana->Get_Status()->Use_Skill(100.f))
			{
				static_cast<CKiana*>(Player)->Set_State(CKiana::STATE_ULTIMATE);
				pModel->Set_Animation_NonLerp(CKiana::ANIM_ULTIMATE);
				Attack_Clear();
				return;
			}
		}
	}

	if (CGameInstance::GetInstance()->DIKeyDown(DIK_W))
	{
		if (true == pKiana->Check_CoolTime_Skill())
		{
			if (true == pKiana->Get_Status()->Use_Skill(25.f))
			{
				pKiana->Set_State(CKiana::STATE_SKILL);
				pModel->Set_Animation_NonLerp(CKiana::ANIM_SKILL);
				Attack_Clear();
				return;
			}
		}
	}

	if (CGameInstance::GetInstance()->DIKeyPress(DIK_A) && m_eCurCombo_Kiana != KIANA_ATTACK_CHARGE_1)
	{
		m_dChargeTimer += TimeDelta;
	}

	if (!CGameInstance::GetInstance()->DIKeyPress(DIK_A) && m_eCurCombo_Kiana != KIANA_ATTACK_CHARGE_1)
	{
		m_dChargeTimer = 0.0;
	}

	if (0.6 < m_dChargeTimer && m_CurCombo != KIANA_ATTACK_CHARGE_1
		&& m_CurCombo != KIANA_ATTACK_CHARGE_2)
	{
		pKiana->Set_State(CKiana::STATE_ATTACK);
		pModel->Set_Animation(CKiana::ANIM_CHARGE_ATTACK_1);
		m_eCurCombo_Kiana = KIANA_ATTACK_CHARGE_1;
		m_bAttackSwitch = true;		
		m_dChargeTimer = 0.0;
		m_dAttackTimer = 0.0;
		return;
	}

	if (CGameInstance::GetInstance()->DIKeyPress(DIK_SPACE))
	{
		pKiana->Set_State(CKiana::STATE_EVADE);
		dynamic_cast<CState_Avoid*>(pKiana->Get_StateCalss(CKiana::STATE_EVADE))->Set_BackDash(pModel, pTransform->Get_State(CTransform::STATE_LOOK));
		Attack_Clear();
	}	

	switch (m_eCurCombo_Kiana)
	{
	case CState_Attack::ATTACKCOMBO_KIANA::KIANA_ATTACK_1:
		Kiana_Update_Attack_1(pKiana, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case CState_Attack::ATTACKCOMBO_KIANA::KIANA_ATTACK_2:
		Kiana_Update_Attack_2(pKiana, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case CState_Attack::ATTACKCOMBO_KIANA::KIANA_ATTACK_3:
		Kiana_Update_Attack_3(pKiana, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case CState_Attack::ATTACKCOMBO_KIANA::KIANA_ATTACK_4:
		Kiana_Update_Attack_4(pKiana, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case CState_Attack::ATTACKCOMBO_KIANA::KIANA_ATTACK_5:
		Kiana_Update_Attack_5(pKiana, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case CState_Attack::ATTACKCOMBO_KIANA::KIANA_ATTACK_CHARGE_1:
		Kiana_Update_Attack_Charge1(pKiana, pModel, pTransform, pNavigation, TimeDelta);
		break;

	case CState_Attack::ATTACKCOMBO_KIANA::KIANA_ATTACK_CHARGE_2:
		Kiana_Update_Attack_Charge2(pKiana, pModel, pTransform, pNavigation, TimeDelta);
		break;

	default:
		break;
	}

	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{
		pKiana->Set_State(CKiana::STATE_IDLE);
		pModel->Set_Animation_NonLerp(CKiana::ANIM_IDLE);
		Attack_Clear();
		return;
	}
}

HRESULT CState_Attack::Kiana_Update_Attack_1(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_dAttackTimer += TimeDelta;
	m_bLerpSwitch = true;

	if(0.2 < m_dAttackTimer && 0.45 > m_dAttackTimer)
	{		
		if (m_fTargetMinDistance < XMVectorGetX(XMVector3Length(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			m_fAcc = (powf(m_fAcc, 2.f) * 0.5f);
			m_fAccSpeed += m_fAcc;
			pTransform->Go_SetDir(CTransform::STATE_LOOK, m_fAccSpeed, TimeDelta);
		}
	}	

	if (true == m_bAttackSwitch && 0.45 <= m_dAttackTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_ATTACK1);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_NormalAttack_1.ogg"), CSoundMgr::KIANA_ATTACK1, 0.1f);		

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_SWING);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_Swing.ogg"), CSoundMgr::KIANA_SWING, 0.2f);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CKiana_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CKiana_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CKiana_Attack_Normal::ATTACK_1;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Normal")
			, &AttackDesc);

		CEffect_Kiana_NormalAttack::ATTACKDESC EffectAttackDesc;
		ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_NormalAttack::ATTACKDESC);
		EffectAttackDesc.m_pPlayerTransform = pTransform;
		EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		EffectAttackDesc.AttackType = CEffect_Kiana_NormalAttack::ATTACK_1;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_NormalAttack")
			, &EffectAttackDesc);

		if (true == Player->Check_UltimateOn())
		{
			CKiana_Attack_Ultimate::ATTACKDESC UlAttackDesc;
			ZeroMemory(&UlAttackDesc, sizeof CKiana_Attack_Ultimate::ATTACKDESC);
			UlAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
			UlAttackDesc.AttackType = CKiana_Attack_Ultimate::ATTACK_1;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Ultimate")
				, &UlAttackDesc);

			CEffect_Kiana_Ultra::ATTACKDESC EffectAttackDesc;
			ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_Ultra::ATTACKDESC);
			EffectAttackDesc.m_pPlayerTransform = pTransform;
			EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
			EffectAttackDesc.AttackType = CEffect_Kiana_Ultra::ATTACK_1;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_Ultra")
				, &EffectAttackDesc);
		}

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (0.55 <= m_dAttackTimer)
	{
		if (CGameInstance::GetInstance()->DIKeyDown(DIK_A))
		{
			Player->Set_State(CKiana::STATE_ATTACK);
			pModel->Set_Animation_NonLerp(CKiana::ANIM_ATTACK_2);
			m_eCurCombo_Kiana = KIANA_ATTACK_2;
			m_bAttackSwitch = true;
			m_dAttackTimer = 0.0;
			return S_OK;
		}
	}

	if (70.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		if (Player->Check_DirInput())
		{
			Player->Set_State(CKiana::STATE_RUN);
			pModel->Set_Animation(CKiana::ANIM_RUN);
			Attack_Clear();
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CState_Attack::Kiana_Update_Attack_2(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_bLerpSwitch = true;

	m_dAttackTimer += TimeDelta;

	if (0.11 > m_dAttackTimer)
	{
		if (m_fTargetMinDistance < XMVectorGetX(XMVector3Length(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			/*m_fAcc = (powf(m_fAcc, 2.f) * 0.5f);
			m_fAccSpeed += m_fAcc;*/
			pTransform->Go_SetDir(CTransform::STATE_LOOK, 15.f, TimeDelta);
		}
	}

	if (true == m_bAttackSwitch && 0.2 <= m_dAttackTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_ATTACK2);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_NormalAttack_2.ogg"), CSoundMgr::KIANA_ATTACK2, 0.1f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_GUNSOUND1);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_GunSound.ogg"), CSoundMgr::KIANA_GUNSOUND1, 0.05f);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CKiana_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CKiana_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CKiana_Attack_Normal::ATTACK_2;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Normal")
			, &AttackDesc);

		CEffect_Kiana_NormalAttack::ATTACKDESC EffectAttackDesc;
		ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_NormalAttack::ATTACKDESC);
		EffectAttackDesc.m_pPlayerTransform = pTransform;
		EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		EffectAttackDesc.AttackType = CEffect_Kiana_NormalAttack::ATTACK_2;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_NormalAttack")
			, &EffectAttackDesc);

		if (true == Player->Check_UltimateOn())
		{
			CKiana_Attack_Ultimate::ATTACKDESC UlAttackDesc;
			ZeroMemory(&UlAttackDesc, sizeof CKiana_Attack_Ultimate::ATTACKDESC);
			UlAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
			UlAttackDesc.AttackType = CKiana_Attack_Ultimate::ATTACK_2;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Ultimate")
				, &UlAttackDesc);

			CEffect_Kiana_Ultra::ATTACKDESC EffectAttackDesc;
			ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_Ultra::ATTACKDESC);
			EffectAttackDesc.m_pPlayerTransform = pTransform;
			EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
			EffectAttackDesc.AttackType = CEffect_Kiana_Ultra::ATTACK_2;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_Ultra")
				, &EffectAttackDesc);
		}

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (0.3 <= m_dAttackTimer)
	{
		if (CGameInstance::GetInstance()->DIKeyDown(DIK_A))
		{
			Player->Set_State(CKiana::STATE_ATTACK);
			pModel->Set_Animation_NonLerp(CKiana::ANIM_ATTACK_3);
			m_eCurCombo_Kiana = KIANA_ATTACK_3;
			m_bAttackSwitch = true;
			m_dAttackTimer = 0.0;
			return S_OK;
		}
	}	

	if (70.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		if (Player->Check_DirInput())
		{
			Player->Set_State(CKiana::STATE_RUN);
			pModel->Set_Animation_NonLerp(CKiana::ANIM_RUN);
			Attack_Clear();
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CState_Attack::Kiana_Update_Attack_3(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_bLerpSwitch = true;

	m_dAttackTimer += TimeDelta;

	if (0.09 > m_dAttackTimer)
	{
		if (m_fTargetMinDistance < XMVectorGetX(XMVector3Length(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			/*m_fAcc = (powf(m_fAcc, 2.f) * 0.5f);
			m_fAccSpeed += m_fAcc;*/
			pTransform->Go_SetDir(CTransform::STATE_LOOK, 15.f, TimeDelta);
		}
	}

	if (true == m_bAttackSwitch && 0.2 <= m_dAttackTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_ATTACK3);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_NormalAttack_3.ogg"), CSoundMgr::KIANA_ATTACK3, 0.1f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_GUNSOUND2);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_GunSound.ogg"), CSoundMgr::KIANA_GUNSOUND2, 0.05f);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CKiana_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CKiana_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CKiana_Attack_Normal::ATTACK_3;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Normal")
			, &AttackDesc);

		CEffect_Kiana_NormalAttack::ATTACKDESC EffectAttackDesc;
		ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_NormalAttack::ATTACKDESC);
		EffectAttackDesc.m_pPlayerTransform = pTransform;
		EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		EffectAttackDesc.AttackType = CEffect_Kiana_NormalAttack::ATTACK_3;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_NormalAttack")
			, &EffectAttackDesc);

		if (true == Player->Check_UltimateOn())
		{
			CKiana_Attack_Ultimate::ATTACKDESC UlAttackDesc;
			ZeroMemory(&UlAttackDesc, sizeof CKiana_Attack_Ultimate::ATTACKDESC);
			UlAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
			UlAttackDesc.AttackType = CKiana_Attack_Ultimate::ATTACK_3;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Ultimate")
				, &UlAttackDesc);

			CEffect_Kiana_Ultra::ATTACKDESC EffectAttackDesc;
			ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_Ultra::ATTACKDESC);
			EffectAttackDesc.m_pPlayerTransform = pTransform;
			EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
			EffectAttackDesc.AttackType = CEffect_Kiana_Ultra::ATTACK_3;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_Ultra")
				, &EffectAttackDesc);
		}

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (0.3 <= m_dAttackTimer)
	{
		if (CGameInstance::GetInstance()->DIKeyDown(DIK_A))
		{
			Player->Set_State(CKiana::STATE_ATTACK);
			pModel->Set_Animation_NonLerp(CKiana::ANIM_ATTACK_4);
			m_eCurCombo_Kiana = KIANA_ATTACK_4;
			m_dAttackTimer = 0.0;
			m_bAttackSwitch = true;
			return S_OK;
		}
	}
	
	if (70.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		if (Player->Check_DirInput())
		{
			Player->Set_State(CKiana::STATE_RUN);
			pModel->Set_Animation(CKiana::ANIM_RUN);
			Attack_Clear();
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CState_Attack::Kiana_Update_Attack_4(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_bLerpSwitch = true;

	m_dAttackTimer += TimeDelta;

	if (0.07 > m_dAttackTimer)
	{
		if (m_fTargetMinDistance < XMVectorGetX(XMVector3Length(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			/*m_fAcc = (powf(m_fAcc, 2.f) * 0.5f);
			m_fAccSpeed += m_fAcc;*/
			pTransform->Go_SetDir(CTransform::STATE_LOOK, 15.f, TimeDelta);
		}
	}

	if (true == m_bAttackSwitch && 0.22 <= m_dAttackTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_ATTACK4);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_NormalAttack_4.ogg"), CSoundMgr::KIANA_ATTACK4, 0.1f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_SWING);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_Swing.ogg"), CSoundMgr::KIANA_SWING, 0.2f);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CKiana_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CKiana_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CKiana_Attack_Normal::ATTACK_4;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Normal")
			, &AttackDesc);

		CEffect_Kiana_NormalAttack::ATTACKDESC EffectAttackDesc;
		ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_NormalAttack::ATTACKDESC);
		EffectAttackDesc.m_pPlayerTransform = pTransform;
		EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		EffectAttackDesc.AttackType = CEffect_Kiana_NormalAttack::ATTACK_4;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_NormalAttack")
			, &EffectAttackDesc);

		if (true == Player->Check_UltimateOn())
		{
			CKiana_Attack_Ultimate::ATTACKDESC UlAttackDesc;
			ZeroMemory(&UlAttackDesc, sizeof CKiana_Attack_Ultimate::ATTACKDESC);
			UlAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
			UlAttackDesc.AttackType = CKiana_Attack_Ultimate::ATTACK_4;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Ultimate")
				, &UlAttackDesc);

			CEffect_Kiana_Ultra::ATTACKDESC EffectAttackDesc;
			ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_Ultra::ATTACKDESC);
			EffectAttackDesc.m_pPlayerTransform = pTransform;
			EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
			EffectAttackDesc.AttackType = CEffect_Kiana_Ultra::ATTACK_4;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_Ultra")
				, &EffectAttackDesc);

			CEffect_Dust::DUSTDESC DustDesc;
			ZeroMemory(&DustDesc, sizeof CEffect_Dust::DUSTDESC);
			DustDesc.PlayerWorldMatrix = pTransform->Get_WorldMatrix();
			DustDesc.fRotationDir = -1.f;
			DustDesc.fSizeY = 1.6f;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Dust")
				, &DustDesc);

			DustDesc.fRotationDir = 1.f;
			DustDesc.fSizeY = 3.2f;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Dust")
				, &DustDesc);
		}

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (true == m_bAttackSwitch2 && 0.29 <= m_dAttackTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_ATTACK4);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_NormalAttack_4.ogg"), CSoundMgr::KIANA_ATTACK4, 0.1f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_SWING);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_Swing.ogg"), CSoundMgr::KIANA_SWING, 0.2f);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CKiana_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CKiana_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CKiana_Attack_Normal::ATTACK_1;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Normal")
			, &AttackDesc);

		CEffect_Kiana_NormalAttack::ATTACKDESC EffectAttackDesc;
		ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_NormalAttack::ATTACKDESC);
		EffectAttackDesc.m_pPlayerTransform = pTransform;
		EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		EffectAttackDesc.AttackType = CEffect_Kiana_NormalAttack::ATTACK_4;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_NormalAttack")
			, &EffectAttackDesc);

		Safe_Release(pGameInstance);

		m_bAttackSwitch2 = false;
	}

	if (0.4 <= m_dAttackTimer)
	{
		if (CGameInstance::GetInstance()->DIKeyDown(DIK_A))
		{
			if (CGameInstance::GetInstance()->DIKeyDown(DIK_A))
			{
				Player->Set_State(CKiana::STATE_ATTACK);
				pModel->Set_Animation_NonLerp(CKiana::ANIM_ATTACK_5);
				m_eCurCombo_Kiana = KIANA_ATTACK_5;
				m_dAttackTimer = 0.0;
				m_bAttackSwitch = true;
				m_bAttackSwitch2 = true;
				return S_OK;
			}
		}
	}	

	if (70.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		if (Player->Check_DirInput())
		{
			Player->Set_State(CKiana::STATE_RUN);
			pModel->Set_Animation(CKiana::ANIM_RUN);
			Attack_Clear();
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CState_Attack::Kiana_Update_Attack_5(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_bLerpSwitch = true;

	m_dAttackTimer += TimeDelta;

	if (0.07 > m_dAttackTimer)
	{
		if (m_fTargetMinDistance < XMVectorGetX(XMVector3Length(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			/*m_fAcc = (powf(m_fAcc, 2.f) * 0.5f);
			m_fAccSpeed += m_fAcc;*/
			pTransform->Go_SetDir(CTransform::STATE_LOOK, 15.f, TimeDelta);
		}
	}

	if (true == m_bAttackSwitch && 0.20 <= m_dAttackTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_ATTACK5);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_NormalAttack_5.ogg"), CSoundMgr::KIANA_ATTACK5, 0.1f);

		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_SWING);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_Swing.ogg"), CSoundMgr::KIANA_SWING, 0.2f);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CKiana_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CKiana_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CKiana_Attack_Normal::ATTACK_4;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Normal")
			, &AttackDesc);

		CEffect_Kiana_NormalAttack::ATTACKDESC EffectAttackDesc;
		ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_NormalAttack::ATTACKDESC);
		EffectAttackDesc.m_pPlayerTransform = pTransform;
		EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		EffectAttackDesc.AttackType = CEffect_Kiana_NormalAttack::ATTACK_5;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_NormalAttack")
			, &EffectAttackDesc);

		if (true == Player->Check_UltimateOn())
		{
			CKiana_Attack_Ultimate::ATTACKDESC UlAttackDesc;
			ZeroMemory(&UlAttackDesc, sizeof CKiana_Attack_Ultimate::ATTACKDESC);
			UlAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
			UlAttackDesc.AttackType = CKiana_Attack_Ultimate::ATTACK_5;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Ultimate")
				, &UlAttackDesc);

			CEffect_Kiana_Ultra::ATTACKDESC EffectAttackDesc;
			ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_Ultra::ATTACKDESC);
			EffectAttackDesc.m_pPlayerTransform = pTransform;
			EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
			EffectAttackDesc.AttackType = CEffect_Kiana_Ultra::ATTACK_5;

			pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_Ultra")
				, &EffectAttackDesc);
		}

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (true == m_bAttackSwitch2 && 0.29 <= m_dAttackTimer)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CKiana_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CKiana_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CKiana_Attack_Normal::ATTACK_1;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Normal")
			, &AttackDesc);

		Safe_Release(pGameInstance);

		m_bAttackSwitch2 = false;
	}	

	if (70.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc())
	{
		if (Player->Check_DirInput())
		{
			Player->Set_State(CKiana::STATE_RUN);
			pModel->Set_Animation_NonLerp(CKiana::ANIM_RUN);
			Attack_Clear();
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CState_Attack::Kiana_Update_Attack_Charge1(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_bLerpSwitch = true;

	m_dAttackTimer += TimeDelta;

	if (0.1 <= m_dAttackTimer && 0.2 >= m_dAttackTimer)
	{
		if (m_fTargetMinDistance < XMVectorGetX(XMVector3Length(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION))))
			pTransform->Go_Straight(TimeDelta);		
	}

	if (true == m_bAttackSwitch && 0.20 <= m_dAttackTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_CHARGE1);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_ChargeAttack_1.ogg"), CSoundMgr::KIANA_CHARGE1, 0.1f);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CKiana_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CKiana_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CKiana_Attack_Normal::ATTACK_CHARGE_1;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Normal")
			, &AttackDesc);

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (0.5 < m_dAttackTimer/*46.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc()*/)
	{		
		pModel->Set_Animation_NonLerp(CKiana::ANIM_CHARGE_ATTACK_2);
		m_eCurCombo_Kiana = KIANA_ATTACK_CHARGE_2;
		m_dAttackTimer = 0.0;
		m_bAttackSwitch = true;
		return S_OK;
	}

	return S_OK;
}

HRESULT CState_Attack::Kiana_Update_Attack_Charge2(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	m_dAttackTimer += TimeDelta;

	if (0.36 > m_dAttackTimer)
		if (m_fTargetMinDistance < XMVectorGetX(XMVector3Length(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION))))
			pTransform->Go_Straight(TimeDelta);	

	if (true == m_bAttackSwitch && 0.36 <= m_dAttackTimer)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::KIANA_CHARGE2);
		CSoundMgr::Get_Instance()->PlaySoundW(TEXT("Kiana_ChargeAttack_2.ogg"), CSoundMgr::KIANA_CHARGE2, 0.1f);

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		/*CKiana_Attack_Normal::ATTACKDESC AttackDesc;
		ZeroMemory(&AttackDesc, sizeof CKiana_Attack_Normal::ATTACKDESC);
		AttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		AttackDesc.AttackType = CKiana_Attack_Normal::ATTACK_CHARGE_2;

		pGameInstance->Add_GameObject(LEVEL_HYPERION, TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Normal")
			, &AttackDesc);*/

		CKiana_Attack_Ultimate::ATTACKDESC UlAttackDesc;
		ZeroMemory(&UlAttackDesc, sizeof CKiana_Attack_Ultimate::ATTACKDESC);
		UlAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		UlAttackDesc.AttackType = CKiana_Attack_Ultimate::ATTACK_CHARGE_2;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Kiana_Attack_Ultimate")
			, &UlAttackDesc);

		CEffect_Kiana_Ultra::ATTACKDESC EffectAttackDesc;
		ZeroMemory(&EffectAttackDesc, sizeof CEffect_Kiana_Ultra::ATTACKDESC);
		EffectAttackDesc.m_pPlayerTransform = pTransform;
		EffectAttackDesc.matPlayerTransform = pTransform->Get_WorldMatrix();
		EffectAttackDesc.AttackType = CEffect_Kiana_Ultra::ATTACK_CHARGE_2;

		pGameInstance->Add_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Kiana_Ultra")
			, &EffectAttackDesc);

		Safe_Release(pGameInstance);

		m_bAttackSwitch = false;
	}

	if (0.5 < m_dAttackTimer/*46.0 <= pModel->Get_CurrentAnim()->Get_TimeAcc()*/)
	{
		if (Player->Check_DirInput())
		{
			Player->Set_State(CKiana::STATE_RUN);
			pModel->Set_Animation(CKiana::ANIM_RUN);
			Attack_Clear();
			return S_OK;
		}
	}	

	return S_OK;
}

_bool Compare(CState_Attack::SORTDESC SortA, CState_Attack::SORTDESC SortB)
{
	return SortA.fLength < SortB.fLength;		
}

void CState_Attack::Set_Target(CTransform* pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLayer* pLayer = pGameInstance->GetInstance()->Get_Layer(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Monster"));

	if (nullptr != pLayer && 0 != pLayer->Get_ObjList().size())
	{
		list<CGameObject*> MonsterList = pLayer->Get_ObjList();		

		SORTDESC SortDesc;
		ZeroMemory(&SortDesc, sizeof SORTDESC);		

		_vector vPlayerPos = pTransform->Get_State(CTransform::STATE_POSITION);

		for (auto& pMonster : MonsterList)
		{			
			_vector vMonsterPos = static_cast<CMonster*>(pMonster)->Get_Pos();

			SortDesc.fLength = XMVectorGetX(XMVector3Length(vPlayerPos - vMonsterPos));
			SortDesc.pMonster = static_cast<CMonster*>(pMonster);

			m_SortList.push_back(SortDesc);			
		}			

		m_SortList.sort(Compare);

		if (nullptr != m_pTargetPos)
			Safe_Release(m_pTargetPos);

		m_pTargetPos = static_cast<CTransform*>(m_SortList.front().pMonster->Get_Component(TEXT("Com_Transform")));
		Safe_AddRef(m_pTargetPos);

		CUI_MonsterHp* pMonsterUI = static_cast<CUI_MonsterHp*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_MonsterUI"), TEXT("Prototype_GameObject_UI_MonsterHp")));
		pMonsterUI->Set_TargetHp(static_cast<CStatus*>(m_SortList.front().pMonster->Get_Component(TEXT("Com_Status"))));
	
		CUI_LockOn* pLockOn = static_cast<CUI_LockOn*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_LockOn"), TEXT("Prototype_GameObject_UI_LockOn")));
		pLockOn->Set_TargetHp(static_cast<CTransform*>(m_SortList.front().pMonster->Get_Component(TEXT("Com_Transform"))));

		m_SortList.clear();			
	}

	Safe_Release(pGameInstance);
}

void CState_Attack::Aiming_Target(CTransform* pTransform, _double TimeDelta)
{
	if (nullptr == m_pTargetPos)
		return;

	if (m_dLerpTime < m_dLerpTimer)
	{
		m_dLerpTimer = 0;
		m_bLerpSwitch = false;
	}	

	_matrix matRotation = XMMatrixIdentity();
	_vector RotLook = XMVector3Normalize(vTargetPos - pTransform->Get_State(CTransform::STATE_POSITION));		
	_vector RotRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), RotLook));
	_vector RotUp = XMVector3Normalize(XMVector3Cross(RotLook, RotRight));

	matRotation.r[0] = RotRight;
	matRotation.r[1] = RotUp;
	matRotation.r[2] = RotLook;

	_vector Quaternion = XMQuaternionRotationMatrix(matRotation);

	_matrix matRot = pTransform->Get_WorldMatrix();

	_vector vScale;
	_vector vRotation;
	_vector vTranslation;

	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, matRot);

	if (m_dLerpTime >= m_dLerpTimer)
	{
		m_dLerpTimer += TimeDelta;

		Quaternion = XMQuaternionNormalize(XMQuaternionSlerp(vRotation, Quaternion, _float(m_dLerpTimer / m_dLerpTime)));

		pTransform->Rotation_Matrix(Quaternion);
	}	
}

CState_Attack* CState_Attack::Create(CHARACTER eCharactorType)
{
	CState_Attack* pInstance = new CState_Attack(eCharactorType);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Failed to Created : CState_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Attack::Free(void)
{
	__super::Free();

	Safe_Release(m_pTargetPos);
}
