#include "SoftwareRasterizer.h"

//private pointer to the main drawing engine
SOFTWARERASTERIZER_DX10_OBJECTS* SoftwareRasterizerSelfPointer;

//queue primitives
vector<LinePrimitive> lineQueue;

UINT numVertexElements = 3;
D3D10_INPUT_ELEMENT_DESC DX10VertexLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 }
};

DWORD* Viewport::TranslateMemory(DWORD* mem, int lpitch32)
{
	return mem + int(Rect.GetPosition().x) + lpitch32*int(Rect.GetPosition().y); 
}


SOFTWARERASTERIZER_DX10_OBJECTS* GetSoftwareRasterizer()
{
	return SoftwareRasterizerSelfPointer;
}

void QueueLine(float x0, float y0, float x1, float y1, DWORD color0, DWORD color1)
{
	LinePrimitive l;
	l.point0 = VECTOR2D(x0, y0);
	l.point1 = VECTOR2D(x1, y1);
	l.color0 = color0;
	l.color1 = color1;
	lineQueue.push_back(l);
}

void ClearLineQueue()
{
	lineQueue.clear();
}

void DrawLineQueue(DWORD* mem, int lpitch32)
{
	vector<LinePrimitive>::iterator vIter;
	for (vIter = lineQueue.begin(); vIter < lineQueue.end(); vIter++)
	{
		DrawLine(mem, lpitch32, (*vIter).point0.x, (*vIter).point0.y, (*vIter).point1.x, (*vIter).point1.y, (*vIter).color0, (*vIter).color1);
	}

	lineQueue.clear();
}

//using DX7 and direct draw
void InitializeSoftwareRasterizer(SOFTWARERASTERIZER_DX7_OBJECTS* softObjPtr)
{

	softObjPtr->lpDDraw7Interface = 0;

	// create IDirectDraw interface 7.0 object and test for error
	if (FAILED(DirectDrawCreateEx(NULL, (void **)&(softObjPtr->lpDDraw7Interface), IID_IDirectDraw7, NULL)))
	{
		//report
		return;
	}


	// set cooperation level with windows, Windows7 & 8 require DDSCL_NORMAL, don't work at all in DDSCL_ESCLUSIVE
	if (FAILED(softObjPtr->lpDDraw7Interface->SetCooperativeLevel(softObjPtr->mainWindow, DDSCL_NORMAL)))
	{
		//report
		return;
	}

	// create the primary surface
	softObjPtr->lpDDSurfPrimary = 0;
	memset(&softObjPtr->ddSurfDesc, 0, sizeof(softObjPtr->ddSurfDesc));
	softObjPtr->ddSurfDesc.dwSize = sizeof(softObjPtr->ddSurfDesc);
	softObjPtr->ddSurfDesc.dwFlags = DDSD_CAPS;
	softObjPtr->ddSurfDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (FAILED(softObjPtr->lpDDraw7Interface->CreateSurface(&softObjPtr->ddSurfDesc, &softObjPtr->lpDDSurfPrimary, NULL)))
	{
		//failed to create surface throw error
		//return 0; //don't return in debug, easier to catch error with access violations
		return;
	}


	//create back buffer manualy, since double buffer chain is disable in windowed mode
	softObjPtr->lpDDSurfBack = 0;
	DDRAW_INIT_STRUCT(softObjPtr->ddPixelFormat);
	softObjPtr->lpDDSurfPrimary->GetPixelFormat(&softObjPtr->ddPixelFormat);


}
void ShutdownSoftwareRasterizer(SOFTWARERASTERIZER_DX7_OBJECTS* softObjPtr)
{
	// now the primary surface
	if (softObjPtr->lpDDSurfPrimary)
	{
		softObjPtr->lpDDSurfPrimary->Release();
		softObjPtr->lpDDSurfPrimary = NULL;
	}

	//take care of the other surfaces
	if (softObjPtr->lpDDSurfBack)
	{
		softObjPtr->lpDDSurfBack->Release();
		softObjPtr->lpDDSurfBack = NULL;
	}

	// now blow away the IDirectDraw7 interface
	if (softObjPtr->lpDDraw7Interface)
	{
		softObjPtr->lpDDraw7Interface->Release();
		softObjPtr->lpDDraw7Interface = NULL;
	}
}

