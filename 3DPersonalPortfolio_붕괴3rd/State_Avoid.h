#pragma once

#include "State.h"

BEGIN(Engine)

class CTransform;
class CModel;

END

BEGIN(Client)

class CKiana;
class CTheresa;
class CPlayer;

class CState_Avoid final : public CState
{
private:
	CState_Avoid(CHARACTER eCharactorType);
	virtual ~CState_Avoid() = default;

public:
	virtual HRESULT Initialize_ProtoType(CTransform* pTransform);
	virtual void Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Set_BackDash(CModel* pModel, _fvector Dir);
	void Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Evade_Clear();

private:
	CTransform* m_pPlayerTransform = { nullptr };

private:
	_bool Is_BackDash = { false };
	_bool m_bSoundSwitch = { true };
	_vector	m_vDashDir;
	_double m_dDashTimer = { 0.0 };
	
public:
	static CState_Avoid* Create(CTransform* pTransform, CHARACTER eCharactorType);
	virtual void Free(void) override;
};

END