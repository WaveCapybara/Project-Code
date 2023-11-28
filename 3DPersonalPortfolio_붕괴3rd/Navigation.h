#pragma once

#include "Component.h"

/* �׺�ۼ��� �����ϴ� �ﰢ������ ��Ƽ� �����Ѵ�. */
/* �׺���̼��� �̿��ϴ� ��ü�� � �쿡 �����ϴ����� ���� �ε����� �����Ѵ�.  */
/* ��ȿ����� �����ӿ����� �Ǵ��� �I���. */

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

public: /* �׺���̼��� �����ϴ� ������ ����� �̿�(�ִ�3��)�� �����Ѵ�. */
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