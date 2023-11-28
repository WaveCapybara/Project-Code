#pragma once

#include "Base.h"

BEGIN(Engine)

class CTransform;

END

BEGIN(Client)

class CTheresa;

class CState abstract : public CBase
{
public:
	enum CHARACTER { CHARACTOR_KIANA, CHARACTER_THERESA, CHARACTER_END };

protected:
	CState(CHARACTER eCharactorType);
	virtual ~CState() = default;

public:
	virtual HRESULT Initialize_ProtoType();
	virtual void Update_State(CTransform* pTransform, _double TimeDelta);
	void Lerp_Look(CTransform* pTransform, _double TimeDelta);
	_vector	Calculate_Quaternion(_fvector CamLook, _matrix matRot);

protected:
	CHARACTER m_eCharacter = { CHARACTER_END };

private:
	void LerpTimer_Clear() {
		m_bLerpSwitch = true;
		m_dLerpTimer = 0.0;
	}

private:
	_bool is_Init = { true };
	_bool m_bLerpSwitch = { false };
	_double m_dLerpTimer = { 0.0 };
	_vector m_vNextLook;
	_vector	m_NextQuaternion;
	CTransform* m_pCamTransform = { nullptr };	

	_double m_dLerpTime = { 0.2 };

public:
	virtual void Free(void) override;
};

END