void InitializeSoftwareRasterizer(SOFTWARERASTERIZER_DX10_OBJECTS* softObjPtr)
{


	//setup swap chain ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	ZeroMemory(&softObjPtr->D3D10SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	//set buffer dimensions and format
	softObjPtr->D3D10SwapChainDesc.BufferCount = 2;
	softObjPtr->D3D10SwapChainDesc.BufferDesc.Width = softObjPtr->clientRect.right;
	softObjPtr->D3D10SwapChainDesc.BufferDesc.Height = softObjPtr->clientRect.bottom;
	softObjPtr->D3D10SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	softObjPtr->D3D10SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//set refresh rate
	softObjPtr->D3D10SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	softObjPtr->D3D10SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	//sampling settings
	softObjPtr->D3D10SwapChainDesc.SampleDesc.Quality = 0;
	softObjPtr->D3D10SwapChainDesc.SampleDesc.Count = 1;

	//output window handle
	softObjPtr->D3D10SwapChainDesc.OutputWindow = softObjPtr->mainWindow;
	softObjPtr->D3D10SwapChainDesc.Windowed = true;

	//create device ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	HRESULT hr1;
	if (FAILED(hr1 = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG , D3D10_SDK_VERSION, &softObjPtr->D3D10SwapChainDesc, &softObjPtr->pD3D10SwapChain, &softObjPtr->pD3D10Device)))
	{
		ZeroMemory(&softObjPtr->D3D10SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		return;
	}

	//get the info interface
	softObjPtr->pD3D10Device->QueryInterface(__uuidof(ID3D10InfoQueue), (void**)&softObjPtr->pD3D10InfoQueue);

	// create render target for merger state ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	ID3D10Texture2D* pBackBuffer;
	if (FAILED(softObjPtr->pD3D10SwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer))) return;

	//try to create render target view
	if (FAILED(softObjPtr->pD3D10Device->CreateRenderTargetView(pBackBuffer, NULL, &softObjPtr->pD3D10RenderTargetView))) return;

	//release the back buffer
	pBackBuffer->Release();

	//set the render target
	softObjPtr->pD3D10Device->OMSetRenderTargets(1, &softObjPtr->pD3D10RenderTargetView, NULL);


	//set view port aka region of render target ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	softObjPtr->D3D10Viewport.Width = softObjPtr->D3D10SwapChainDesc.BufferDesc.Width;
	softObjPtr->D3D10Viewport.Height = softObjPtr->D3D10SwapChainDesc.BufferDesc.Height;
	softObjPtr->D3D10Viewport.MinDepth = 0.0f;
	softObjPtr->D3D10Viewport.MaxDepth = 1.0f;
	softObjPtr->D3D10Viewport.TopLeftX = 0;
	softObjPtr->D3D10Viewport.TopLeftY = 0;

	softObjPtr->pD3D10Device->RSSetViewports(1, &softObjPtr->D3D10Viewport);

	//set up matrices ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓


	//compute the ratio of screen dimensions

	float aspectRatio = float(softObjPtr->clientRect.left) / float(softObjPtr->clientRect.bottom);
	float planeXOrig = -1.0f, planeYOrig = -1.0f;
	float planeWidth = 2.0f;
	float planeHeight = 2.0f;


	//orthographic
	XMMATRIX xmatT = XMMatrixIdentity();
	XMStoreFloat4x4(&softObjPtr->matView, xmatT);


	//load the effects file to be used ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

	HRESULT hrFx;
	if (FAILED(hrFx = D3DX10CreateEffectFromFile(TEXT("D:\\Users\\Tim\\Documents\\Visual Studio Projects\\SoftwareRasterizer\\Common\\DX10RendererEffect.fx"), NULL, NULL, "fx_4_0", D3D10_SHADER_DEBUG, 0, softObjPtr->pD3D10Device, NULL, NULL, &softObjPtr->pD3D10Effect, NULL, NULL)))
	{
		return;
	}


	//create matrix effect pointers
	softObjPtr->pViewMatrixEffectVariable = softObjPtr->pD3D10Effect->GetVariableByName("View")->AsMatrix();
	softObjPtr->pProjectionMatrixEffectVariable = softObjPtr->pD3D10Effect->GetVariableByName("Projection")->AsMatrix();
	softObjPtr->pWorldMatrixEffectVariable = softObjPtr->pD3D10Effect->GetVariableByName("World")->AsMatrix();


	//create input layout ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	D3D10_PASS_DESC PassDesc;
	softObjPtr->pD3D10EffectTechnique = softObjPtr->pD3D10Effect->GetTechniqueByName("Render");
	softObjPtr->pD3D10EffectTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);

	ID3D10InputLayout* pInputLayout;
	if (FAILED(softObjPtr->pD3D10Device->CreateInputLayout(DX10VertexLayout, numVertexElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout))) return;

	softObjPtr->pD3D10Device->IASetInputLayout(pInputLayout);

	//create the VERTEX BUFFER for the plane that we'll be drawing on ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	softObjPtr->numVertices = 4;

	D3D10_BUFFER_DESC vbDesc;
	vbDesc.Usage = D3D10_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(DX10VERTEX)* softObjPtr->numVertices; //total size of buffer in bytes
	vbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vbDesc.MiscFlags = 0;

	if (FAILED(softObjPtr->pD3D10Device->CreateBuffer(&vbDesc, NULL, &softObjPtr->pD3D10VertexBuffer))) return;

	// Set vertex buffer
	UINT stride = sizeof(DX10VERTEX);
	UINT offset = 0;
	softObjPtr->pD3D10Device->IASetVertexBuffers(0, 1, &softObjPtr->pD3D10VertexBuffer, &stride, &offset);

	//create the INDEX BUFFER for the plane ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	softObjPtr->numIndices = 6; //2 triangles (3 verts each) for a plane


	D3D10_BUFFER_DESC ibDesc;
	ibDesc.Usage = D3D10_USAGE_DYNAMIC;
	ibDesc.ByteWidth = sizeof(unsigned int)*softObjPtr->numIndices;
	ibDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	ibDesc.MiscFlags = 0;

	if (FAILED(softObjPtr->pD3D10Device->CreateBuffer(&ibDesc, NULL, &softObjPtr->pD3D10IndexBuffer))) return;

	softObjPtr->pD3D10Device->IASetIndexBuffer(softObjPtr->pD3D10IndexBuffer, DXGI_FORMAT_R32_UINT, offset);

	//initialize the VERTEX BUFFER ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	UINT numVertices = 4;
	DX10VERTEX* v = NULL;

	//lock vertex buffer for CPU use
	softObjPtr->pD3D10VertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&v);

	v[0] = DX10VERTEX(XMFLOAT3(planeXOrig, planeYOrig, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0.0f, 1.0f));
	v[1] = DX10VERTEX(XMFLOAT3(planeXOrig + planeWidth, planeYOrig, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(1.0f, 1.0f));
	v[2] = DX10VERTEX(XMFLOAT3(planeXOrig + planeWidth, planeYOrig + planeHeight, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(1.0f, 0.0f));
	v[3] = DX10VERTEX(XMFLOAT3(planeXOrig, planeYOrig + planeHeight, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0.0f, 0.0f));

	softObjPtr->pD3D10VertexBuffer->Unmap();

	//initialize the INDEX BUFFER ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	int* i = NULL;
	softObjPtr->pD3D10IndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&i);

	i[0] = 0;
	i[1] = 1;
	i[2] = 2;
	i[3] = 0;
	i[4] = 2;
	i[5] = 3;

	softObjPtr->pD3D10IndexBuffer->Unmap();

	//set up rasterizer flags ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	D3D10_RASTERIZER_DESC rasterizerState;
	rasterizerState.CullMode = D3D10_CULL_NONE;
	rasterizerState.FillMode = D3D10_FILL_SOLID;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = true;

	ID3D10RasterizerState* pRS;
	softObjPtr->pD3D10Device->CreateRasterizerState(&rasterizerState, &pRS);
	softObjPtr->pD3D10Device->RSSetState(pRS);

	softObjPtr->pD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//set the matrices in the effect
	//set effect matrices
	XMMATRIX xmatTWorld = XMMatrixIdentity();
	XMStoreFloat4x4(&softObjPtr->matWorld, xmatTWorld);
	//D3DXMatrixIdentity(&softObjPtr->matWorld);

	softObjPtr->pWorldMatrixEffectVariable->SetMatrix((float*)&softObjPtr->matWorld);
	softObjPtr->pViewMatrixEffectVariable->SetMatrix((float*)&softObjPtr->matView);
	softObjPtr->pProjectionMatrixEffectVariable->SetMatrix((float*)&softObjPtr->matProjection);

	//create our texture (or load it) ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = softObjPtr->D3D10SwapChainDesc.BufferDesc.Width;
	desc.Height = softObjPtr->D3D10SwapChainDesc.BufferDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//softObjPtr->D3D10SwapChainDesc.BufferDesc.Format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;


	HRESULT hr;
	if (FAILED(hr = softObjPtr->pD3D10Device->CreateTexture2D(&desc, 0, &softObjPtr->texture)))
	{
		return;
	}

	//create shader resource view for the texture ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
	/*	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;
	*/
	/*
	//create a sampler
	D3D10_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D10_SAMPLER_DESC));
	samplerDesc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
	softObjPtr->pD3D10Device->CreateSamplerState(&samplerDesc, &softObjPtr->pD3D10TextureSampler);
	softObjPtr->pD3D10Device->PSSetSamplers(0, 1, &softObjPtr->pD3D10TextureSampler);
	*/

	softObjPtr->pD3D10Device->CreateShaderResourceView(softObjPtr->texture, NULL, &softObjPtr->textureSRV);
	softObjPtr->textureSV = softObjPtr->pD3D10Effect->GetVariableByName("tex2D")->AsShaderResource();
	softObjPtr->textureSV->SetResource(softObjPtr->textureSRV);


	//set up alpha blending for transparent textures ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	ID3D10BlendState* pBlendState = NULL;

	D3D10_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D10_BLEND_DESC));

	BlendState.BlendEnable[0] = TRUE;
	BlendState.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	BlendState.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	BlendState.BlendOp = D3D10_BLEND_OP_ADD;
	BlendState.SrcBlendAlpha = D3D10_BLEND_ZERO;
	BlendState.DestBlendAlpha = D3D10_BLEND_ZERO;
	BlendState.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	BlendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

	softObjPtr->pD3D10Device->CreateBlendState(&BlendState, &pBlendState);
	softObjPtr->pD3D10Device->OMSetBlendState(pBlendState, 0, 0xffffffff);


	//	softObjPtr->pD3D10Device->PSSetShaderResources(0, 1, &softObjPtr->textureSRV);

	softObjPtr->drawOffsetX = softObjPtr->drawOffsetY = 0.0f;

	SoftwareRasterizerSelfPointer = softObjPtr;


}
void ShutdownSoftwareRasterizer(SOFTWARERASTERIZER_DX10_OBJECTS* softObjPtr)
{
	if (softObjPtr->pD3D10Effect) softObjPtr->pD3D10Effect->Release();
	if (softObjPtr->pD3D10IndexBuffer) softObjPtr->pD3D10IndexBuffer->Release();
	if (softObjPtr->pD3D10VertexBuffer) softObjPtr->pD3D10VertexBuffer->Release();
	if (softObjPtr->pD3D10RenderTargetView) softObjPtr->pD3D10RenderTargetView->Release();
	if (softObjPtr->pD3D10SwapChain) softObjPtr->pD3D10SwapChain->Release();
	if (softObjPtr->pD3D10Device) softObjPtr->pD3D10Device->Release();

	softObjPtr->pD3D10Effect = NULL;
	softObjPtr->pD3D10IndexBuffer = NULL;
	softObjPtr->pD3D10VertexBuffer = NULL;
	softObjPtr->pD3D10RenderTargetView = NULL;
	softObjPtr->pD3D10SwapChain = NULL;
	softObjPtr->pD3D10Device = NULL;
}


