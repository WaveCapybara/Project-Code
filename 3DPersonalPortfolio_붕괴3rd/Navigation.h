#pragma once

#include "Component.h"

/* 네비겡션을 구성하는 삼각형들을 모아서 보관한다. */
/* 네비게이션을 이용하는 객체가 어떤 쎌에 존재하는지에 대한 인덱스를 보관한다.  */
/* 쎌안에서의 움직임에대한 판단을 숳앻나다. */

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNaviDesc
	{
		_int	iCurrentIndex = { -1 };
	}NAVIDESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;

public: /* 네비게이션을 구성하는 각각의 쎌들의 이웃(최대3개)을 셋팅한다. */
	HRESULT SetUp_Neighbors();
	_vector is_MoveOnNavigation(_fvector vPosition, _vector vDir);

public:
	_uint Get_CurrentCell() { return m_NaviDesc.iCurrentIndex; }
	void Set_CurrentCell(_uint iCell) {	m_NaviDesc.iCurrentIndex = iCell; }

#ifdef _DEBUG
public:
	HRESULT Render();

#endif

private:
	_uint	m_iCellCount = { 0 };
	vector<class CCell*>				m_Cells;

	NAVIDESC							m_NaviDesc;




public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END