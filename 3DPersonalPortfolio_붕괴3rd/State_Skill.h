#pragma once

#include "State.h"

BEGIN(Engine)


class CTransform;
class CNavigation;
class CModel;

END

BEGIN(Client)

class CPlayer;

class CState_Skill final : public CState
{
private:
	CState_Skill(CHARACTER eCharactorType);
	virtual ~CState_Skill() = default;

public:
	virtual HRESULT Initialize_ProtoType();
	virtual void Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Kiana_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Theresa_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Skill_Clear() { 
		m_dAttackTimer = 0.0;
		m_bAttackSwitch = true; }

private:
	_double	m_dAttackTimer = { 0.0 };
	_bool m_bAttackSwitch = { true };


public:
	static CState_Skill* Create(CHARACTER eCharactorType);
	virtual void Free(void) override;
};

END