void InitializeSoftwareRasterizer(SOFTWARERASTERIZER_DX12_OBJECTS* softObjPtr)
{
	/*

	//enable debug layer
#ifdef _DEBUG
{
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
}
#endif

	ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory1(IID_PPV_ARGS(&factory));
	ComPtr<IDXGIAdapter1> hardwareAdapter;
	for (UINT index = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(index, &hardwareAdapter); ++index)
	{
		DXGI_ADAPTER_DESC1 desc;
		hardwareAdapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		//check if there is Direct3D 12 support, then create
		if (SUCCEEDED(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&softObjPtr->pD3D12Device))))
		{
			break;
		}
	}


	//describe and create the command queue 
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	softObjPtr->pD3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&softObjPtr->pD3D12CommandQueue));

	//describe and create the swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = softObjPtr->clientRect.right - softObjPtr->clientRect.left;
	swapChainDesc.BufferDesc.Height = softObjPtr->clientRect.bottom - softObjPtr->clientRect.top;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = softObjPtr->mainWindow;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;


	ComPtr<IDXGISwapChain> swapChain;
	factory->CreateSwapChain(softObjPtr->pD3D12CommandQueue.Get(), &swapChainDesc, &swapChain);
	factory->MakeWindowAssociation(softObjPtr->mainWindow, DXGI_MWA_NO_ALT_ENTER);
	swapChain.As(&softObjPtr->pD3D12SwapChain);
	softObjPtr->frameIndex = softObjPtr->pD3D12SwapChain->GetCurrentBackBufferIndex();

	//create descriptor heaps
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	softObjPtr->pD3D12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&softObjPtr->pD3D12RTVHeap));

	softObjPtr->rtvDescriptorSize = softObjPtr->pD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	//create frame resources
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = softObjPtr->pD3D12RTVHeap->GetCPUDescriptorHandleForHeapStart();
	//create a rtv for each frame
	for (UINT n = 0; n < 2; n++)
	{
		softObjPtr->pD3D12SwapChain->GetBuffer(n, IID_PPV_ARGS(&softObjPtr->pD3D12RenderTargets[n]));
		softObjPtr->pD3D12Device->CreateRenderTargetView(softObjPtr->pD3D12RenderTargets[n].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += softObjPtr->rtvDescriptorSize;
	}

	softObjPtr->pD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&softObjPtr->pD3D12CommandAllocator));

	//create empty root signature
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.NumParameters = 0;
	rootSigDesc.pParameters = nullptr;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	softObjPtr->pD3D12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&softObjPtr->pD3D12RootSignature));

	//Create a pipiline stage which includes loading and compiling shaders
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

	#ifdef _DEBUG
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#else
	UINT compileFlags = 0;
	#endif

//	D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
//	D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);


	//vertex input layout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	//describe and create the graphics pipeline state object (PSO) and it's constituents
	D3D12_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.MultisampleEnable = FALSE;
	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.ForcedSampleCount = 0;
	rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;

	D3D12_RENDER_TARGET_BLEND_DESC defRTBlendDesc =
	{
		FALSE, FALSE,
		D3D12_BLEND_ONE,D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE,D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};

	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		blendDesc.RenderTarget[i] = defRTBlendDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = softObjPtr->pD3D12RootSignature.Get();
	psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	psoDesc.RasterizerState = rasterDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc.Count = 1;

	softObjPtr->pD3D12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&softObjPtr->pD3D12PipelineState));



	//create the command list
	softObjPtr->pD3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, softObjPtr->pD3D12CommandAllocator.Get(), softObjPtr->pD3D12PipelineState.Get(), IID_PPV_ARGS(&softObjPtr->pD3D12CommandList));
	softObjPtr->pD3D12CommandList->Close();

	//create the vertex buffer
	float aspectRatio = 1.9f;
	DX12VERTEX triVertices[] =
	{
		{ { 0.0f, 0.25f * aspectRatio, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f }, {0.0f, 0.0f} },
		{ { 0.25f, -0.25f * aspectRatio, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f }, {0.0f, 0.0f} },
		{ { -0.25f, -0.25f * aspectRatio, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f }, {0.0f, 0.0f} }
	};

	const UINT vertexBufferSize = sizeof(triVertices);

	// Note: using upload heaps to transfer static data like vert buffers is not 
	// recommended. Every time the GPU needs it, the upload heap will be marshalled 
	// over. Please read up on Default Heap usage. An upload heap is used here for 
	// code simplicity and because there are very few verts to actually transfer.
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = vertexBufferSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	softObjPtr->pD3D12Device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&softObjPtr->pD3D12VertexBuffer));

	//load the vb
	unsigned char* pVertexDataBegin;
	softObjPtr->pD3D12VertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, triVertices, vertexBufferSize);
	softObjPtr->pD3D12VertexBuffer->Unmap(0, nullptr);

	//initial vb view
	softObjPtr->d3d12VertexBufferView.BufferLocation = softObjPtr->pD3D12VertexBuffer->GetGPUVirtualAddress();
	softObjPtr->d3d12VertexBufferView.StrideInBytes = sizeof(DX12VERTEX);
	softObjPtr->d3d12VertexBufferView.SizeInBytes = vertexBufferSize;

	//create synchronization objects and wait until assets have been uploaded to the GPU
	softObjPtr->pD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&softObjPtr->pD3D12Fence));
	softObjPtr->fenceValue = 1;

	//Create an event handle to use for frame synchronization
	softObjPtr->hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (softObjPtr->hFenceEvent == nullptr)
	{
		//fail
		return;
	}

	//Wait for the command list to execute; we are reusing the smae command list in our main loop 
	//but for now we just want to wait for setup to complete before continuing

	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity.

	const UINT64 fence = softObjPtr->fenceValue;
	softObjPtr->pD3D12CommandQueue->Signal(softObjPtr->pD3D12Fence.Get(), fence);
	softObjPtr->fenceValue++;

	//Wait until the previous frame is finished
	if (softObjPtr->pD3D12Fence->GetCompletedValue() < fence)
	{
		softObjPtr->pD3D12Fence->SetEventOnCompletion(fence, softObjPtr->hFenceEvent);
		WaitForSingleObject(softObjPtr->hFenceEvent, INFINITE);
	}

	softObjPtr->frameIndex = softObjPtr->pD3D12SwapChain->GetCurrentBackBufferIndex();
	*/

}
void ShutdownSoftwareRasterizer(SOFTWARERASTERIZER_DX12_OBJECTS* softObjPtr)
{

}


void IncrementDrawOffset(float x, float y)
{
	SoftwareRasterizerSelfPointer->drawOffsetX += x;
	SoftwareRasterizerSelfPointer->drawOffsetY += y;
}


//bitwise extraction of the encoded color bytes
void GetRGBA32FromDWORD(int& r, int& g, int& b, int& a, DWORD color)
{
	a = (color >> 24);
	r = (color << 24) >> 24;
	g = (color << 16 ) >> 24; 
	b = (color << 8) >> 24;
}




void SortVerticesByY(VERTEX2D verts[], int num)
{


	bool isSwapped;

	do
	{
		isSwapped = false;
		VERTEX2D temp;
		for(int i = 0; i < num; i++)
		{
			if(i+1 == num)
				break;
			 
			if((verts[i+1].pos.y > verts[i].pos.y))
			{
				temp = verts[i];
				verts[i] = verts[i+1];
				verts[i + 1] = temp;
				isSwapped = true;
			}
			

		}
		

	}while(isSwapped);

	
	//sort in x direction also
	if(verts[0].pos.y == verts[1].pos.y) //flat bottom
	{
		if(verts[0].pos.x > verts[1].pos.x)
		{
			VERTEX2D t = verts[0];
			verts[0] = verts[1];
			verts[1] = t;
		}
	}
	if(verts[1].pos.y == verts[2].pos.y) //flat top
	{
		if(verts[1].pos.x > verts[2].pos.x)
		{
			VERTEX2D t = verts[1];
			verts[1] = verts[2];
			verts[2] = t;
		}
	}
}


