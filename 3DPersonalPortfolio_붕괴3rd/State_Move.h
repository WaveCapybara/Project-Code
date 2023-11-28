#pragma once

#include "State.h"

BEGIN(Engine)

class CTransform;
class CNavigation;
class CModel;

END

BEGIN(Client)

class CPlayer;

class CState_Move final : public CState
{
private:
	CState_Move(CHARACTER eCharactorType);
	virtual ~CState_Move() = default;

public:
	virtual HRESULT Initialize_ProtoType();
	virtual void Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);

private:
	void Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);

private:
	_double m_dSoundTimer = { 0.0 };

public:
	static CState_Move* Create(CHARACTER eCharactorType);
	virtual void Free(void) override;
};

END
