#pragma once
#include <Windows.h>
#include "..\SoftwareBitmap\Bitmap.h"
#include <D3D10.h>
#include <DirectXMath.h>
#include "..\D3DX\D3DX10.h"
using namespace DirectX;



namespace SoftwareRasterizer
{

	const float MATH_PI = 3.1415926f;
#define _RGBA32BIT(r, g, b, a) ( (r & 255) + ((g & 255) << 8) + ((b & 255) << 16)  + ((a & 255) << 24) )
#define MATH_DEGTORAD(degrees) (degrees * MATH_PI/180)
#define MATH_RADTODEG(rad) (rad * 180/MATH_PI);



	struct DX10VERTEX
	{
		DX10VERTEX() {}
		DX10VERTEX(XMFLOAT3 p, XMFLOAT4 c, XMFLOAT2 uv)
		{
			pos = p;
			color = c;
			texCoord = uv;
		}

		XMFLOAT3 pos;
		XMFLOAT4 color;
		XMFLOAT2 texCoord;

	};

	struct VECTOR2D
	{
	public:

		VECTOR2D(float inX, float inY);
		VECTOR2D();

		//convinent operators overloaded
		VECTOR2D operator+ (const VECTOR2D& rhs);

		VECTOR2D operator- (const VECTOR2D& rhs);

		VECTOR2D operator* (float scalar);


		bool operator == (const VECTOR2D& rhs);

		void Normalize();
		void MakeNegative();
		float Length();
		float DistanceTo(VECTOR2D v);
		float AngleToVector(VECTOR2D& v);
		float AngleToWorld();

		float x;
		float y;

		static VECTOR2D DOWN_VECTOR();
		static VECTOR2D UP_VECTOR();
		static VECTOR2D LEFT_VECTOR();
		static VECTOR2D RIGHT_VECTOR();


	};





	class RECT2D
	{
	public:

		RECT2D() :left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
		RECT2D(float x, float y, float x1, float y1) : left(x), top(y), right(x1), bottom(y1) {}
		RECT2D(RECT& r) {
			left = r.left;
			right = r.right;
			top = r.top;
			bottom = r.bottom;
		}
		void setWidth(float w)
		{
			right = left + w;

		}

		void setHeight(float h)
		{
			bottom = top + h;
		}

		void setPos(float x, float y)
		{
			right = (right - left) + x;
			bottom = (bottom - top) + y;
			left = x;
			top = y;

		}

		void SetRECT(RECT r)
		{
			left = r.left;
			top = r.top;
			right = r.right;
			bottom = r.bottom;
		}

		void setPos(VECTOR2D& v)
		{
			right = (right - left) + v.x;
			bottom = (bottom - top) + v.y;
			left = v.x;
			top = v.y;
		}

		float getWidth() const
		{
			return right - left;
		}

		float getHeight() const
		{
			return bottom - top;
		}

		void move(VECTOR2D dir, int pxPerSec, float deltaTime)
		{
			left = left + pxPerSec*dir.x * deltaTime;
			right = right + pxPerSec*dir.x * deltaTime;
			top = top + pxPerSec*dir.y * deltaTime;
			bottom = bottom + pxPerSec*dir.y * deltaTime;
		}

		VECTOR2D GetPosition() const
		{
			VECTOR2D v;
			v.x = left;
			v.y = top;

			return v;
		}

		inline bool isPointInRect(float x, float y)
		{
			//reverse signs if the region is negative (made from right to left)
			if (left > right)
			{
				if (x <= left && x >= right)
					if (top > bottom)
					{
						if (y <= top && y >= bottom)
							return true;
					}
					else
					{
						if (y >= top && y <= bottom)
							return true;
					}
			}

			if (x >= left && x <= right)
			{
				if (top > bottom)
				{
					if (y <= top && y >= bottom)
						return true;
				}
				else
				{

					if (y >= top && y <= bottom)
						return true;
				}
			}

			return false;
		}

		bool isRectInRect(RECT& region)
		{
			//test x dimension
			if (region.left <= left)
			{

				if (region.right > left)
				{
					//now check y dimension
					if (region.top <= top)
					{
						if (region.bottom > top)
							return true;
						else return false;
					}
					else if (region.top > top)
					{
						if (region.top < bottom)
							return true;
						else
							return false;
					}
				}
				else
					return false;

			}
			else if (region.left > left)
			{
				if (region.left < right)
				{
					//now check y dimension
					if (region.top <= top)
					{
						if (region.bottom > top)
							return true;
						else return false;
					}
					else if (region.top > top)
					{
						if (region.top < bottom)
							return true;
						else
							return false;
					}
				}
				else
					return false;
			}

			return false;
		}

		RECT GetWINRECT()
		{
			RECT r;
			r.left = int(left + 0.5f);
			r.right = int(right + 0.5f);
			r.top = int(top + 0.5f);
			r.bottom = int(bottom + 0.5f);

			return r;
		}

	public:
		float left;
		float top;
		float right;
		float bottom;

	};






	typedef struct MATRIX2x2_TYP
	{
		union
		{
			float M[2][2]; //array index

			struct
			{
				float _00, _01;
				float _10, _11;
			};
		};

	}MATRIX2x2, MATRIX2D, *MATRIX2x2_PTR, *MATRIX2D_PTR;


	void MatrixIdentity(MATRIX2D* mat);

	class SRasterizer
	{
	public:
		SRasterizer(HWND Window);
		~SRasterizer();


		void DrawLine(DWORD* buffer, int lpitch32, int height, int x0, int y0, int x1, int y1, DWORD color0, DWORD color1, RECT* clipRect = NULL);
		void DrawBitmapWithClipping(SoftwareBitmap::Bitmap* bitmap, int destPosX, int destPosY, RECT* sourceRegion = NULL);

	private:
		int mCohenSutherlandLineClip(int& x1, int& y1, int& x2, int& y2, RECT clipRect);
		void mGetRGBA32FromDWORD(int& r, int& g, int& b, int& a, DWORD color);


	private:
		ID3D10Device*				pD3D10Device;
		IDXGISwapChain*				pD3D10SwapChain;
		ID3D10RenderTargetView*     pD3D10RenderTargetView;
		ID3D10InputLayout*			pInputLayout;
		ID3D10Effect*				pD3D10Effect;
		ID3D10EffectTechnique*      pD3D10EffectTechnique;
		ID3D10Buffer*				pD3D10VertexBuffer;
		ID3D10Buffer*				pD3D10IndexBuffer;
		ID3D10InfoQueue*			pD3D10InfoQueue;
		ID3D10SamplerState*			pD3D10TextureSampler;
		ID3D10BlendState*			pBlendState;
		DXGI_SWAP_CHAIN_DESC		D3D10SwapChainDesc;
		D3D10_VIEWPORT				D3D10Viewport;

		ID3D10EffectMatrixVariable* pViewMatrixEffectVariable;
		ID3D10EffectMatrixVariable* pWorldMatrixEffectVariable;
		ID3D10EffectMatrixVariable* pProjectionMatrixEffectVariable;

		ID3D10ShaderResourceView*			textureSRV;
		ID3D10EffectShaderResourceVariable* textureSV;
		ID3D10Texture2D*					texture;

		XMFLOAT4X4	matView;
		XMFLOAT4X4	matProjection;
		XMFLOAT4X4  matWorld;

		int			numVertices;
		int			numIndices;

		HWND		mainWindow;
		HINSTANCE	hInstance;
		RECT		clientRect;
		float		drawOffsetX, drawOffsetY;

		MATRIX2D cameraTransform;
		MATRIX2D projectionTransform;
		RECT2D   clipRectangle;
	};

}