//flips bitmap in the proces
void BlitBitmapToMemory(DWORD* surfMem, DWORD* bitmapMem, int width, int height)
{
	//flip the bitmap, start with last row and copy to first
	bitmapMem += width*height - width;
	for(int row = 0; row < height; row++)
	{
		memcpy(surfMem, bitmapMem, sizeof(DWORD)*width); //blit one line
		surfMem += width;
		bitmapMem -= width;
	}
}

void CreateOffscreenSurfaceDX7(LPDIRECTDRAW7 ddInterface, LPDIRECTDRAWSURFACE7* outSurface, int width, int height, DDPIXELFORMAT pixFormat)
{
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);

	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
	ddsd.ddpfPixelFormat = pixFormat;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

	ddInterface->CreateSurface(&ddsd, outSurface, NULL);

}


//misc, move this~
void MakeListFromSelected(MESHOBJECT_PTR obj, RECT selectArea);
void CheckHighlightMouseHover();
void CheckCollisions();




void DrawLine(DWORD* buffer, int lpitch32, int x0, int y0, int x1, int y1, DWORD color0, DWORD color1, RECT* clipRect)
{
	if (clipRect)
	{
		//clip line
		int clipValue = CohenSutherlandLineClip(x0, y0, x1, y1, *clipRect);

		//line is invisible, don't draw
		if (clipValue == 0)
			return;
	}

	//which side is longer
	int xLen = abs(x1 - x0);
	int yLen = abs(y1 - y0);

	if (xLen >= yLen)
	{
		
		//decode colors from DWORD
		int c1R, c1G, c1B, c1A;
		int c2R, c2G, c2B, c2A;
		GetRGBA32FromDWORD(c1R, c1G, c1B, c1A, color0);
		GetRGBA32FromDWORD(c2R, c2G, c2B, c2A, color1);

		//compute color deltas for the gradient
		double deltaR = (double)(c2R - c1R) / (abs(x1 - x0));
		double deltaG = (double)(c2G - c1G) / (abs(x1 - x0));
		double deltaB = (double)(c2B - c1B) / (abs(x1 - x0));
		double deltaA = (double)(c2A - c1A) / (abs(x1 - x0));

		//final display colors
		double outR = c1R;
		double outG = c1G;
		double outB = c1B;
		double outA = c1A;

		//determine direction term
		int incTerm = 1;
		if (x0 > x1)
			incTerm = -1;

		//how much does y change with each x step
		double deltaY = (double)(y1 - y0) / (x1 - x0);
		double y = y0;

		for (int x = x0; x != x1; x += incTerm)
		{
			DWORD color = _RGBA32BIT(int(outR + 0.5f), int(outG + 0.5f), int(outB + 0.5f), int(outA + 0.5f));
			
			buffer[x + int(y + 0.5f)*lpitch32] = color;
			y += deltaY*incTerm;
			outR = (outR + deltaR);
			outG = (outG + deltaG);
			outB = (outB + deltaB);
			outA = (outA + deltaA);


		}
		
	}

	if (yLen > xLen)
	{
		


		//decode colors from DWORD
		int c1R, c1G, c1B, c1A;
		int c2R, c2G, c2B, c2A;
		GetRGBA32FromDWORD(c1R, c1G, c1B, c1A, color0);
		GetRGBA32FromDWORD(c2R, c2G, c2B, c2A, color1);

		//compute color deltas for the gradient
		double deltaR = (double)(c2R - c1R) / (abs(y1 - y0));
		double deltaG = (double)(c2G - c1G) / (abs(y1 - y0));
		double deltaB = (double)(c2B - c1B) / (abs(y1 - y0));
		double deltaA = (double)(c2A - c1A) / (abs(y1 - y0));

		//final display colors
		double outR = c1R;
		double outG = c1G;
		double outB = c1B;
		double outA = c1A;

		//compute direction term
		int incTerm = 1;
		if (y0 > y1)
			incTerm = -1;

		//how much does y change with each x step
		double deltaX = (double)(x1 - x0) / (y1 - y0);
		double x = x0;

		for (int y = y0; y != y1; y += incTerm)
		{

			DWORD color = _RGBA32BIT(int(outR + 0.5f), int(outG + 0.5f), int(outB + 0.5f), int(outA + 0.5f));
			buffer[int(x + 0.5) + y*lpitch32] = color;
			x += deltaX*incTerm;
			outR = (outR + deltaR);
			outG = (outG + deltaG);
			outB = (outB + deltaB);
			outA = (outA + deltaA);


		}
		
	}
}



void DrawBitmapWithClipping(DWORD* dest, int destLPitch32, BITMAP_FILE* source, int destPosX, int destPosY, RECT* sourceRegion)
{
//	destPosX += int(SoftwareRasterizerSelfPointer->drawOffsetX + 0.5f);
//	destPosY += int(SoftwareRasterizerSelfPointer->drawOffsetY + 0.5f);

	int image_width = source->infoHeader.biWidth;
	int image_height = source->infoHeader.biHeight;
	int offsetX = 0;
	int offsetY = 0;
	UINT byteCount = source->infoHeader.biBitCount / 8;

	//use whole source image unless region is specified
	if (sourceRegion)
	{
		image_width = sourceRegion->right - sourceRegion->left;
		image_height = sourceRegion->bottom - sourceRegion->top;
		offsetX = sourceRegion->left;
		offsetY = sourceRegion->top;
	}

	//reject off screen bitmap
	if (destPosX + image_width < 0)
		return;
	if (destPosY + image_height < 0)
		return;
	if (destPosX > SoftwareRasterizerSelfPointer->clientRect.right)
		return;
	if (destPosY >  SoftwareRasterizerSelfPointer->clientRect.bottom)
		return;


	//destination region
	int x1 = destPosX;
	int y1 = destPosY;
	int x2 = x1 + image_width;
	int y2 = y1 + image_height;

	if (x1 < 0)
		x1 = 0;
	if (y1 < 0)
		y1 = 0;
	if (x2 >= SoftwareRasterizerSelfPointer->clientRect.right)
		x2 = SoftwareRasterizerSelfPointer->clientRect.right - 1;
	if (y2 >= SoftwareRasterizerSelfPointer->clientRect.bottom)
		y2 = SoftwareRasterizerSelfPointer->clientRect.bottom - 1;

	//compute offset into region if clipped by window
	int reg_offsetX = x1 - destPosX;
	int reg_offsetY = y1 - destPosY;

	//establish starting points in memory for 32 bit bitmap
	if (byteCount == 4)
	{
		DWORD* sourceStartMem = ((DWORD*)source->data) + ((offsetX + reg_offsetX) + ((offsetY + reg_offsetY) * image_width));
		DWORD* destStartMem = (dest) + ( x1 +  y1*destLPitch32);

		int numColumns = (x2 - x1);
		int numRows = (y2 - y1);

		for (int row = 0; row < numRows; row++)
		{
			for (int column = 0; column < numColumns; column++)
			{

				destStartMem[column] = sourceStartMem[column];

			}

			destStartMem += destLPitch32;
			sourceStartMem += image_width;
		}

	}
	else if (byteCount == 3)
	{
		UCHAR* sourceStartMem = source->data + (((offsetX + reg_offsetX) * byteCount) + ((offsetY + reg_offsetY)* image_width * byteCount));
		UCHAR* destStartMem = (UCHAR*) (dest)+ ( x1  + y1 *(destLPitch32 << 2));

		int numColumns = (x2 - x1) + 1;
		int numRows = (y2 - y1) + 1;

		for (int row = 0; row < numRows; row++)
		{
			for (int column = 0; column < numColumns; column++)
			{
				UCHAR pixel[4];
				pixel[0] = sourceStartMem[column * 3];
				pixel[1] = sourceStartMem[column * 3 + 1];
				pixel[2] = sourceStartMem[column * 3 + 2];

				destStartMem[column * 3] = pixel[0];
				destStartMem[column * 3 + 1] = pixel[1];
				destStartMem[column * 3 + 2] = pixel[2];
				destStartMem[column * 3 + 3] = 255;


			}

			destStartMem += destLPitch32 << 2;
			sourceStartMem += image_width * 3;
		}
	}



}




