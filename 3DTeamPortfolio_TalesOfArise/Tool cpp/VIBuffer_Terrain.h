#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct TerrainDesc
	{
		_bool isLoadFile;
		char szBinFileName[MAX_PATH];
		_uint iTerrainSizeX;
		_uint iTerrainSizeY;
		_float fVerticesDestance;

	}TERRAINDESC;

	typedef struct TerrainPickignDesc
	{
		_float3 vVBPos;
		_float3 vVBNormal;
	}PICKINGDESC;

private:
	CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapfilePath);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Initialize(const char* pFilePath);
	void Tick(_double TimeDelta, _uint iIndex, _float fRadian, _float fSpeed);
	_float4* Get_TerrainBuffer() { return vVIBufferPos; }	
	_uint Get_VtxCntX() { return m_iNumVerticesX; };
	_uint Get_VtxCntZ() { return m_iNumVerticesZ; };

	PICKINGDESC* Get_PickingRange(_uint iIndex, _float fRadian);

public:
	void Save_Terrain(ofstream& writeFile);
	void Load_Terrain(ifstream& readFile);

private:
	_uint m_iNumVerticesX = { 0 };
	_uint m_iNumVerticesZ = { 0 };
	_float m_fVerticesDest = { 1.f };

private:
	_float4* vVIBufferPos = { nullptr };	
	_float** pUpperSpeed = { nullptr };

	PICKINGDESC* m_tPickingDesc = { nullptr };
	_uint m_iNumRange = { 0 };

private:
	list<_uint> m_IndexList;
	_uint m_iDescSize = { 0 };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapfilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END