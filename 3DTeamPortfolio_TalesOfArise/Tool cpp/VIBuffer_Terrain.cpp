#include "ViBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	:CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, vVIBufferPos(rhs.vVIBufferPos)	
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapfilePath)
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Failed To Create Terrain : Arguements is nullptr");
		return E_FAIL;
	}

	TERRAINDESC TerrainDesc;
	ZeroMemory(&TerrainDesc, sizeof TERRAINDESC);
	memcpy(&TerrainDesc, pArg, sizeof TERRAINDESC);	

	m_iNumVerticesX = _uint(TerrainDesc.iTerrainSizeX * 1.0f / TerrainDesc.fVerticesDestance);
	m_iNumVerticesZ = _uint(TerrainDesc.iTerrainSizeY * 1.0f / TerrainDesc.fVerticesDestance);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_fVerticesDest = TerrainDesc.fVerticesDestance;

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = sizeof(_ulong);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	vVIBufferPos = new _float4[m_iNumVertices];
	
	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = _float3(((_float)j * TerrainDesc.fVerticesDestance), 0.f, ((_float)i * TerrainDesc.fVerticesDestance));
			vVIBufferPos[iIndex] = _float4(pVertices[iIndex].vPosition.x, pVertices[iIndex].vPosition.y, pVertices[iIndex].vPosition.z, 1.f);
			pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.0f), i / (m_iNumVerticesZ - 1.0f));
		}
	}	

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	_vector		vSour, vDest, vNormal;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			_uint	iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices] = iIndices[0];
			pIndices[iNumIndices + 1] = iIndices[1];
			pIndices[iNumIndices + 2] = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[pIndices[iNumIndices + 1]].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumIndices]].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumIndices + 2]].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumIndices + 1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumIndices]].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumIndices]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumIndices + 1]].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumIndices + 1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumIndices + 2]].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumIndices + 2]].vNormal) + vNormal);
			iNumIndices += 3;

			pIndices[iNumIndices] = iIndices[0];
			pIndices[iNumIndices + 1] = iIndices[2];
			pIndices[iNumIndices + 2] = iIndices[3];
			vSour = XMLoadFloat3(&pVertices[pIndices[iNumIndices + 1]].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumIndices]].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumIndices + 2]].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumIndices + 1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumIndices]].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumIndices]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumIndices + 1]].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumIndices + 1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumIndices + 2]].vNormal,
				XMLoadFloat3(&pVertices[pIndices[iNumIndices + 2]].vNormal) + vNormal);
			iNumIndices += 3;
		}
	}

	for (_uint i = 0; i < m_iNumVertices; ++i)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pVB)))
		return E_FAIL;	

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pIndices;
	
	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(const char* pFilePath)
{
	ifstream readFile;
	readFile.open(pFilePath, ios::in | ios::binary);

	char buf[MAX_PATH];

	_float4x4 matPivot;

	readFile.read(buf, sizeof(_float4x4));
	memcpy(&matPivot, buf, sizeof(_float4x4));

	return S_OK;
}

