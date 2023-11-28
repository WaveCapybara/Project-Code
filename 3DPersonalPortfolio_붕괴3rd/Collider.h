#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
	enum DATA { DATA_ORIGINAL, DATA_INVALIDATE, DATA_END };

public:
	typedef struct tagColliderDesc
	{
		_float3		vPosition;
		_float3		vSize;
		_float3		vRotation;
	}COLLIDERDESC;

	typedef struct tagCollider
	{
		class CGameObject* pGameObject;
		_bool m_isColl_Finished;
	}COLLIDER;

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_bool Intersect(class CCollider* pTargetCollider);
	void Check_Intersect(class CGameObject* pGameObject, TYPE eCollType);
	_bool Is_Collide(class CCollider* pTargetCollider);

public:
	list<COLLIDER>* Get_Collider() {		
		return &m_ColliderList;
	}


public:
	void Tick(_fmatrix TransformMatrix);
	HRESULT Render();

private:
	BoundingBox* m_pAABB[DATA_END] = { nullptr };
	BoundingOrientedBox* m_pOBB[DATA_END] = { nullptr };
	BoundingSphere* m_pSphere[DATA_END] = { nullptr };

	TYPE							m_eColliderType = { TYPE_END };
	COLLIDERDESC					m_ColliderDesc;
	_bool							m_isColl = { false };
	list<COLLIDER>					m_ColliderList;

#ifdef _DEBUG
private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pPrimitiveBatch = { nullptr };
	BasicEffect* m_pEffects = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END