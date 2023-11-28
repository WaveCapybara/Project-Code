#include "stdafx.h" 
#include "State.h"
#include "GameInstance.h"
#include "Theresa.h"
#include "StaticCamera.h"
#include "Transform.h"

CState::CState(CHARACTER eCharactorType)
	:m_eCharacter(eCharactorType)
{
}

HRESULT CState::Initialize_ProtoType()
{	
	return S_OK;
}

void CState::Update_State(CTransform* pTransform, _double TimeDelta)
{	
	if (true == is_Init)
	{
		CStaticCamera* vCam = dynamic_cast<CStaticCamera*>(CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Camera"), TEXT("Prototype_GameObject_StaticCamera")));
		m_pCamTransform = vCam->Get_Transform();
		Safe_AddRef(m_pCamTransform);
		m_vNextLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		m_NextQuaternion = XMVectorSet(0.f, 0.f, 0.f, 0.f );
		is_Init = false;
	}
	
	_vector vCamLook = m_pCamTransform->Get_State(CTransform::STATE_LOOK);	
	vCamLook = XMVectorSetY(vCamLook, 0.f);	

	if (CGameInstance::GetInstance()->DIKeyPress(DIK_UP) && CGameInstance::GetInstance()->DIKeyPress(DIK_RIGHT))		
	{
		m_NextQuaternion = Calculate_Quaternion(vCamLook, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f)));
		LerpTimer_Clear();
	}

	else if (CGameInstance::GetInstance()->DIKeyPress(DIK_DOWN) && CGameInstance::GetInstance()->DIKeyPress(DIK_RIGHT))
	{
		m_NextQuaternion = Calculate_Quaternion(vCamLook, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(135.f)));
		LerpTimer_Clear();
	}

	else if (CGameInstance::GetInstance()->DIKeyPress(DIK_UP) && CGameInstance::GetInstance()->DIKeyPress(DIK_LEFT))
	{
		m_NextQuaternion = Calculate_Quaternion(vCamLook, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(315.f)));
		LerpTimer_Clear();
	}

	else if (CGameInstance::GetInstance()->DIKeyPress(DIK_DOWN) && CGameInstance::GetInstance()->DIKeyPress(DIK_LEFT))
	{
		m_NextQuaternion = Calculate_Quaternion(vCamLook, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(225.f)));
		LerpTimer_Clear();
	}

	else if (CGameInstance::GetInstance()->DIKeyPress(DIK_UP))
	{
		m_NextQuaternion = Calculate_Quaternion(vCamLook, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f)));
		LerpTimer_Clear();
	}

	else if (CGameInstance::GetInstance()->DIKeyPress(DIK_DOWN))
	{
		m_NextQuaternion = Calculate_Quaternion(vCamLook, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f)));
		LerpTimer_Clear();
	}

	else if (CGameInstance::GetInstance()->DIKeyPress(DIK_LEFT))
	{
		m_NextQuaternion = Calculate_Quaternion(vCamLook, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f)));
		LerpTimer_Clear();
	}

	else if (CGameInstance::GetInstance()->DIKeyPress(DIK_RIGHT))
	{
		m_NextQuaternion = Calculate_Quaternion(vCamLook, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)));
		LerpTimer_Clear();
	}
	
	if(true == m_bLerpSwitch)
		Lerp_Look(pTransform, TimeDelta);
}

void CState::Lerp_Look(CTransform* pTransform, _double TimeDelta)
{
	_matrix matRotation = pTransform->Get_WorldMatrix();

	_vector vScale;
	_vector vRotation;
	_vector vTranslation;

	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, matRotation);

	if (m_dLerpTime >= m_dLerpTimer)
	{
		m_dLerpTimer += TimeDelta;

		m_NextQuaternion = XMQuaternionNormalize(XMQuaternionSlerp(vRotation, m_NextQuaternion, _float(m_dLerpTimer / m_dLerpTime)));

		pTransform->Rotation_Matrix(m_NextQuaternion);
	}

	else
	{
		m_dLerpTimer = 0;
		m_bLerpSwitch = false;
	}
}

_vector CState::Calculate_Quaternion(_fvector CamLook, _matrix matRot)
{
	_matrix matRotation = XMMatrixIdentity();
	_vector RotLook = XMVector3Normalize(XMVector3TransformNormal(CamLook, matRot));	
	_vector RotRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), RotLook));
	_vector RotUp = XMVector3Normalize(XMVector3Cross(RotLook, RotRight));

	matRotation.r[0] = RotRight;
	matRotation.r[1] = RotUp;
	matRotation.r[2] = RotLook;

	_vector Quaternion = XMQuaternionRotationMatrix(matRotation);

	return Quaternion;	
}

void CState::Free(void)
{	
	Safe_Release(m_pCamTransform);
}