void DrawCharBitmap(DWORD* mem_buffer, int lpitch32, char* bitmap, DWORD color, int xpos, int ypos, int rows, int columns, float scale)
{

	
	if(scale == 1.0f)
	{
		for(int y = 0; y < rows; y++)
		{
			for(int x = 0; x < columns; x++)
			{
				if(bitmap[x + y*columns])
					mem_buffer[x + xpos + (y + ypos)*lpitch32] = color;
				else
					mem_buffer[x + xpos + (y + ypos)*lpitch32] = COLOR_BLACK;

			}
		}
	}
	else
	{
		float newXDimension = scale * columns;
		float newYDimension = scale * rows;
		float ratio = 1/scale;
		float overflowY = 0.0f;
		float overflowX = 0.0f;


	//	int txpos = xpos, typos = ypos;
		int bmpXpos = 0, bmpYpos = 0;
		
		int row = 0, col = 0;
		for(row; row < (int)newYDimension; row++)
		{
			if(bmpYpos > rows - 1)
			{
				break;		
			}

			col = 0;
			for(col; col < (int)newXDimension; col++)
			{
				if(bitmap[bmpXpos + bmpYpos*columns])
				{
					mem_buffer[xpos + col + (ypos + row)*lpitch32] = color;
				}
				else
				{
					mem_buffer[xpos + col + (ypos + row)*lpitch32] = COLOR_BLACK;
				}

	

 				overflowY += ratio;
				if(overflowY >= 1.0f)
				{
					bmpXpos++;
					overflowY = 0.0f;
				}


				


			}

			overflowX += ratio;
			if(overflowX > 1.0f)
			{
				overflowX = 0.0f;
				bmpYpos++;
			}

			bmpXpos=0;

		
		}

	}
}