void CVIBuffer_Terrain::Tick(_double TimeDelta, _uint iIndex, _float fRadian, _float fSpeed)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;	

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_uint Range = _uint(fRadian / m_fVerticesDest);	

	_uint StartIndex = iIndex - ((m_iNumVerticesX + 1) * (Range / 2));
	_uint EndIndex = iIndex + ((m_iNumVerticesX + 1) * (Range / 2));

	_vector vSour, vDest, vNormal;		

	_float** fUpperSpeed = new _float* [Range + 1];

	for (_uint i = 0; i < Range + 1; ++i) {
		fUpperSpeed[i] = new _float[Range + 1];
	}	

	for (_uint i = 0; i < Range + 1; ++i)
	{
		for (_uint j = 0; j < Range + 1; ++j)
		{
			_uint iRandeIndex = StartIndex + (m_iNumVerticesX * i) + j;
			
			_uint distanceFromCenter = std::abs(_int(i - ((Range / 2)))) + std::abs(_int(j - ((Range / 2))));
			fUpperSpeed[i][j] = fSpeed - (fSpeed / Range) * distanceFromCenter;
			
			if (m_iNumVertices <= iRandeIndex)
				break;

			((VTXNORTEX*)SubResource.pData)[iRandeIndex].vPosition.y += fUpperSpeed[i][j] * TimeDelta;

			_uint	iIndices[4] = {
				iRandeIndex + m_iNumVerticesX,
				iRandeIndex + m_iNumVerticesX + 1,
				iRandeIndex + 1,
				iRandeIndex
			};

			vSour = XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[1]].vPosition) - XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[2]].vPosition) - XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[0]].vNormal,
				XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[0]].vNormal) + vNormal);

			XMStoreFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[1]].vNormal,
				XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[1]].vNormal) + vNormal);

			XMStoreFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[2]].vNormal,
				XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[2]].vNormal) + vNormal);

			vSour = XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[3]].vPosition) - XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[2]].vPosition);
			vDest = XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[0]].vPosition) - XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[3]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[0]].vNormal,
				XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[0]].vNormal) + vNormal);

			XMStoreFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[2]].vNormal,
				XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[2]].vNormal) + vNormal);

			XMStoreFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[3]].vNormal,
				XMLoadFloat3(&((VTXNORTEX*)SubResource.pData)[iIndices[3]].vNormal) + vNormal);		
		}
	}	

	for (_uint i = 0; i < Range + 1; ++i) {
		Safe_Delete(fUpperSpeed[i]);
	}

	Safe_Delete_Array(fUpperSpeed);	

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		vVIBufferPos[i] = _float4(((VTXNORTEX*)SubResource.pData)[i].vPosition.x, ((VTXNORTEX*)SubResource.pData)[i].vPosition.y, ((VTXNORTEX*)SubResource.pData)[i].vPosition.z, 1.f);
	}

	m_pContext->Unmap(m_pVB, 0);

	
}

CVIBuffer_Terrain::PICKINGDESC* CVIBuffer_Terrain::Get_PickingRange(_uint iIndex, _float fRadian)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_IndexList.clear();

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_uint Range = _uint(fRadian / m_fVerticesDest);

	m_iNumRange = Range;

	_uint StartIndex = iIndex - ((m_iNumVerticesX + 1) * (Range / 2));
	_uint EndIndex = iIndex + ((m_iNumVerticesX + 1) * (Range / 2));	

	_int iOrigin_i, iOrigin_j;	

	for (_uint i = 0; i < m_iNumRange; ++i)
	{
		for (_uint j = 0; j < m_iNumRange; ++j)
		{
			iOrigin_i = abs((_int)i - (_int)m_iNumRange / 2);
			iOrigin_j = abs((_int)j - (_int)m_iNumRange / 2);

			if ((fRadian / 2) * (fRadian / 2) < iOrigin_i * iOrigin_i + iOrigin_j * iOrigin_j)
				continue;

			_uint iRandeIndex = StartIndex + (m_iNumVerticesX * i) + j;	

			if (m_iNumVertices <= iRandeIndex)
				break;			

			m_IndexList.push_back(iRandeIndex);			
		}
	}

	if (m_iDescSize != m_IndexList.size())
	{
		m_iDescSize = m_IndexList.size();

		Safe_Delete_Array(m_tPickingDesc);
		m_tPickingDesc = nullptr;

		m_tPickingDesc = new PICKINGDESC[m_iDescSize];
	}

	_uint index = 0;
	
	for(auto& iter : m_IndexList)
	{
		m_tPickingDesc[index].vVBPos = ((VTXNORTEX*)SubResource.pData)[iter].vPosition;
		m_tPickingDesc[index].vVBNormal = ((VTXNORTEX*)SubResource.pData)[iter].vNormal;

		++index;
	}

	m_pContext->Unmap(m_pVB, 0);

	return m_tPickingDesc;
}

