#include "Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"
#include "GameObject.h"


CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_eColliderType(rhs.m_eColliderType)
	, m_ColliderList(rhs.m_ColliderList)
#ifdef _DEBUG
	, m_pPrimitiveBatch(rhs.m_pPrimitiveBatch)
	, m_pEffects(rhs.m_pEffects)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif 
{

#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif 

}

HRESULT CCollider::Initialize_Prototype(TYPE eColliderType)
{
	m_eColliderType = eColliderType;

#ifdef _DEBUG
	m_pPrimitiveBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
	m_pEffects = new BasicEffect(m_pDevice);

	m_pEffects->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iLength = { 0 };

	m_pEffects->GetVertexShaderBytecode(&pShaderByteCode, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount, pShaderByteCode, iLength, &m_pInputLayout)))
		return E_FAIL;
#endif



	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_ColliderDesc, pArg, sizeof m_ColliderDesc);

	for (_uint i = 0; i < DATA_END; ++i)
	{
		switch (m_eColliderType)
		{
		case TYPE_AABB:
			m_pAABB[i] = new BoundingBox(m_ColliderDesc.vPosition, _float3(m_ColliderDesc.vSize.x * 0.5f, m_ColliderDesc.vSize.y * 0.5f, m_ColliderDesc.vSize.z * 0.5f));
			break;
		case TYPE_OBB:
			m_pOBB[i] = new BoundingOrientedBox(m_ColliderDesc.vPosition,
				_float3(m_ColliderDesc.vSize.x * 0.5f, m_ColliderDesc.vSize.y * 0.5f, m_ColliderDesc.vSize.z * 0.5f),
				_float4(0.f, 0.f, 0.f, 1.f));

			m_pOBB[i]->Transform(*m_pOBB[i], XMMatrixRotationX(m_ColliderDesc.vRotation.x) * XMMatrixRotationY(m_ColliderDesc.vRotation.y) * XMMatrixRotationZ(m_ColliderDesc.vRotation.z));
			break;
		case TYPE_SPHERE:
			m_pSphere[i] = new BoundingSphere(m_ColliderDesc.vPosition, m_ColliderDesc.vSize.x);
			break;
		}
	}


	return S_OK;
}

_bool CCollider::Intersect(CCollider* pTargetCollider)
{
	m_isColl = { false };
	pTargetCollider->m_isColl = { false };
	
	if (TYPE_AABB == m_eColliderType)
	{
		if (TYPE_AABB == pTargetCollider->m_eColliderType)
			m_isColl = m_pAABB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pAABB[DATA_INVALIDATE]);
		if (TYPE_OBB == pTargetCollider->m_eColliderType)
			m_isColl = m_pAABB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pOBB[DATA_INVALIDATE]);
		if (TYPE_SPHERE == pTargetCollider->m_eColliderType)
			m_isColl = m_pAABB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pSphere[DATA_INVALIDATE]);
	}

	if (TYPE_OBB == m_eColliderType)
	{
		if (TYPE_AABB == pTargetCollider->m_eColliderType)
			m_isColl = m_pOBB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pAABB[DATA_INVALIDATE]);
		if (TYPE_OBB == pTargetCollider->m_eColliderType)
			m_isColl = m_pOBB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pOBB[DATA_INVALIDATE]);
		if (TYPE_SPHERE == pTargetCollider->m_eColliderType)
			m_isColl = m_pOBB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pSphere[DATA_INVALIDATE]);
	}

	if (TYPE_SPHERE == m_eColliderType)
	{
		if (TYPE_AABB == pTargetCollider->m_eColliderType)
			m_isColl = m_pSphere[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pAABB[DATA_INVALIDATE]);
		if (TYPE_OBB == pTargetCollider->m_eColliderType)
			m_isColl = m_pSphere[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pOBB[DATA_INVALIDATE]);
		if (TYPE_SPHERE == pTargetCollider->m_eColliderType)
			m_isColl = m_pSphere[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pSphere[DATA_INVALIDATE]);
	}

	pTargetCollider->m_isColl = m_isColl;

	return m_isColl;
}

void CCollider::Check_Intersect(CGameObject* pGameObject, TYPE eCollType)
{
	for (auto& pGameObjects : m_ColliderList)
	{
		if (pGameObject == pGameObjects.pGameObject)
			return;
	}

	CCollider* pCollider = nullptr;

	switch (eCollType)
	{
	case Engine::CCollider::TYPE_AABB:
		pCollider = static_cast<CCollider*>(pGameObject->Get_Component(TEXT("Com_AABB")));
		break;

	case Engine::CCollider::TYPE_OBB:
		pCollider = static_cast<CCollider*>(pGameObject->Get_Component(TEXT("Com_OBB")));
		break;

	case Engine::CCollider::TYPE_SPHERE:
		pCollider = static_cast<CCollider*>(pGameObject->Get_Component(TEXT("Com_Sphere")));
		break;

	default:
		break;
	}

	if (nullptr == pCollider)
		return;
	
	if (Is_Collide(pCollider))
	{
		COLLIDER Collider;
		ZeroMemory(&Collider, sizeof COLLIDER);

		Collider.pGameObject = pGameObject;
		Safe_AddRef(Collider.pGameObject);
		Collider.m_isColl_Finished = false;

		m_ColliderList.push_back(Collider);
		return;
	}

	else
		return;
}