void _DrawFlatTopTriangleTextured(VERTEX2D sortVerts[], MATRIX3D& transform, BITMAP_FILE_PTR texture, DWORD* video_mem, int lpitch32)
{
	float xtranslate = transform.M[2][0];
	float ytranslate = transform.M[2][1];

	float dxdyL = (sortVerts[1].pos.x - sortVerts[0].pos.x) / abs(sortVerts[1].pos.y - sortVerts[0].pos.y);
	float dudyL = (sortVerts[1].u - sortVerts[0].u) / abs(sortVerts[1].pos.y - sortVerts[0].pos.y);
	float dvdyL = (sortVerts[1].v - sortVerts[0].v) / abs(sortVerts[1].pos.y - sortVerts[0].pos.y);

	float dxdyR = (sortVerts[2].pos.x - sortVerts[0].pos.x) / abs(sortVerts[2].pos.y - sortVerts[0].pos.y);
	float dudyR = (sortVerts[2].u - sortVerts[0].u) / abs(sortVerts[2].pos.y - sortVerts[0].pos.y);
	float dvdyR = (sortVerts[2].v - sortVerts[0].v) / abs(sortVerts[2].pos.y - sortVerts[0].pos.y);


	float x_left  = sortVerts[0].pos.x;
	float x_right = sortVerts[0].pos.x;
	
	float u_left  = sortVerts[0].u;
	float u_right = sortVerts[0].u;
	float v_left  = sortVerts[0].v;
	float v_right = sortVerts[0].v;

	float ui = 0;
	float vi = 0;
	float du = 0;
	float dv = 0;
	float xstart = 0;
	float xend = 0;
	float scanline_len = 0;

	for(float y = sortVerts[0].pos.y; y > sortVerts[1].pos.y; y--)
	{
		if(scanline_len != 0)
		{
			ui = u_left;
			vi = v_left;
			du = (u_right - u_left)/scanline_len;
			dv = (v_right - v_left)/scanline_len;

			xstart = x_left;
			xend = x_right;

			DWORD* texData = (DWORD*)texture->data;
			for(xstart; xstart < xend; xstart++)
			{
				int iui = int(ui + 0.5f);
				int ivi = int(vi + 0.5f);
				DWORD pixel = texData[iui + ivi*texture->infoHeader.biWidth];
				video_mem[int(xstart + xtranslate + (y + ytranslate)*lpitch32 + 0.5f)] = pixel;

				ui+= du;
				vi+= dv;
			}
		}
		
		
		u_left  += dudyL;
		u_right += dudyR;
		v_left  += dvdyL;
		v_right += dvdyR;

		x_left  += dxdyL;
		x_right += dxdyR;

		scanline_len = abs(x_right - x_left);

		
	}


}
void _DrawFlatBottomTriangleTextured(VERTEX2D sortVerts[], MATRIX3D& transform, BITMAP_FILE_PTR texture, DWORD* video_mem, int lpitch32)
{
	float dxdyL = (sortVerts[0].pos.x - sortVerts[2].pos.x)/abs(sortVerts[0].pos.y - sortVerts[2].pos.y);
	float dudyL = (sortVerts[0].u - sortVerts[2].u) / abs(sortVerts[0].pos.y - sortVerts[2].pos.y);
	float dvdyL = (sortVerts[0].v - sortVerts[2].v) / abs(sortVerts[0].pos.y - sortVerts[2].pos.y);

	float dxdyR = (sortVerts[1].pos.x - sortVerts[2].pos.x)/abs(sortVerts[1].pos.y - sortVerts[2].pos.y);
	float dudyR = (sortVerts[1].u - sortVerts[2].u) / abs(sortVerts[1].pos.y - sortVerts[2].pos.y);
	float dvdyR = (sortVerts[1].v - sortVerts[2].v) / abs(sortVerts[1].pos.y - sortVerts[2].pos.y);

	float x_left = sortVerts[2].pos.x;
	float x_right = x_left;
	float u_left = sortVerts[2].u;
	float u_right = u_left;
	float v_left = sortVerts[2].v;
	float v_right = v_left;

	float scanline_len = 0;
	float ui = 0;
	float vi = 0;
	float du = 0;
	float dv = 0;
	float xstart = 0;
	float xend = 0;

	float transformX = transform.M[2][0];
	float transformY = transform.M[2][1];

	for(int y = int(sortVerts[2].pos.y + 0.5f); y < int(sortVerts[0].pos.y + 0.5f); y++)
	{
		if(scanline_len != 0)
		{
			ui = u_left;
			vi = v_left;
			du = (u_right - u_left)/scanline_len;
			dv = (v_right - v_left)/scanline_len;
			xstart = x_left;
			xend = x_right;

			DWORD* texMem = (DWORD*)texture->data;
			for(xstart; xstart < xend; xstart++)
			{
				int iui = int(ui + 0.5f);
				int ivi = int(vi + 0.5f);
				DWORD pixel = texMem[iui + ivi*texture->infoHeader.biWidth];
				video_mem[int(xstart + transformX + (y + transformY)*lpitch32)] = pixel;

				ui+= du;
				vi+= dv;
			}
			
		}

		x_left += dxdyL;
		x_right += dxdyR;
		u_left += dudyL;
		u_right += dudyR;
		v_left += dvdyL;
		v_right += dvdyR;

		scanline_len = abs(x_right - x_left);
	}
}
void _DrawFlatTopTriangleGouraud(VERTEX2D sortVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32)
{
	float L_change_x = (sortVerts[0].pos.x - sortVerts[1].pos.x)/(sortVerts[0].pos.y - sortVerts[1].pos.y);
	float R_change_x = (sortVerts[0].pos.x - sortVerts[2].pos.x)/(sortVerts[0].pos.y - sortVerts[2].pos.y);

	//transform matrix decomposition
	float xTranslation = transform.M[2][0];
	float yTranslation = transform.M[2][1];

	float xScale = transform.M[0][0];
	float yScale = transform.M[1][1];

//	MATRIX3D modTransform = transform;
	//modTransform.M[2][0] = 0.0f;
	//modTransform.M[2][1] = 0.0f;
	
//	for(int i = 0; i < 3; i++)
	//{
	//	MatrixVectorMultiply(&sortVerts[i].pos, &transform);
	//}


	for(int i = 0; i < 3; i++)
	{
		sortVerts[i].pos.x *= xScale;
		sortVerts[i].pos.y *= yScale;
	}


	//interpolate color from top vertex to bottom
	int rL1, gL1, bL1, aL1;
	int rL2, gL2, bL2, aL2;

	int rR1, gR1, bR1, aR1;
	int rR2, gR2, bR2, aR2;

	int heightL = sortVerts[0].pos.y - sortVerts[2].pos.y;
	int heightR = sortVerts[0].pos.y - sortVerts[1].pos.y;
	GetRGBA32FromDWORD(rL1, gL1, bL1, aL1, sortVerts[0].color);
	GetRGBA32FromDWORD(rL2, gL2, bL2, aL2, sortVerts[1].color);
	GetRGBA32FromDWORD(rR1, gR1, bR1, aR1, sortVerts[0].color);
	GetRGBA32FromDWORD(rR2, gR2, bR2, aR2, sortVerts[2].color);

	//find the sample rate
	float rrateL = (rL2 - rL1)/(float)heightL;
	float grateL = (gL2 - gL1)/(float)heightL;
	float brateL = (bL2 - bL1)/(float)heightL;
	float arateL = (aL2 - aL1) / (float)heightL;

	float rrateR = (rR2 - rR1)/(float)heightR;
	float grateR = (gR2 - gR1)/(float)heightR;
	float brateR = (bR2 - bR1)/(float)heightR;
	float arateR = (aR2 - aR1) / (float)heightR;


	//start at top verex
	float LsampledColorR, LsampledColorG, LsampledColorB, LsampledColorA;
	float RsampledColorR, RsampledColorG, RsampledColorB, RsampledColorA;

	int tr, tg, tb, ta;
	GetRGBA32FromDWORD(tr, tg, tb, ta, sortVerts[0].color);
	LsampledColorR = tr;
	LsampledColorG = tg;
	LsampledColorB = tb;
	LsampledColorA = ta;

	RsampledColorR = tr;
	RsampledColorG = tg;
	RsampledColorB = tb;
	RsampledColorA = ta;
	

	float rasterLineXLeft = sortVerts[0].pos.x;
	float rasterLineXRight = sortVerts[0].pos.x;


	//start at bottom verex
	for(int y = int(sortVerts[0].pos.y + 0.5f); y >= int(sortVerts[1].pos.y + 0.5f); y--)
	{
		DWORD sampledColorL = _RGBA32BIT(int(LsampledColorR + 0.5f), int(LsampledColorG + 0.5f), int(LsampledColorB + 0.5f), int(LsampledColorA + 0.5f));
		DWORD sampledColorR = _RGBA32BIT(int(RsampledColorR + 0.5f), int(RsampledColorG + 0.5f), int(RsampledColorB + 0.5f), int(RsampledColorA + 0.5f));
		
		QueueLine( rasterLineXLeft + xTranslation, y + yTranslation, rasterLineXRight + xTranslation, y + yTranslation, sampledColorL, sampledColorR);
			
		// go up the slope
		rasterLineXLeft  -=  L_change_x;
		rasterLineXRight -=  R_change_x;

		//guraud shading
		LsampledColorR += rrateL;
		LsampledColorG += grateL;
		LsampledColorB += brateL;
		LsampledColorA += arateL;

		RsampledColorR += rrateR;
		RsampledColorG += grateR;
		RsampledColorB += brateR;
		RsampledColorA += arateR;

	}
}
void _DrawFlatBottomTriangleGouraud(VERTEX2D sortVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32)
{
	//compute change in x for every change in y for left line
	float L_change_x = (sortVerts[2].pos.x - sortVerts[1].pos.x)/(sortVerts[2].pos.y - sortVerts[1].pos.y);
	float R_change_x = (sortVerts[2].pos.x - sortVerts[0].pos.x)/(sortVerts[2].pos.y - sortVerts[0].pos.y);


	//transform matrix decomposition
	float xTranslation = transform.M[2][0];
	float yTranslation = transform.M[2][1];

	float xScale = transform.M[0][0];
	float yScale = transform.M[1][1];


	for(int i = 0; i < 3; i++)
	{
		sortVerts[i].pos.x *= xScale;
		sortVerts[i].pos.y *= yScale;
	}

	


	//interpolate color from top vertex to bottom
	int rL1, gL1, bL1, aL1;
	int rL2, gL2, bL2, aL2;

	int rR1, gR1, bR1, aR1;
	int rR2, gR2, bR2, aR2;
	int heightL = sortVerts[2].pos.y - sortVerts[1].pos.y;
	int heightR = sortVerts[2].pos.y - sortVerts[0].pos.y;
	GetRGBA32FromDWORD(rL1, gL1, bL1, aL1, sortVerts[2].color);
	GetRGBA32FromDWORD(rL2, gL2, bL2, aL2, sortVerts[1].color);
	GetRGBA32FromDWORD(rR1, gR1, bR1, aR1, sortVerts[2].color);
	GetRGBA32FromDWORD(rR2, gR2, bR2, aR2, sortVerts[0].color);

	//find the sample rate
	float rrateL = (rL2 - rL1)/(float)heightL;
	float grateL = (gL2 - gL1)/(float)heightL;
	float brateL = (bL2 - bL1)/(float)heightL;
	float arateL = (aL2 - aL1) / (float)heightL;

	float rrateR = (rR2 - rR1)/(float)heightR;
	float grateR = (gR2 - gR1)/(float)heightR;
	float brateR = (bR2 - bR1)/(float)heightR;
	float arateR = (aR2 - aR1) / (float)heightR;

	//start at top verex
	float LsampledColorR, LsampledColorG, LsampledColorB, LsampledColorA;
	float RsampledColorR, RsampledColorG, RsampledColorB, RsampledColorA;

	int tr, tg, tb, ta;
	GetRGBA32FromDWORD(tr, tg, tb, ta, sortVerts[2].color);
	LsampledColorR = tr;
	LsampledColorG = tg;
	LsampledColorB = tb;
	LsampledColorA = ta;

	RsampledColorR = tr;
	RsampledColorG = tg;
	RsampledColorB = tb;
	RsampledColorA = ta;
	

	float rasterLineXLeft = sortVerts[2].pos.x;
	float rasterLineXRight = sortVerts[2].pos.x;

	for(int y = int(sortVerts[2].pos.y + 0.5f); y <= int(sortVerts[1].pos.y + 0.5f); y++)
	{ 
		DWORD sampledColorL = _RGBA32BIT(int(LsampledColorR + 0.5f), int(LsampledColorG + 0.5f), int(LsampledColorB + 0.5f), int(LsampledColorA + 0.5f));
		DWORD sampledColorR = _RGBA32BIT(int(RsampledColorR + 0.5f), int(RsampledColorG + 0.5f), int(RsampledColorB + 0.5f), int(RsampledColorA + 0.5f));
		
		QueueLine( rasterLineXLeft + xTranslation, y + yTranslation, rasterLineXRight + xTranslation, y + yTranslation, sampledColorL, sampledColorR);
			
		// go up the slope
		rasterLineXLeft  +=  L_change_x;
		rasterLineXRight +=  R_change_x;

		//guraud shading
		LsampledColorR -= rrateL;
		LsampledColorG -= grateL;
		LsampledColorB -= brateL;
		LsampledColorA -= arateL;

		RsampledColorR -= rrateR;
		RsampledColorG -= grateR;
		RsampledColorB -= brateR;
		RsampledColorA -= arateR;


	}
}
void _DrawFlatTopTriangleConstant(VERTEX2D sortedVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32)
{
	float L_change_x = (sortedVerts[0].pos.x - sortedVerts[1].pos.x) / (sortedVerts[0].pos.y - sortedVerts[1].pos.y);
	float R_change_x = (sortedVerts[0].pos.x - sortedVerts[2].pos.x) / (sortedVerts[0].pos.y - sortedVerts[2].pos.y);

	TransformVertexList(sortedVerts, 3, &transform);


	float rasterLineXLeft = sortedVerts[0].pos.x;
	float rasterLineXRight = sortedVerts[0].pos.x;

	


	//start at bottom verex
	DWORD* tempVideoMem = video_mem;
	for (int y = int(sortedVerts[0].pos.y + 0.5f); y >= int(sortedVerts[1].pos.y + 0.5f); y--)
	{
		
		//QueueLine( rasterLineXLeft, y, rasterLineXRight, y, sortedVerts[0].color, sortedVerts[0].color);

		//simply fill the triangle, much faster then parametric line
		tempVideoMem = video_mem;
		tempVideoMem += int(rasterLineXLeft) + y*lpitch32;
		for (int i = 0; i < rasterLineXRight - rasterLineXLeft; i++)
		{
			
			tempVideoMem[i] = sortedVerts[0].color;
		}

		// go up the slope
		rasterLineXLeft -= L_change_x;
		rasterLineXRight -= R_change_x;

	}
}
void _DrawFlatBottomTriangleConstant(VERTEX2D sortedVerts[], MATRIX3D& transform, DWORD* video_mem, int lpitch32)
{
	//compute change in x for every change in y for left line
	float L_change_x = (sortedVerts[2].pos.x - sortedVerts[1].pos.x) / (sortedVerts[2].pos.y - sortedVerts[1].pos.y);
	float R_change_x = (sortedVerts[2].pos.x - sortedVerts[0].pos.x) / (sortedVerts[2].pos.y - sortedVerts[0].pos.y);

	TransformVertexList(sortedVerts, 3, &transform);

	float rasterLineXLeft = sortedVerts[2].pos.x;
	float rasterLineXRight = sortedVerts[2].pos.x;

	for (int y = int(sortedVerts[2].pos.y + 0.5f); y <= int(sortedVerts[1].pos.y + 0.5f); y++)
	{
	
	
		QueueLine( rasterLineXLeft, y, rasterLineXRight, y, sortedVerts[0].color, sortedVerts[0].color);

		// go up the slope
		rasterLineXLeft += L_change_x;
		rasterLineXRight += R_change_x;

	}
}