void CVIBuffer_Terrain::Save_Terrain(ofstream& writeFile)
{
	if (writeFile.is_open())
	{
		write_typed_data(writeFile, &m_iNumVerticesX, sizeof(_uint));
		write_typed_data(writeFile, &m_iNumVerticesZ, sizeof(_uint));
		write_typed_data(writeFile, &m_iNumVertices, sizeof(_uint));		
		write_typed_data(writeFile, &m_fVerticesDest, sizeof(_float));
		write_typed_data(writeFile, &m_iNumIndices, sizeof(_uint));
		

		D3D11_MAPPED_SUBRESOURCE		SubResource;

		m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		for (_uint i = 0; i < m_iNumVertices; ++i)
		{
			write_typed_data(writeFile, &((VTXNORTEX*)SubResource.pData)[i].vPosition, sizeof(_float3));
			write_typed_data(writeFile, &((VTXNORTEX*)SubResource.pData)[i].vNormal, sizeof(_float3));
			write_typed_data(writeFile, &((VTXNORTEX*)SubResource.pData)[i].vTexUV, sizeof(_float2));		
		}

		m_pContext->Unmap(m_pVB, 0);	

		ZeroMemory(&SubResource, sizeof D3D11_MAPPED_SUBRESOURCE);

		m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		_uint i = 0;
		for (i = 0; i < m_iNumIndices; ++i)
		{
			write_typed_data(writeFile, &((_ulong*)SubResource.pData)[i], sizeof(_ulong));			
		}

		m_pContext->Unmap(m_pIB, 0);

		writeFile.close();			
	}	
}

void CVIBuffer_Terrain::Load_Terrain(ifstream& readFile)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	Safe_Delete_Array(vVIBufferPos);

	char buf[MAX_PATH];

	readFile.read(buf, sizeof(_uint));
	memcpy(&m_iNumVerticesX, buf, sizeof(_uint));

	readFile.read(buf, sizeof(_uint));
	memcpy(&m_iNumVerticesZ, buf, sizeof(_uint));

	readFile.read(buf, sizeof(_uint));
	memcpy(&m_iNumVertices, buf,sizeof(_uint));
		
	readFile.read(buf, sizeof(_float));
	memcpy(&m_fVerticesDest, buf, sizeof(_float));

	readFile.read(buf, sizeof(_uint));
	memcpy(&m_iNumIndices, buf, sizeof(_uint));
	
	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = sizeof(_ulong);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	vVIBufferPos = new _float4[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		readFile.read(buf, sizeof(_float3));
		memcpy(&pVertices[i].vPosition, buf, sizeof(_float3));
				
		vVIBufferPos[i] = _float4(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z, 1.f);

		readFile.read(buf, sizeof(_float3));
		memcpy(&pVertices[i].vNormal, buf, sizeof(_float3));

		readFile.read(buf, sizeof(_float2));
		memcpy(&pVertices[i].vTexUV, buf, sizeof(_float2));

	}	

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pVB)))
	{
		MSG_BOX("Load Fail! : VertexBuffer Create Fail");
		return;
	}

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		readFile.read(buf, sizeof(_ulong));
		memcpy(&pIndices[i], buf, sizeof(_ulong));
	}

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pIB)))
	{
		MSG_BOX("Load Fail! : IndexBuffer Create Fail");
		return;
	}

	Safe_Delete_Array(pIndices);
	Safe_Delete_Array(pVertices);

}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapfilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapfilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{		
	Safe_Delete_Array(vVIBufferPos);		

	if (nullptr != m_tPickingDesc)
	{
		Safe_Delete_Array(m_tPickingDesc);
		m_tPickingDesc = nullptr;
	}

	__super::Free();
	
}