_bool CCollider::Is_Collide(CCollider* pTargetCollider)
{
	_bool Collide = false;

	if (TYPE_AABB == m_eColliderType)
	{
		if (TYPE_AABB == pTargetCollider->m_eColliderType)
			Collide = m_pAABB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pAABB[DATA_INVALIDATE]);
		if (TYPE_OBB == pTargetCollider->m_eColliderType)
			Collide = m_pAABB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pOBB[DATA_INVALIDATE]);
		if (TYPE_SPHERE == pTargetCollider->m_eColliderType)
			Collide = m_pAABB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pSphere[DATA_INVALIDATE]);
	}

	if (TYPE_OBB == m_eColliderType)
	{
		if (TYPE_AABB == pTargetCollider->m_eColliderType)
			Collide = m_pOBB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pAABB[DATA_INVALIDATE]);
		if (TYPE_OBB == pTargetCollider->m_eColliderType)
			Collide = m_pOBB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pOBB[DATA_INVALIDATE]);
		if (TYPE_SPHERE == pTargetCollider->m_eColliderType)
			Collide = m_pOBB[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pSphere[DATA_INVALIDATE]);
	}

	if (TYPE_SPHERE == m_eColliderType)
	{
		if (TYPE_AABB == pTargetCollider->m_eColliderType)
			Collide = m_pSphere[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pAABB[DATA_INVALIDATE]);
		if (TYPE_OBB == pTargetCollider->m_eColliderType)
			Collide = m_pSphere[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pOBB[DATA_INVALIDATE]);
		if (TYPE_SPHERE == pTargetCollider->m_eColliderType)
			Collide = m_pSphere[DATA_INVALIDATE]->Intersects(*pTargetCollider->m_pSphere[DATA_INVALIDATE]);
	}

	return Collide;
}

void CCollider::Tick(_fmatrix TransformMatrix)
{
	switch (m_eColliderType)
	{
	case TYPE_AABB:
		m_pAABB[DATA_ORIGINAL]->Transform(*m_pAABB[DATA_INVALIDATE], Remove_Rotation(TransformMatrix));
		break;

	case TYPE_OBB:
		m_pOBB[DATA_ORIGINAL]->Transform(*m_pOBB[DATA_INVALIDATE], TransformMatrix);
		break;

	case TYPE_SPHERE:
		m_pSphere[DATA_ORIGINAL]->Transform(*m_pSphere[DATA_INVALIDATE], TransformMatrix);
		break;
	}
}



HRESULT CCollider::Render()
{
#ifdef _DEBUG
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffects->SetWorld(XMMatrixIdentity());

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	m_pEffects->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffects->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	Safe_Release(pPipeLine);

	m_pEffects->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pPrimitiveBatch->Begin();

	_vector		vColor = m_isColl == true ? XMVectorSet(1.f, 1.f, 1.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f);

	switch (m_eColliderType)
	{
	case TYPE_AABB:
		DX::Draw(m_pPrimitiveBatch, *m_pAABB[DATA_INVALIDATE], vColor);
		break;
	case TYPE_OBB:
		DX::Draw(m_pPrimitiveBatch, *m_pOBB[DATA_INVALIDATE], vColor);
		break;
	case TYPE_SPHERE:
		DX::Draw(m_pPrimitiveBatch, *m_pSphere[DATA_INVALIDATE], vColor);
		break;
	}

	m_pPrimitiveBatch->End();

#endif 

	return S_OK;
}



_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_vector			vScale, vRotation, vTranslation;

	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, TransformMatrix);

	return XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), vTranslation);	
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);

	if (false == m_isCloned)
	{
		Safe_Delete(m_pEffects);
		Safe_Delete(m_pPrimitiveBatch);
	}
#endif

	for (_uint i = 0; i < DATA_END; ++i)
	{
		Safe_Delete(m_pAABB[i]);
		Safe_Delete(m_pOBB[i]);
		Safe_Delete(m_pSphere[i]);
	}

	for (auto& iter : m_ColliderList)
		Safe_Release(iter.pGameObject);
	m_ColliderList.clear();

}
