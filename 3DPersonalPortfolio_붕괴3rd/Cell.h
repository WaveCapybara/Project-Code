#pragma once

#include "Base.h"

/* 네비게이션의 삼각형 하나를 구성하는 클래스다. */
/* 세 변(점)의 정보를 저장한다. */
/* 이웃 삼각형 세개의 정보를 젖아한다. */
/* 이 삼각형을 나갔냐 안나갔냐 */

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	const _float3* Get_Point(POINT ePoint) const {
		return &m_vPoints[ePoint];
	}

	const _float3* Get_Normals(LINE eLine) const {
		return &m_vNormals[eLine];
	}

	void SetUp_Neighbor(LINE eLine, class CCell* pNeighbor) {
		m_iNeighborIndex[eLine] = pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _uint iIndex);
	_bool Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);
	_bool isIn(_fvector vPosition, _int* pNeighborIndex);
	_vector ifOut(_fvector vPosition, _vector vDir);

#ifdef _DEBUG
public:
	HRESULT Render_Debug(const _float4* pColor, _float fY);
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
	class CShader* m_pShader = { nullptr };

private:
	HRESULT Ready_Debug();
#endif

private:
	_float3					m_vPoints[POINT_END];
	_float3					m_vNormals[LINE_END];
	_int					m_iIndex = { 0 };
	_int					m_iNeighborIndex[LINE_END] = { -1, -1, -1 };


public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END