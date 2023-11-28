#pragma once

#include "State.h"

BEGIN(Engine)

class CTransform;
class CNavigation;
class CModel;

END

BEGIN(Client)

class CPlayer;

class CState_SwitchIn final : public CState
{
private:
	CState_SwitchIn(CHARACTER eCharactorType);
	virtual ~CState_SwitchIn() = default;

public:
	virtual HRESULT Initialize_ProtoType();
	virtual void Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);

private:
	void Update_Kiana(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Update_Theresa(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);

private:
	_bool m_bSoundSwitch = { true };

private:
	void SwitchIn_Clear();

public:
	static CState_SwitchIn* Create(CHARACTER eCharactorType);
	virtual void Free(void) override;
};

END