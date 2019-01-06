#include "SRasterizer.h"

namespace SoftwareRasterizer
{

	UINT numVertexElements = 3;
	D3D10_INPUT_ELEMENT_DESC DX10VertexLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};


	SRasterizer::SRasterizer(HWND Window)
	{
		//setup swap chain 
		ZeroMemory(&D3D10SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		//set buffer dimensions and format
		RECT clientRegion;
		GetClientRect(Window, &clientRegion);

		D3D10SwapChainDesc.BufferCount = 2;
		D3D10SwapChainDesc.BufferDesc.Width = clientRegion.right - clientRegion.left;
		D3D10SwapChainDesc.BufferDesc.Height = clientRegion.bottom - clientRegion.top;
		D3D10SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		D3D10SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		//set refresh rate
		D3D10SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		D3D10SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

		//sampling settings
		D3D10SwapChainDesc.SampleDesc.Quality = 0;
		D3D10SwapChainDesc.SampleDesc.Count = 1;

		//output window handle
		D3D10SwapChainDesc.OutputWindow = Window;
		D3D10SwapChainDesc.Windowed = true;
		mainWindow = Window;

		//create device 

		HRESULT hr1;
		if (FAILED(hr1 = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG, D3D10_SDK_VERSION, &D3D10SwapChainDesc, &pD3D10SwapChain, &pD3D10Device)))
		{
			ZeroMemory(&D3D10SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
			return;
		}

		//get the info interface
		pD3D10Device->QueryInterface(__uuidof(ID3D10InfoQueue), (void**)&pD3D10InfoQueue);

		// create render target for merger state

		ID3D10Texture2D* pBackBuffer;
		if (FAILED(pD3D10SwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer))) return;

		//try to create render target view
		if (FAILED(pD3D10Device->CreateRenderTargetView(pBackBuffer, NULL, &pD3D10RenderTargetView))) return;

		//release the back buffer
		pBackBuffer->Release();

		//set the render target
		pD3D10Device->OMSetRenderTargets(1, &pD3D10RenderTargetView, NULL);


		//set view port aka region of render target 
		D3D10Viewport.Width = D3D10SwapChainDesc.BufferDesc.Width;
		D3D10Viewport.Height = D3D10SwapChainDesc.BufferDesc.Height;
		D3D10Viewport.MinDepth = 0.0f;
		D3D10Viewport.MaxDepth = 1.0f;
		D3D10Viewport.TopLeftX = 0;
		D3D10Viewport.TopLeftY = 0;

		pD3D10Device->RSSetViewports(1, &D3D10Viewport);

		//set up matrices 


		//compute the ratio of screen dimensions

		float aspectRatio = float(clientRect.left) / float(clientRect.bottom);
		float planeXOrig = -1.0f, planeYOrig = -1.0f;
		float planeWidth = 2.0f;
		float planeHeight = 2.0f;


		//orthographic
		XMMATRIX xmatT = XMMatrixIdentity();
		XMStoreFloat4x4(&matView, xmatT);


		//load the effects file to be used 

		HRESULT hrFx;
		if (FAILED(hrFx = D3DX10CreateEffectFromFile(TEXT("C:\\Users\\Tim\\Documents\\Visual Studio Projects\\SoftwareRasterizer\\SoftwareRasterizer\\DX10RendererEffect.fx"), NULL, NULL, "fx_4_0", D3D10_SHADER_DEBUG, 0, pD3D10Device, NULL, NULL, &pD3D10Effect, NULL, NULL)))
		{
			return;
		}


		//create matrix effect pointers
		pViewMatrixEffectVariable = pD3D10Effect->GetVariableByName("View")->AsMatrix();
		pProjectionMatrixEffectVariable = pD3D10Effect->GetVariableByName("Projection")->AsMatrix();
		pWorldMatrixEffectVariable = pD3D10Effect->GetVariableByName("World")->AsMatrix();


		//create input layout 
		D3D10_PASS_DESC PassDesc;
		pD3D10EffectTechnique = pD3D10Effect->GetTechniqueByName("Render");
		pD3D10EffectTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);


		if (FAILED(pD3D10Device->CreateInputLayout(DX10VertexLayout, numVertexElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout))) return;

		pD3D10Device->IASetInputLayout(pInputLayout);

		//create the VERTEX BUFFER for the plane that we'll be drawing on 
		numVertices = 4;

		D3D10_BUFFER_DESC vbDesc;
		vbDesc.Usage = D3D10_USAGE_DYNAMIC;
		vbDesc.ByteWidth = sizeof(DX10VERTEX)* numVertices; //total size of buffer in bytes
		vbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vbDesc.MiscFlags = 0;

		if (FAILED(pD3D10Device->CreateBuffer(&vbDesc, NULL, &pD3D10VertexBuffer))) return;

		// Set vertex buffer
		UINT stride = sizeof(DX10VERTEX);
		UINT offset = 0;
		pD3D10Device->IASetVertexBuffers(0, 1, &pD3D10VertexBuffer, &stride, &offset);

		//create the INDEX BUFFER for the plane 
		numIndices = 6; //2 triangles (3 verts each) for a plane


		D3D10_BUFFER_DESC ibDesc;
		ibDesc.Usage = D3D10_USAGE_DYNAMIC;
		ibDesc.ByteWidth = sizeof(unsigned int)*numIndices;
		ibDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		ibDesc.MiscFlags = 0;

		if (FAILED(pD3D10Device->CreateBuffer(&ibDesc, NULL, &pD3D10IndexBuffer))) return;

		pD3D10Device->IASetIndexBuffer(pD3D10IndexBuffer, DXGI_FORMAT_R32_UINT, offset);

		//initialize the VERTEX BUFFER 
		UINT numVertices = 4;
		DX10VERTEX* v = NULL;

		//lock vertex buffer for CPU use
		pD3D10VertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&v);

		v[0] = DX10VERTEX(XMFLOAT3(planeXOrig, planeYOrig, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0.0f, 1.0f));
		v[1] = DX10VERTEX(XMFLOAT3(planeXOrig + planeWidth, planeYOrig, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(1.0f, 1.0f));
		v[2] = DX10VERTEX(XMFLOAT3(planeXOrig + planeWidth, planeYOrig + planeHeight, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(1.0f, 0.0f));
		v[3] = DX10VERTEX(XMFLOAT3(planeXOrig, planeYOrig + planeHeight, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0.0f, 0.0f));

		pD3D10VertexBuffer->Unmap();

		//initialize the INDEX BUFFER 
		int* i = NULL;
		pD3D10IndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&i);

		i[0] = 0;
		i[1] = 1;
		i[2] = 2;
		i[3] = 0;
		i[4] = 2;
		i[5] = 3;

		pD3D10IndexBuffer->Unmap();

		//set up rasterizer flags 
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
		pD3D10Device->CreateRasterizerState(&rasterizerState, &pRS);
		pD3D10Device->RSSetState(pRS);

		pD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//set the matrices in the effect
		//set effect matrices
		XMMATRIX xmatTWorld = XMMatrixIdentity();
		XMStoreFloat4x4(&matWorld, xmatTWorld);
		//D3DXMatrixIdentity(&matWorld);

		pWorldMatrixEffectVariable->SetMatrix((float*)&matWorld);
		pViewMatrixEffectVariable->SetMatrix((float*)&matView);
		pProjectionMatrixEffectVariable->SetMatrix((float*)&matProjection);

		//create our texture (or load it)
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = D3D10SwapChainDesc.BufferDesc.Width;
		desc.Height = D3D10SwapChainDesc.BufferDesc.Height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//D3D10SwapChainDesc.BufferDesc.Format;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D10_USAGE_DYNAMIC;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;


		HRESULT hr;
		if (FAILED(hr = pD3D10Device->CreateTexture2D(&desc, 0, &texture)))
		{
			return;
		}

		//create shader resource view for the texture
		D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;

		pD3D10Device->CreateShaderResourceView(texture, NULL, &textureSRV);
		textureSV = pD3D10Effect->GetVariableByName("tex2D")->AsShaderResource();
		textureSV->SetResource(textureSRV);


		//set up alpha blending for transparent textures 

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

		pD3D10Device->CreateBlendState(&BlendState, &pBlendState);
		pD3D10Device->OMSetBlendState(pBlendState, 0, 0xffffffff);


		//Local pipeline intializations
		MATRIX2D iden;
		MatrixIdentity(&iden);

		cameraTransform = iden;
		projectionTransform = iden;

		//default clip region set to whole window
		clipRectangle.setPos(0, 0);
		clipRectangle.setWidth(clientRect.right);
		clipRectangle.setHeight(clientRect.bottom);


		drawOffsetX = drawOffsetY = 0.0f;
	}

	SRasterizer::~SRasterizer()
	{
		if (pBlendState) pBlendState->Release();
		if (textureSRV) textureSRV->Release();
		if (texture) texture->Release();
		if (pD3D10IndexBuffer) pD3D10IndexBuffer->Release();
		if (pD3D10VertexBuffer) pD3D10VertexBuffer->Release();
		if (pInputLayout) pInputLayout->Release();
		if (pD3D10Effect) pD3D10Effect->Release();
		if (pD3D10RenderTargetView) pD3D10RenderTargetView->Release();
		if (pD3D10InfoQueue) pD3D10InfoQueue->Release();
		if (pD3D10SwapChain) pD3D10SwapChain->Release();
		if (pD3D10Device) pD3D10Device->Release();

		pBlendState = NULL;
		textureSRV = NULL;
		texture = NULL;
		pD3D10IndexBuffer = NULL;
		pD3D10VertexBuffer = NULL;
		pInputLayout = NULL;
		pD3D10Effect = NULL;
		pD3D10RenderTargetView = NULL;
		pD3D10InfoQueue = NULL;
		pD3D10SwapChain = NULL;
		pD3D10Device = NULL;
	}
	void SRasterizer::DrawLine(DWORD* buffer, int buffer_width, int buffer_height, int x0, int y0, int x1, int y1, DWORD color0, DWORD color1, RECT* clipRect)
	{
		if (clipRect)
		{
			//clip line
			int clipValue = mCohenSutherlandLineClip(x0, y0, x1, y1, *clipRect);

			//line is invisible, don't draw
			if (clipValue == 0)
				return;
		}

		//which side is longer
		int xLen = abs(x1 - x0);
		int yLen = abs(y1 - y0);

		if (!((buffer_width > x1 && buffer_width > x0) && (buffer_height > y1 || buffer_height > y0)))
			return;

		if (xLen >= yLen)
		{

			//decode colors from DWORD
			int c1R, c1G, c1B, c1A;
			int c2R, c2G, c2B, c2A;
			mGetRGBA32FromDWORD(c1R, c1G, c1B, c1A, color0);
			mGetRGBA32FromDWORD(c2R, c2G, c2B, c2A, color1);

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

				buffer[x + int(y + 0.5f)*buffer_width] = color;
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
			mGetRGBA32FromDWORD(c1R, c1G, c1B, c1A, color0);
			mGetRGBA32FromDWORD(c2R, c2G, c2B, c2A, color1);

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
				buffer[int(x + 0.5) + y*buffer_width] = color;
				x += deltaX*incTerm;
				outR = (outR + deltaR);
				outG = (outG + deltaG);
				outB = (outB + deltaB);
				outA = (outA + deltaA);


			}

		}
	}
	void SRasterizer::DrawBitmapWithClipping(SoftwareBitmap::Bitmap* source, int destPosX, int destPosY, RECT* sourceRegion)
	{

		//clear render target
		pD3D10Device->ClearRenderTargetView(pD3D10RenderTargetView, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));

		//Lock the texture and draw
		D3D10_MAPPED_TEXTURE2D mappedTex;
		texture->Map(D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex);

		//clear texture and assign to temporary variable
		UINT lpitch32 = mappedTex.RowPitch >> 2; //in bytes, div by 4 to get num dwords
		DWORD* texture_buffer = (DWORD*)mappedTex.pData;
		int pitch = mappedTex.RowPitch;
		RECT clientR; 
		GetClientRect(mainWindow, &clientR);
		ZeroMemory(texture_buffer, (clientR.bottom - clientR.top)*mappedTex.RowPitch);

		//Draw to texture

		int image_width = source->GetInfoHeader().biWidth;
		int image_height = source->GetInfoHeader().biHeight;
		int offsetX = 0;
		int offsetY = 0;
		UINT byteCount = source->GetInfoHeader().biBitCount / 8;

		//use whole source image unless region is specified
		if (sourceRegion)
		{
			image_width = sourceRegion->right - sourceRegion->left;
			image_height = sourceRegion->bottom - sourceRegion->top;
			offsetX = sourceRegion->left;
			offsetY = sourceRegion->top;
		}
