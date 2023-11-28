#include "stdafx.h" 
#include "State_SwitchOut.h"
#include "Theresa.h"
#include "Kiana.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "Monster.h"
#include "State_Avoid.h"
#include "Model.h"
#include "Navigation.h"
#include "Animation.h"
#include "StaticCamera.h"
#include "Skill_Interface.h"

CState_SwitchOut::CState_SwitchOut(CHARACTER eCharactorType)
	:CState(eCharactorType)
{
}

HRESULT CState_SwitchOut::Initialize_ProtoType()
{
	return S_OK;
}

void CState_SwitchOut::Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
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

void CState_SwitchOut::Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{					
		CTheresa* pTheresa = static_cast<CTheresa*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Prototype_GameObject_Kiana")));
		pTheresa->Swap_Avatar(pTransform->Get_State(CTransform::STATE_POSITION), pNavigation->Get_CurrentCell());

		/*CSkill_Interface* pInterface = static_cast<CSkill_Interface*>(CGameInstance::GetInstance()->Get_GameObject(LEVEL_HYPERION, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Skill_Interface")));
		pInterface->Swap_Avatar(CSkill_Interface::UI_THERESA);*/

		CLayer* pLayer = CGameInstance::GetInstance()->Get_Layer(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Monster"));

		if (nullptr != pLayer)
		{
			list<CGameObject*> MonsterList = pLayer->Get_ObjList();

			for (auto& pMonster : MonsterList)
				static_cast<CMonster*>(pMonster)->Change_Avatar(CState::CHARACTER_THERESA);
		}

		CStaticCamera* pCamera = static_cast<CStaticCamera*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_StaticCamera")));
		pCamera->is_Changed(CState::CHARACTER_THERESA);

		pTheresa->Set_ReTick();
		static_cast<CKiana*>(Player)->Set_StopTick();

		return;		
	}
}

void CState_SwitchOut::Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta)
{
	if (pModel->Get_CurrentAnim()->Get_AnimFinished())
	{		
		CKiana* pKiana = static_cast<CKiana*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Kiana"), TEXT("Prototype_GameObject_Kiana")));
		pKiana->Swap_Avatar(pTransform->Get_State(CTransform::STATE_POSITION), pNavigation->Get_CurrentCell());

		/*CSkill_Interface* pInterface = static_cast<CSkill_Interface*>(CGameInstance::GetInstance()->Get_GameObject(LEVEL_HYPERION, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Skill_Interface")));
		pInterface->Swap_Avatar(CSkill_Interface::UI_KIANA);*/

		CLayer* pLayer = CGameInstance::GetInstance()->Get_Layer(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Monster"));

		if (nullptr != pLayer)
		{
			list<CGameObject*> MonsterList = pLayer->Get_ObjList();

			for (auto& pMonster : MonsterList)
				static_cast<CMonster*>(pMonster)->Change_Avatar(CState::CHARACTOR_KIANA);					
		}

		CStaticCamera* pCamera = static_cast<CStaticCamera*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_StaticCamera")));
		pCamera->is_Changed(CState::CHARACTOR_KIANA);

		pKiana->Set_ReTick();
		static_cast<CTheresa*>(Player)->Set_StopTick();

		return;
	}
}

CState_SwitchOut* CState_SwitchOut::Create(CHARACTER eCharactorType)
{
	CState_SwitchOut* pInstance = new CState_SwitchOut(eCharactorType);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Failed to Created : CState_SwitchOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_SwitchOut::Free(void)
{
	__super::Free();
}

