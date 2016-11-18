#include "HardwareSurface.h"
using namespace DirectX;
using std::vector;

int HardwareSurface::instanceCount = 0;
ID3D10Buffer* HardwareSurface::pD3D10IndexBuffer = NULL;
ID3D10Buffer* HardwareSurface::pD3D10VertexBuffer = NULL;
SOFTWARERASTERIZER_DX10_OBJECTS* localRasterizer;
HardwareSurface::HardwareSurface(int inWidth, int inHeight, int numFrames)
{
	localRasterizer = GetSoftwareRasterizer();

	//initialize the buffers on first run
	if (instanceCount == 0)
	{
		float aspectRatio = float(localRasterizer->clientRect.right) / float(localRasterizer->clientRect.top);
		float planeXOrig = -0.5f, planeYOrig = -0.5f;
		float planeWidth = 1.0f;
		float planeHeight = 1.0f;

		//create the VERTEX BUFFER  ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

		D3D10_BUFFER_DESC vbDesc;
		vbDesc.Usage = D3D10_USAGE_DYNAMIC;
		vbDesc.ByteWidth = sizeof(DX10VERTEX) * 4; //total size of buffer in bytes
		vbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vbDesc.MiscFlags = 0;

		if (FAILED(localRasterizer->pD3D10Device->CreateBuffer(&vbDesc, NULL, &pD3D10VertexBuffer))) return;


		//create the INDEX BUFFER   ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

		D3D10_BUFFER_DESC ibDesc;
		ibDesc.Usage = D3D10_USAGE_DYNAMIC;
		ibDesc.ByteWidth = sizeof(unsigned int) * 6; //6 indices for a square
		ibDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		ibDesc.MiscFlags = 0;

		if (FAILED(localRasterizer->pD3D10Device->CreateBuffer(&ibDesc, NULL, &pD3D10IndexBuffer))) return;


		//initialize the VERTEX BUFFER ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		UINT numVertices = 4;
		DX10VERTEX* v = NULL;

		//lock vertex buffer for CPU use
		pD3D10VertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&v);

		v[0] = DX10VERTEX(XMFLOAT3(planeXOrig, planeYOrig, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0.0f, 1.0f));
		v[1] = DX10VERTEX(XMFLOAT3(planeXOrig + planeWidth, planeYOrig, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(1.0f, 1.0f));
		v[2] = DX10VERTEX(XMFLOAT3(planeXOrig + planeWidth, planeYOrig + planeHeight, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(1.0f, 0.0f));
		v[3] = DX10VERTEX(XMFLOAT3(planeXOrig, planeYOrig + planeHeight, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0.0f, 0.0f));

		pD3D10VertexBuffer->Unmap();

		//initialize the INDEX BUFFER ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
		int* i = NULL;
		pD3D10IndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&i);

		i[0] = 0;
		i[1] = 3;
		i[2] = 2;
		i[3] = 2;
		i[4] = 1;
		i[5] = 0;

		pD3D10IndexBuffer->Unmap();

		//set up directly to clip projection 
		XMMATRIX m = XMMatrixSet(2.0f / localRasterizer->clientRect.right, 0.0f, -1.0f, 0.0f,
			0.0f, 2.0f / localRasterizer->clientRect.top, -1.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		//m._11 = 2.0f / localRasterizerObject->clientRect.right;
		//	m._13 = -1;
		//m._22 = 2.0f / localRasterizerObject->clientRect.bottom;
		//m._23 = -1;

		XMFLOAT4X4 mf;
		XMStoreFloat4x4(&mf, m);
		localRasterizer->pProjectionMatrixEffectVariable->SetMatrix((float*)mf.m);

	}

	//create our texture (or load it) ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = inWidth;
	desc.Height = inHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//softObjPtr->D3D10SwapChainDesc.BufferDesc.Format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	if (numFrames == 1)
		localRasterizer->pD3D10Device->CreateTexture2D(&desc, 0, &texture);
	else
	{
		for (int i = 0; i < numFrames; i++)
		{
			ID3D10Texture2D* tex;
			localRasterizer->pD3D10Device->CreateTexture2D(&desc, 0, &tex);
			textureFlipBook.push_back(tex);
		}

		texture = textureFlipBook[0];
	}

	//create intreface to shader kept texture
	localRasterizer->pD3D10Device->CreateShaderResourceView(texture, NULL, &textureSRV);
	textureSV = localRasterizer->pD3D10Effect->GetVariableByName("spriteTex")->AsShaderResource();
	textureSV->SetResource(textureSRV);
	currentFrame = 0;

	instanceCount++;
}
HardwareSurface::~HardwareSurface()
{
	if (pD3D10IndexBuffer) pD3D10IndexBuffer->Release();
	if (pD3D10VertexBuffer) pD3D10VertexBuffer->Release();
	instanceCount--;
}

void HardwareSurface::SetTransform(XMFLOAT4X4& mat)
{
	worldTransform = mat;
}

void HardwareSurface::Draw()
{

	if (textureFlipBook.size() > 1)
	{

		texture = textureFlipBook[currentFrame];
		currentFrame++;
		if (currentFrame == textureFlipBook.size())
		{
			currentFrame = 0;
		}

	}

	UINT stride = sizeof(DX10VERTEX);
	UINT offset = 0;

	ID3D10Buffer* lastVB, *lastIB;
	DXGI_FORMAT ibFormat;
	//save old buffers
	localRasterizer->pD3D10Device->IAGetVertexBuffers(0, 1, &lastVB, &stride, &offset);
	localRasterizer->pD3D10Device->IAGetIndexBuffer(&lastIB, &ibFormat, &offset);

	//set new ones
	localRasterizer->pD3D10Device->IASetVertexBuffers(0, 1, &pD3D10VertexBuffer, &stride, &offset);
	localRasterizer->pD3D10Device->IASetIndexBuffer(pD3D10IndexBuffer, DXGI_FORMAT_R32_UINT, offset);


	//set transform
	localRasterizer->pWorldMatrixEffectVariable->SetMatrix((float*)&worldTransform);

	D3D10_TECHNIQUE_DESC techDesc;
	localRasterizer->pD3D10EffectTechnique->GetDesc(&techDesc);

	//apply a pass, only the one correstponding to sprites
	localRasterizer->pD3D10EffectTechnique->GetPassByIndex(1)->Apply(0);

	//draw
	localRasterizer->pD3D10Device->DrawIndexed(6, 0, 0);


	//replace old buffers
	localRasterizer->pD3D10Device->IASetVertexBuffers(0, 1, &lastVB, &stride, &offset);
	localRasterizer->pD3D10Device->IASetIndexBuffer(lastIB, ibFormat, offset);


}

void HardwareSurface::LockFrame(int frameNum, DWORD** vid, int* lpitch32)
{


	D3D10_MAPPED_TEXTURE2D mappedTex;
	DWORD* texture_buffer;
	if (frameNum == 0)
		texture->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex);
	else
		textureFlipBook[frameNum]->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex);

	texture_buffer = (DWORD*)mappedTex.pData;
	ZeroMemory(texture_buffer, 100 * mappedTex.RowPitch);
	*vid = texture_buffer;
	*lpitch32 = mappedTex.RowPitch >> 2;;
}