/*
		//reject off screen bitmap
		if (destPosX + image_width < 0)
			return;
		if (destPosY + image_height < 0)
			return;
		if (destPosX > clientRect.right)
			return;
		if (destPosY > clientRect.bottom)
			return;
*/

		//destination region
		int x1 = destPosX;
		int y1 = destPosY;
		int x2 = x1 + image_width;
		int y2 = y1 + image_height;

		if (x1 < 0)
			x1 = 0;
		if (y1 < 0)
			y1 = 0;
		if (x2 >= clientRect.right)
			x2 = clientRect.right - 1;
		if (y2 >= clientRect.bottom)
			y2 = clientRect.bottom - 1;

		//compute offset into region if clipped by window
		int reg_offsetX = x1 - destPosX;
		int reg_offsetY = y1 - destPosY;

		//establish starting points in memory for 32 bit bitmap
		if (byteCount == 4)
		{
			DWORD* sourceStartMem = ((DWORD*)source->GetData()) + ((offsetX + reg_offsetX) + ((offsetY + reg_offsetY) * image_width));
			DWORD* destStartMem = (texture_buffer)+(x1 + y1*lpitch32);

			int numColumns = (x2 - x1);
			int numRows = (y2 - y1);

			for (int row = 0; row < numRows; row++)
			{
				for (int column = 0; column < numColumns; column++)
				{

					destStartMem[column] = sourceStartMem[column];

				}

				destStartMem += lpitch32;
				sourceStartMem += image_width;
			}

		}
		else if (byteCount == 1)
		{


			UCHAR* sourceStartMem = (UCHAR*)source->GetData() + (offsetX + reg_offsetX) + (offsetY + reg_offsetY) * image_width;
			DWORD* destStartMem = (texture_buffer)+(x1 + y1*lpitch32);

			int numColumns = (x2 - x1);
			int numRows = (y2 - y1);

			for (int row = 0; row < numRows; row++)
			{
				for (int column = 0; column < numColumns; column++)
				{

					if (sourceStartMem[column] != 0)
						destStartMem[column] = _RGBA32BIT(int(sourceStartMem[column] + 0.5f), int(sourceStartMem[column] + 0.5f), int(sourceStartMem[column] + 0.5f), 255);

				}

				destStartMem += lpitch32;
				sourceStartMem += image_width;
			}



		}



		//Release the texture
		texture->Unmap(D3D10CalcSubresource(0, 0, 1));


		//apply pass and draw the main texture
		pD3D10EffectTechnique->GetPassByIndex(0)->Apply(0);
		pD3D10Device->DrawIndexed(numIndices, 0, 0);


		//end of frame
		pD3D10SwapChain->Present(0, 0);
		 

	}

	int SRasterizer::mCohenSutherlandLineClip(int& x1, int& y1, int& x2, int& y2, RECT clipRect)
	{
#define CLIP_CODE_CENTER	0x0000
#define CLIP_CODE_NORTH		0x0008
#define CLIP_CODE_SOUTH		0x0004
#define CLIP_CODE_EAST		0x0002
#define CLIP_CODE_WEST		0x0001
#define CLIP_CODE_NE		0x000A
#define CLIP_CODE_SE		0x0006
#define CLIP_CODE_NW		0x0009
#define CLIP_CODE_SW		0x0005

		//working values
		int p1X = x1;
		int p1Y = y1;
		int p2X = x2;
		int p2Y = y2;

		//codes, assume center region
		int p1_code = CLIP_CODE_CENTER, p2_code = CLIP_CODE_CENTER;

		//determine intersection regions
		//point 1
		if (p1X < clipRect.left)
			p1_code = p1_code | CLIP_CODE_WEST;
		else if (p1X > clipRect.right)
			p1_code = p1_code | CLIP_CODE_EAST;

		if (p1Y < clipRect.top)
			p1_code = p1_code | CLIP_CODE_NORTH;
		else if (p1Y > clipRect.bottom)
			p1_code = p1_code | CLIP_CODE_SOUTH;

		//point 2
		if (p2X < clipRect.left)
			p2_code = p2_code | CLIP_CODE_WEST;
		else if (p2X > clipRect.right)
			p2_code = p2_code | CLIP_CODE_EAST;

		if (p2Y < clipRect.top)
			p2_code = p2_code | CLIP_CODE_NORTH;
		else if (p2Y > clipRect.bottom)
			p2_code = p2_code | CLIP_CODE_SOUTH;


		//totally visible
		if (p1_code == 0 && p2_code == 0)
			return 1;


		//totally invisible
		if (p1_code == p2_code) //same region
			return 0;



		//calculate new coordinates
		switch (p1_code)
		{
		case CLIP_CODE_CENTER: break;

		case CLIP_CODE_NORTH:
		{
			if (y2 - y1 == 0)
				return 0;
			p1X = x1 + 0.5 + (clipRect.top - y1) * (x2 - x1) / (y2 - y1);
			p1Y = clipRect.top;
		}break;

		case CLIP_CODE_SOUTH:
		{

			if (y2 - y1 == 0)
				return 0;
			p1X = x1 + 0.5 + (clipRect.bottom - y1) * (x2 - x1) / (y2 - y1);
			p1Y = clipRect.bottom;
		}break;

		case CLIP_CODE_EAST:
		{
			if (x2 - x1 == 0)
				return 0;
			p1X = clipRect.right;
			p1Y = y1 + 0.5 + (clipRect.right - x1) * (y2 - y1) / (x2 - x1);
		}break;


		case CLIP_CODE_WEST:
		{
			//find y intersection with left vertical line
			if (x2 - x1 == 0)
				return 0;
			p1X = clipRect.left;
			p1Y = y1 + 0.5 + (clipRect.left - x1) * (y2 - y1) / (x2 - x1);
		}break;

		case CLIP_CODE_NE:
		{
			if (y2 - y1 == 0)
				return 0;
			p1X = x1 + 0.5 + (clipRect.top - y1) * (x2 - x1) / (y2 - y1);
			p1Y = clipRect.top;

			if (p1X < clipRect.left || p1X > clipRect.right)
			{
				if (x2 - x1 == 0)
					return 0;
				//east vline intersection
				p1X = clipRect.right;
				p1Y = y1 + 0.5 + (clipRect.right - x1) * (y2 - y1) / (x2 - x1);
			}

		}break;

		case CLIP_CODE_SE:
		{
			if (y2 - y1 == 0)
				return 0;
			p1X = x1 + 0.5 + (clipRect.bottom - y1) * (x2 - x1) / (y2 - y1);
			p1Y = clipRect.bottom;

			if (p1X < clipRect.left || p1X > clipRect.right)
			{
				if (x2 - x1 == 0)
					return 0;
				p1X = clipRect.left;
				p1Y = y1 + 0.5 + (clipRect.right - x1) * (y2 - y1) / (x2 - x1);

			}
		}break;
		case CLIP_CODE_NW:
		{
			if (y2 - y1 == 0)
				return 0;
			p1X = x1 + 0.5 + (clipRect.top - y1) * (x2 - x1) / (y2 - y1);

			if (p1X < clipRect.left || p1X > clipRect.right)
			{
				if (x2 - x1 == 0)
					return 0;
				p1X = clipRect.left;
				p1Y = y1 + 0.5 + (clipRect.left - x1) * (y2 - y1) / (x2 - x1);

			}
		}break;

		case CLIP_CODE_SW:
		{
			if (y2 - y1 == 0)
				return 0;
			p1X = x1 + 0.5 + (clipRect.bottom - y1) * (x2 - x1) / (y2 - y1);
			p1Y = clipRect.bottom;

			if (p1X < clipRect.left || p1X > clipRect.right)
			{
				if (x2 - x1 == 0)
					return 0;
				p1X = clipRect.left;
				p1Y = y1 + 0.5 + (clipRect.left - x1) * (y2 - y1) / (x2 - x1);

			}
		}break;

		default: break;

		}

		switch (p2_code)
		{
		case CLIP_CODE_CENTER: break;

		case CLIP_CODE_NORTH:
		{
			if (y1 - y2 == 0)
				return 0;
			p2X = x2 + (clipRect.top - y2) * (x1 - x2) / (y1 - y2);
			p2Y = clipRect.top;

		}break;

		case CLIP_CODE_SOUTH:
		{
			if (y1 - y2 == 0)
				return 0;
			p2X = x2 + (clipRect.bottom - y2) * (x1 - x2) / (y1 - y2);
			p2Y = clipRect.bottom;
		}break;

		case CLIP_CODE_WEST:
		{
			if (x1 - x2 == 0)
				return 0;
			p2X = clipRect.left;
			p2Y = y2 + (clipRect.left - x2) * (y1 - y2) / (x1 - x2);

		}break;

		case CLIP_CODE_EAST:
		{
			if (x1 - x2 == 0)
				return 0;
			p2X = clipRect.right;
			p2Y = y2 + (clipRect.right - x2) * (y1 - y2) / (x1 - x2);
		}break;

		case CLIP_CODE_NE:
		{
			if (y1 - y2 == 0)
				return 0;
			p2X = x2 + 0.5 + (clipRect.top - y2) * (x1 - x2) / (y1 - y2);
			p2Y = clipRect.top;

			if (p2X < clipRect.left || p2X > clipRect.right)
			{
				if (x1 - x2 == 0)
					return 0;
				p2X = clipRect.right;
				p2Y = y2 + 0.5 + (clipRect.right - x2) * (y1 - y2) / (x1 - x2);
			}
		}break;

		case CLIP_CODE_SE:
		{
			if (y1 - y2 == 0)
				return 0;
			p2X = x2 + 0.5 + (clipRect.bottom - y2) * (x1 - x2) / (y1 - y2);
			p2Y = clipRect.bottom;

			if (p2X < clipRect.left || p2X > clipRect.right)
			{
				if (x1 - x2 == 0)
					return 0;
				p2X = clipRect.right;
				p2Y = y2 + 0.5 + (clipRect.right - x2) * (y1 - y2) / (x1 - x2);

			}
		}break;

		case CLIP_CODE_NW:
		{
			if (y1 - y2 == 0)
				return 0;
			p2X = x2 + 0.5 + (clipRect.top - y2) * (x1 - x2) / (y1 - y2);
			p2Y = clipRect.top;

			if (p2X < clipRect.left || p2X > clipRect.right)
			{
				if (x1 - x2 == 0)
					return 0;
				p2X = clipRect.left;
				p2Y = y2 + 0.5 + (clipRect.left - x2) * (y1 - y2) / (x1 - x2);

			}
		}break;

		case CLIP_CODE_SW:
		{
			if (y1 - y2 == 0)
				return 0;
			p2X = x2 + 0.5 + (clipRect.bottom - y2) * (x1 - x2) / (y1 - y2);
			p2Y = clipRect.bottom;

			if (p2X < clipRect.left || p2X > clipRect.right)
			{
				if (x1 - x2 == 0)
					return 0;
				p2X = clipRect.left;
				p2Y = y2 + 0.5 + (clipRect.left - x2) * (y1 - y2) / (x1 - x2);


			}
		}break;

		default: break;


		}//end switch

		 //do bounds checking
		if (p1X < clipRect.left || p1X > clipRect.right ||
			p1Y < clipRect.top || p1Y > clipRect.bottom ||
			p2X < clipRect.left || p2X > clipRect.right ||
			p2Y < clipRect.top || p2Y > clipRect.bottom)
		{
			return 0;
		}


		//appy results
		x1 = p1X;
		y1 = p1Y;
		x2 = p2X;
		y2 = p2Y;

		return 1;

	}

	//bitwise extraction of the encoded color bytes
	void SRasterizer::mGetRGBA32FromDWORD(int& r, int& g, int& b, int& a, DWORD color)
	{
		a = (color >> 24);
		r = (color << 24) >> 24;
		g = (color << 16) >> 24;
		b = (color << 8) >> 24;
	}


	void MatrixIdentity(MATRIX2D* mat)
	{
		mat->_00 = 1.0f;
		mat->_01 = 0.0f;

		mat->_10 = 0.0f;
		mat->_11 = 1.0f;

	}



	VECTOR2D::VECTOR2D(float inX, float inY) : x(inX), y(inY) {}
	VECTOR2D::VECTOR2D() {}

	//convinent operators overloaded
	VECTOR2D VECTOR2D::operator+ (const VECTOR2D& rhs)
	{
		return VECTOR2D(x + rhs.x, y + rhs.y);
	}

	VECTOR2D VECTOR2D::operator- (const VECTOR2D& rhs)
	{
		return VECTOR2D(x - rhs.x, y - rhs.y);
	}

	VECTOR2D VECTOR2D::operator* (float scalar)
	{
		return VECTOR2D(x * scalar, y * scalar);
	}


	bool VECTOR2D::operator == (const VECTOR2D& rhs)
	{
		if (rhs.x == x && rhs.y == y)
			return true;

		return false;
	}

	void VECTOR2D::Normalize()
	{
		//formula: || v || = v / |v|

		//avoid a Length() call overhead, just calculate on the spot
		float len = sqrt(x*x + y*y);

		if (len == 0)
			return;

		x = x / len;
		y = y / len;

	}

	void VECTOR2D::MakeNegative()
	{
		x = -x;
		y = -y;
	}

	float VECTOR2D::Length()
	{
		// x^2 + y^2 = r^2, r = sqrt(x^2 + y^2)
		return sqrt(x*x + y*y);
	}

	float VECTOR2D::DistanceTo(VECTOR2D v)
	{
		VECTOR2D dir;
		dir = v - (*this);

		return dir.Length();
	}

	float VECTOR2D::AngleToVector(VECTOR2D& v)
	{
		return MATH_RADTODEG(atan((x*v.x + y*v.y) / (Length()*v.Length())));
	}

	float VECTOR2D::AngleToWorld()
	{

		float radANG;

		if (x == 0 && y == 0)
			return 0;

		if (x == 0 && y == -1)
			return -90;
		if (x == 0 && y == 1)
		{
			return 90;
		}
		if (x == 1 && y == 0)
		{
			return 0;
		}
		if (x == -1 && y == 0)
		{
			return -1;
		}

		if (x > 0 && y < 0)
		{
			return radANG = MATH_RADTODEG(atan(y / x));
		}
		else if (x < 0 && y < 0)
		{
			float a = -atan(x / y);
			a -= MATH_PI / 2;
			return MATH_RADTODEG(a);
		}
		else if (x > 0 && y > 0)
		{
			float a = atan(y / x);
			return MATH_RADTODEG(a);
		}
		else if (x < 0 && y > 0)
		{
			float a = atan(y / x);
			a += MATH_PI / 2;
			return MATH_RADTODEG(a);
		}

	}

	VECTOR2D VECTOR2D::UP_VECTOR()
	{

		VECTOR2D v;
		v.x = 0.0f;
		v.y = 1.0f;
		return v;

	}

	VECTOR2D VECTOR2D::LEFT_VECTOR()
	{
		VECTOR2D v;
		v.x = -1.0f;
		v.y = 0.0f;
		return v;
	}

	VECTOR2D VECTOR2D::RIGHT_VECTOR()
	{
		VECTOR2D v;
		v.x = 1.0f;
		v.y = 0.0f;
		return v;
	}

	VECTOR2D VECTOR2D::DOWN_VECTOR()
	{
		VECTOR2D v;
		v.x = 0.0f;
		v.y = -1.0f;
		return v;
	}

}