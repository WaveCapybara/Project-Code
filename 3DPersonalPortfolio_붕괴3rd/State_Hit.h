#pragma once

#include "State.h"

BEGIN(Engine)

class CModel;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CPlayer;

class CState_Hit final : public CState
{
private:
	CState_Hit(CHARACTER eCharactorType);
	virtual ~CState_Hit() = default;

public:
	virtual HRESULT Initialize_ProtoType();
	virtual void Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);

private:
	void Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);

	_double TimaAcc = { 0.0 };

public:
	static CState_Hit* Create(CHARACTER eCharactorType);
	virtual void Free(void) override;
};

END

