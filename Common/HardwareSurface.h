#include <D3D10.h>
#include <DirectXMath.h>
#include <vector>
#include <windows.h>
#include "SoftwareRasterizer.h"

class HardwareSurface
{
public:
	HardwareSurface(int inWidht, int inHeight, int numFrames = 1);

	~HardwareSurface();

	void SetTransform(DirectX::XMFLOAT4X4& mat);
	void LockFrame(int frameNum, DWORD** vid, int* lpitch32);
	void UnlockFrame(int frameNum)
	{
		if (frameNum == 0)
			texture->Unmap(D3D10CalcSubresource(0, 0, 1));
		else
			textureFlipBook[frameNum]->Unmap(D3D10CalcSubresource(0, 0, 1));

	}

	//all hardware, place with other hardware calls
	void Draw();


private:
	ID3D10Texture2D* texture;
	std::vector<ID3D10Texture2D*> textureFlipBook;
	int currentFrame;

	ID3D10ShaderResourceView*	textureSRV;
	ID3D10EffectShaderResourceVariable* textureSV;
	DirectX::XMFLOAT4X4 worldTransform;

	//common to all surfaces
	static int instanceCount;
	static 	ID3D10Buffer*	pD3D10VertexBuffer;
	static  ID3D10Buffer*	pD3D10IndexBuffer;

};
#pragma once
