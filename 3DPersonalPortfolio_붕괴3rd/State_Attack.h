#pragma once

#include "State.h"

BEGIN(Engine)

class CModel;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CPlayer;
class CTheresa;
class CKiana;

class CState_Attack final : public CState
{
public:
	typedef struct tagLengthSortDesc
	{
		_float fLength;
		class CMonster* pMonster;
	}SORTDESC;

public:
	enum ATTACKCOMBO { ATTACK_1, ATTACK_2, ATTACK_3, ATTACK_CHARGE, ATTACK_END };
	enum ATTACKCOMBO_KIANA { KIANA_ATTACK_1, KIANA_ATTACK_2, KIANA_ATTACK_3, KIANA_ATTACK_4, KIANA_ATTACK_5, 
		KIANA_ATTACK_CHARGE_1, KIANA_ATTACK_CHARGE_2, KIANA_ATTACK_END };

private:
	CState_Attack(CHARACTER eCharactorType);
	virtual ~CState_Attack() = default;

public:
	virtual HRESULT Initialize_ProtoType();
	virtual void Update_State(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	void	Attack_Clear();

private:
	void Theresa_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Theresa_Update_Attack_1(CTheresa* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Theresa_Update_Attack_2(CTheresa* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Theresa_Update_Attack_3(CTheresa* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Theresa_Update_Attack_Charge(CTheresa* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);

private:
	void Kiana_Update(CPlayer* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Kiana_Update_Attack_1(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Kiana_Update_Attack_2(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Kiana_Update_Attack_3(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Kiana_Update_Attack_4(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Kiana_Update_Attack_5(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Kiana_Update_Attack_Charge1(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);
	HRESULT	Kiana_Update_Attack_Charge2(CKiana* Player, CModel* pModel, CTransform* pTransform, CNavigation* pNavigation, _double TimeDelta);

private:
	void Set_Target(CTransform* pTransform);
	void Aiming_Target(CTransform* pTransform, _double TimeDelta);	

private:
	ATTACKCOMBO m_CurCombo = { ATTACK_1 };
	ATTACKCOMBO_KIANA m_eCurCombo_Kiana = { KIANA_ATTACK_1 };

	_double m_dChargeTimer = { 0.0 };
	_double m_dAttackTimer = { 0.0 };
	_double m_dLerpTimer = { 0.0 };
	_double m_dLerpTime = { 0.2 };
	_double m_dEffectTimer = { 0.0 };

	_bool	m_bAttackSwitch = { true };
	_bool	m_bAttackSwitch2 = { true };
	_bool	m_bAimingSwitch = { true };
	_bool	m_bLerpSwitch = { false };

	_float m_fAcc = { 2.f };
	_float m_fLowAcc = { 10.f };
	_float	m_fAccSpeed = { 0.f };

	class CTransform* m_pTargetPos = { nullptr };
	_vector vTargetPos;
	_float m_fTargetMinDistance = { 0.f };

private:
	list<SORTDESC> m_SortList;

public:
	static CState_Attack* Create(CHARACTER eCharactorType);
	virtual void Free(void) override;
};

END