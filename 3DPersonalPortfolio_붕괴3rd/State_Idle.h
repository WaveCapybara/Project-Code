#pragma once

#include "State.h"

BEGIN(Engine)

class CTransform;
class CNavigation;
class CModel;

END

BEGIN(Client)

class CPlayer;

class CState_Idle final : public CState
{
private:
	CState_Idle(CHARACTER eCharactorType);
	virtual ~CState_Idle() = default;

public:
	virtual HRESULT Initialize_ProtoType();
	virtual void Update_State(CPlayer* Player, CModel* pModel , CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);

private:
	void Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);

public:
	static CState_Idle* Create(CHARACTER eCharactorType);
	virtual void Free(void) override;
};

END