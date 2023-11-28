#pragma once

#include "State.h"

BEGIN(Engine)


class CTransform;
class CNavigation;
class CModel;

END

BEGIN(Client)

class CPlayer;

class CState_Ultimate final : public CState
{
public:
	typedef struct tagUltimateDesc
	{
		_matrix matPlayerWorld;
		_vector vPlayerPos;
		_vector vAttackPos;
		_uint AttackCount;

	}ULDESC;

private:
	CState_Ultimate(CHARACTER eCharactorType);
	virtual ~CState_Ultimate() = default;

public:
	virtual HRESULT Initialize_ProtoType();
	virtual void Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Kiana_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Theresa_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void Ultimate_Clear();

private:
	_double	m_dAttackTimer = { 0.0 };

	_bool m_bAttackSwitch = { true };
	_bool m_bCameraSwitch = { true };
	_bool m_bEffectSwitch = { true };
	_bool m_bEffectSwitch2 = { true };

public:
	static CState_Ultimate* Create(CHARACTER eCharactorType);
	virtual void Free(void) override;
};

END