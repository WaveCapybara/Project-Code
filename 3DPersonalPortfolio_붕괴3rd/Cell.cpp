#include "..\Public\Cell.h"

#ifdef _DEBUG
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Cell.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;

	XMStoreFloat3(&m_vNormals[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	m_vNormals[LINE_AB] = _float3(m_vNormals[LINE_AB].z * -1.f, 0.0f, m_vNormals[LINE_AB].x);

	XMStoreFloat3(&m_vNormals[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	m_vNormals[LINE_BC] = _float3(m_vNormals[LINE_BC].z * -1.f, 0.0f, m_vNormals[LINE_BC].x);

	XMStoreFloat3(&m_vNormals[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));
	m_vNormals[LINE_CA] = _float3(m_vNormals[LINE_CA].z * -1.f, 0.0f, m_vNormals[LINE_CA].x);


#ifdef _DEBUG
	if (FAILED(Ready_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(pSourPoint), XMLoadFloat3(&m_vPoints[POINT_A])))
	{
		if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;

		else if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(pSourPoint), XMLoadFloat3(&m_vPoints[POINT_B])))
	{
		if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;

		else if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(pSourPoint), XMLoadFloat3(&m_vPoints[POINT_C])))
	{
		if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;

		else if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector		vSour = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
		_vector		vDest = XMVector3Normalize(XMLoadFloat3(&m_vNormals[i]));

		if (0.0f < XMVectorGetX(XMVector3Dot(vSour, vDest)))
		{
			*pNeighborIndex = m_iNeighborIndex[i];
			return false;
		}
	}

	return true;
}

_vector CCell::ifOut(_fvector vPosition, _vector vDir)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector		vSour = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
		_vector		vDest = XMVector3Normalize(XMLoadFloat3(&m_vNormals[i]));

		vDest = XMVector3Normalize(vDest);		

		if (0.0f < XMVectorGetX(XMVector3Dot(vSour, vDest)))
		{			
			if (0.0f >= XMVectorGetX(XMVector3Dot(vDir, vDest)))
				return vDir;

			return (vDir + (-vDest * XMVector3Dot(vDir, vDest)));
		}
	}

	return vDir;
}

#ifdef _DEBUG

HRESULT CCell::Ready_Debug()
{
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

HRESULT CCell::Render_Debug(const _float4* pColor, _float fY)
{
	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());


	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	WorldMatrix._42 = fY;

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_RawValue("g_vColor", pColor, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	m_pShader->Begin(0);

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	return S_OK;
}

#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
#endif
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