void DrawCircle(DWORD* video_mem, int lpitch32, int centerX, int centerY, int radius, DWORD color)
{

}

void SplitTriangle(VERTEX2D sortVerts[], VERTEX2D topTriangle[], VERTEX2D bottomTriangle[])
{
	//triangle has to require splitting otherwise this function will fail
	//check if the triangle is already top or bottom before calling this function
	
	//needs to be split
	//start at top vertex and go down
	//compute change in x for every change in y for left line
	float L_change_x = (sortVerts[1].pos.x - sortVerts[0].pos.x)/abs(sortVerts[1].pos.y - sortVerts[0].pos.y);
	float R_change_x = (sortVerts[2].pos.x - sortVerts[0].pos.x)/abs(sortVerts[2].pos.y - sortVerts[0].pos.y);

	float rasterLineXLeft = sortVerts[0].pos.x;
	float rasterLineXRight = sortVerts[0].pos.x;

	//start at top verex
	for(int y = int(sortVerts[0].pos.y + 0.5f); y >=  int(sortVerts[1].pos.y + 0.5f); y--)
	{
		// go up the slope
		rasterLineXLeft  +=  L_change_x;
		rasterLineXRight +=  R_change_x;

	}

	//new point
	VERTEX2D newV;
	newV.pos = VECTOR2D(rasterLineXRight, sortVerts[1].pos.y);

	//find how far the most bottom and most top vertices are in scanlines
	float dy = sortVerts[2].pos.y - sortVerts[0].pos.y;

	//find number of scanlines (pixels) between bottom vertex and new vertex
	float d2toNew = sortVerts[2].pos.y - newV.pos.y;

	//find ratio
	float ratio = d2toNew/dy;

	//first use ratio to interpolate color
	int v0r, v0g, v0b, v0a;
	int v2r, v2g, v2b, v2a;
	int newR, newG, newB, newA;
	GetRGBA32FromDWORD(v0r, v0g, v0b, v0a, sortVerts[0].color);
	GetRGBA32FromDWORD(v2r, v2g, v2b, v2a, sortVerts[2].color);

	newR = v2r + int((v0r-v2r)*ratio + 0.5f);
	newG = v2g + int((v0g-v2g)*ratio + 0.5f);
	newB = v2b + int((v0b-v2b)*ratio + 0.5f);
	newA = v2a + int((v0a - v2a)*ratio + 0.5f);
	DWORD newColor = _RGBA32BIT(newR, newG, newB, newA);
	newV.color = newColor;

	//now interpolate the u,v coordinates
	int new_u, new_v;
	new_u = sortVerts[2].u + int((sortVerts[0].u - sortVerts[2].u)*ratio + 0.5f);
	new_v = sortVerts[2].v + int((sortVerts[0].v - sortVerts[2].v)*ratio + 0.5f);
	newV.u = new_u;
	newV.v = new_v;
	

	//assign the tris
	bottomTriangle[0] = sortVerts[0];
	bottomTriangle[1] = newV;
	bottomTriangle[2] = sortVerts[1];


	topTriangle[0] = sortVerts[1];
	topTriangle[1] = newV;
	topTriangle[2] = sortVerts[2];

	SortVerticesByY(bottomTriangle, 3);
	SortVerticesByY(topTriangle, 3);
	

}

