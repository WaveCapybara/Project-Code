#include "Navigation.h"
#include "Cell.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFile)
{
	char buf[MAX_PATH];

	ifstream readFile;
	readFile.open(pNavigationDataFile, ios::in | ios::binary);
	
	readFile.read(buf, sizeof(_uint));
	memcpy(&m_iCellCount, buf, sizeof(_uint));

	_int	Index;
	_float3	vPoints[CCell::POINT_END];

	for (_uint i = 0; i < m_iCellCount; i++)
	{
		readFile.read(buf, sizeof(_int));
		memcpy(&Index, buf, sizeof(_int));

		readFile.read(buf, sizeof(_float3));
		memcpy(&vPoints[CCell::POINT_A], buf, sizeof(_float3));

		readFile.read(buf, sizeof(_float3));
		memcpy(&vPoints[CCell::POINT_B], buf, sizeof(_float3));

		readFile.read(buf, sizeof(_float3));
		memcpy(&vPoints[CCell::POINT_C], buf, sizeof(_float3));

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, Index);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}	

	readFile.close();

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;	

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_NaviDesc = *(NAVIDESC*)pArg;

	return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

_vector CNavigation::is_MoveOnNavigation(_fvector vPosition, _vector vDir)
{
	if (-1 == m_NaviDesc.iCurrentIndex)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_int	iNeighborIndex = { -1 };

	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex))
	{
		return vDir;
	}
	else
	{		
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return XMVectorSet(0.f, 0.f, 0.f, 0.f);

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
					break;
			}

			m_NaviDesc.iCurrentIndex = iNeighborIndex;
			return vDir;
		}
		else
		{			
			return m_Cells[m_NaviDesc.iCurrentIndex]->ifOut(vPosition, vDir);
		}		
	}	
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	_float4 Green = { 0.f, 1.f, 0.f, 1.f };
	_float4 Red = { 1.f, 1.f, 1.f, 1.f };

	if (-1 == m_NaviDesc.iCurrentIndex)
	{
		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render_Debug(&Green, 0.0f);
		}
	}
	else
	{
		m_Cells[m_NaviDesc.iCurrentIndex]->Render_Debug(&Red, 0.1f);
	}

	return S_OK;
}

#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFile)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}