void DrawMeshObject(MESHOBJECT& m, int flags, DWORD* video_mem, int lpitch32)
{
	if (m.vertexBuffer.empty())
		return;

	switch (flags)
	{
	case DO_POINTLIST:
	{
		for (int i = 0; i < m.vertexBuffer.size(); i++)
		{
		//	DrawPixelSquare(m.vertexBuffer[i].pos.x, m.vertexBuffer[i].pos.y, m.vertexBuffer[i].color, 1, video_mem, lpitch32);
		}
	} break;

	case DO_LINELIST:
	{

		float tX, tY;
		tX = m.worldTransform.M[2][0];
		tY = m.worldTransform.M[2][1];


		int c = 0;
		for (c; c < m.vertexBuffer.size() - 1; c++)
		{
			QueueLine( m.vertexBuffer[c].pos.x + tX, m.vertexBuffer[c].pos.y + tY, m.vertexBuffer[c + 1].pos.x + tX, m.vertexBuffer[c + 1].pos.y + tY, COLOR_RED, COLOR_RED);
		}

	} break;

	case DO_FLAT:
	{
	
					
		for (int i = 0; i < m.indexBuffer.size(); i += 3)
		{
			VERTEX2D tri[3];
			tri[0] = m.vertexBuffer[m.indexBuffer[i]];
			tri[1] = m.vertexBuffer[m.indexBuffer[i + 1]];
			tri[2] = m.vertexBuffer[m.indexBuffer[i + 2]];

			MESHOBJECT newMO;
			for (int vertNum = 0; vertNum < 3; vertNum++)
			{
				if (tri[vertNum].pos.y < 0)
				{
					//two adjacent line segments
					VECTOR2D L1p1, L1p2;
					VECTOR2D L2p1, L2p2;

					int adjacentM1 = vertNum - 1;
					int adjacentP1 = vertNum + 1;

					if (adjacentM1 < 0)
						adjacentM1 = m.vertexBuffer.size() - 1; //set to last vertex
					if (adjacentP1 == m.vertexBuffer.size())
						adjacentP1 = 0; //set to first vertex

					L1p1 = m.vertexBuffer[adjacentM1].pos;
					L1p2 = m.vertexBuffer[vertNum].pos;

					L2p1 = m.vertexBuffer[adjacentP1].pos;
					L2p2 = m.vertexBuffer[vertNum].pos;

					//if line 1 is vertical, new point is simply x coord and y coord is 0
					VECTOR2D L1NewPt;
					if ((L1p2.x - L1p1.x) == 0)
					{
						L1NewPt.x = L1p1.x;
						L1NewPt.y = 0;
					}
					else
					{
						float slopeL1 = (L1p2.y - L1p1.y) / (L1p2.x - L1p1.x);
						float yIntercept = L1p1.y - L1p1.x * slopeL1;
						yIntercept = -yIntercept;
						L1NewPt.x = yIntercept / slopeL1;
						L1NewPt.y = 0;
					}

					VECTOR2D L2NewPt;
					if ((L2p2.x - L2p1.x) == 0)
					{
						L2NewPt.x = L1p1.x;
						L2NewPt.y = 0;
					}
					else
					{
						float slopeL2 = (L2p2.y - L2p1.y) / (L2p2.x - L2p1.x);
						float yIntercept = L2p2.y - L2p2.x * slopeL2;
						yIntercept = -yIntercept;
						L2NewPt.x = yIntercept / slopeL2;
						L2NewPt.y = 0;
					}
					
					VERTEX2D newV1, newV2;
					newV1.pos = L1NewPt;
					newV1.color = m.vertexBuffer[0].color;
					newV2.pos = L2NewPt;
					newV2.color = newV1.color;

					newMO.vertexBuffer.push_back(newV1);
					newMO.vertexBuffer.push_back(newV2);

				}
				else
				{
					newMO.vertexBuffer.push_back(tri[vertNum]);
				}

				newMO.indexBuffer.push_back(0);
				newMO.indexBuffer.push_back(1);
				newMO.indexBuffer.push_back(2);
				newMO.indexBuffer.push_back(2);
				newMO.indexBuffer.push_back(3);
				newMO.indexBuffer.push_back(0);


			}

		//	DrawMeshObject(newMO, DO_FLAT, video_mem, lpitch32);


			DrawTriangle(video_mem, lpitch32, tri, m.worldTransform, DO_FLAT );
		}
	} break;

	case DO_GOURAUD:
	{
		for (int i = 0; i < m.indexBuffer.size(); i += 3)
		{
			VERTEX2D tri[3];
			tri[0] = m.vertexBuffer[m.indexBuffer[i]];
			tri[1] = m.vertexBuffer[m.indexBuffer[i + 1]];
			tri[2] = m.vertexBuffer[m.indexBuffer[i + 2]];

			DrawTriangle(video_mem, lpitch32, tri, m.worldTransform, DO_GOURAUD);
		}
	}break;

	
	case DO_TEXTURED:
	{
		
		for (int i = 0; i < m.indexBuffer.size(); i += 3)
		{
			VERTEX2D tri[3];
			tri[0] = m.vertexBuffer[m.indexBuffer[i]];
			tri[1] = m.vertexBuffer[m.indexBuffer[i + 1]];
			tri[2] = m.vertexBuffer[m.indexBuffer[i + 2]];

			DrawTriangle(video_mem, lpitch32, tri, m.worldTransform, DO_TEXTURED, &m.texture);
		}

	}break;
	

	default: break;
	}

}


void DrawTriangle(DWORD* video_mem, int lpitch32, VERTEX2D triangle[3], MATRIX3D& transform, int drawOptions, BITMAP_FILE* texture)
{
	if (drawOptions == DRAW_OPTIONS::DO_FLAT)
	{

		VERTEX2D sortVerts[3];
		sortVerts[0] = triangle[0];
		sortVerts[1] = triangle[1];
		sortVerts[2] = triangle[2];

		SortVerticesByY(sortVerts, 3);

		//check if sides are equal, means tri doesn't have to be split, draw right away
		if (sortVerts[0].pos.y == sortVerts[1].pos.y)
		{
			_DrawFlatBottomTriangleConstant(sortVerts, transform, video_mem, lpitch32);
			return;
		}
		if (sortVerts[1].pos.y == sortVerts[2].pos.y)
		{
			_DrawFlatTopTriangleConstant(sortVerts, transform, video_mem, lpitch32);
			return;
		}

		//split triangle
		VERTEX2D topTri[3];
		VERTEX2D bottomTri[3];
		SplitTriangle(sortVerts, topTri, bottomTri);

		_DrawFlatBottomTriangleConstant(topTri, transform, video_mem, lpitch32);
		_DrawFlatTopTriangleConstant(bottomTri, transform, video_mem, lpitch32);

	}
	else if (drawOptions == DRAW_OPTIONS::DO_LINELIST)
	{

		int lastIndex = 0;
		int xOffset = transform.M[2][0];
		int yOffset = transform.M[2][1];

		//draw a flipped polygon (-1 flips the y axis)
		for (int i = 0; i < 3; i++)
		{
			QueueLine(
				triangle[i].pos.x + xOffset,
				triangle[i].pos.y + yOffset,
				triangle[i + 1].pos.x + xOffset,
				triangle[i + 1].pos.y + yOffset,
				COLOR_BLUE,
				COLOR_BLUE);

			//save which vertex was last
			lastIndex = i;
		}

		//close polygon
		QueueLine(
			triangle[0].pos.x + xOffset,
			triangle[0].pos.y + yOffset,
			triangle[lastIndex + 1].pos.x + xOffset,
			triangle[lastIndex + 1].pos.y + yOffset,
			COLOR_BLUE,
			COLOR_BLUE);

	}
	else if (drawOptions == DRAW_OPTIONS::DO_GOURAUD)
	{

		VERTEX2D sortVerts[3];
		sortVerts[0] = triangle[0];
		sortVerts[1] = triangle[1];
		sortVerts[2] = triangle[2];


		SortVerticesByY(sortVerts, 3);


		if (sortVerts[0].pos.y == sortVerts[1].pos.y)
		{
			_DrawFlatBottomTriangleGouraud(sortVerts, transform, video_mem, lpitch32);
			return;
		}
		if (sortVerts[1].pos.y == sortVerts[2].pos.y)
		{
			_DrawFlatTopTriangleGouraud(sortVerts, transform, video_mem, lpitch32);
			return;
		}

		VERTEX2D topTri[3];
		VERTEX2D bottomTri[3];
		SplitTriangle(sortVerts, topTri, bottomTri);



		_DrawFlatBottomTriangleGouraud(topTri, transform, video_mem, lpitch32);
		_DrawFlatTopTriangleGouraud(bottomTri, transform, video_mem, lpitch32);
	}
	else if (drawOptions == DRAW_OPTIONS::DO_TEXTURED)
	{
		VERTEX2D sortVerts[3];
		sortVerts[0] = triangle[0];
		sortVerts[1] = triangle[1];
		sortVerts[2] = triangle[2];

		SortVerticesByY(sortVerts, 3);


		if (sortVerts[0].pos.y == sortVerts[1].pos.y)
		{
			_DrawFlatBottomTriangleTextured(sortVerts, transform, texture, video_mem, lpitch32);
			return;
		}
		if (sortVerts[1].pos.y == sortVerts[2].pos.y)
		{
			_DrawFlatTopTriangleTextured(sortVerts, transform, texture, video_mem, lpitch32);
			return;
		}


		VERTEX2D topTri[3];
		VERTEX2D bottomTri[3];
		SplitTriangle(sortVerts, topTri, bottomTri);

		float offX = transform.M[2][0];
		float offY = transform.M[2][1];

	
		_DrawFlatBottomTriangleTextured(topTri, transform, texture, video_mem, lpitch32);
		_DrawFlatTopTriangleTextured(bottomTri, transform, texture, video_mem, lpitch32);

	}
}




//---------------------System---------------------------------------
void WindowVisibility(HWND window, bool isVisible)
{
	if(!IsWindow(window))
		return;

	if(isVisible)
		ShowWindow(window, SW_SHOW);
	else
		ShowWindow(window, SW_HIDE);
}
void SystemHotKeys(HWND window)
{
	static bool isVisible = true;
	short lshift_state = GetAsyncKeyState(VK_LSHIFT);
	short lctrl_state = GetAsyncKeyState(VK_LCONTROL);

	short hotkey_H_hideWnd = GetAsyncKeyState(0x48);

	if((lshift_state & 0xFF00) && (lctrl_state & 0xFF00) && (hotkey_H_hideWnd & 0xFF00))
	{
		isVisible = !isVisible;
		WindowVisibility(window, isVisible);
		Sleep(1000);
	}


}

