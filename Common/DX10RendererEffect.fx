matrix World;
matrix View;
matrix Projection;
Texture2D tex2D;
Texture2D spriteTex;
Texture2D uiTexForeground;
Texture2D uiTexBackground;

SamplerState linearSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};


PS_INPUT VS_MAIN(float4 Pos : POSITION, float4 Color : COLOR, float2 TexCoord : TEXCOORD)
{
	PS_INPUT psInput;

	psInput.Pos = Pos;
	psInput.Color = Color;
	psInput.TexCoord = TexCoord;

	return psInput;
}


float4 PS_MAIN(PS_INPUT psInput) : SV_Target
{
	float4 c = tex2D.Sample(linearSampler, psInput.TexCoord);
	return c;
}

//sprite-----------------------------------------------------------------------


PS_INPUT VS_SPRITE(float4 Pos : POSITION, float4 Color : COLOR, float2 TexCoord : TEXCOORD)
{
	PS_INPUT psIn;
	float4 tPos;
	float4 finPos;
	tPos = mul(Pos, World);
	finPos = mul(tPos, Projection);
	finPos.z = 0.5f;
	psIn.Pos = finPos;
	psIn.Color = Color;
	psIn.TexCoord = TexCoord;

	return psIn;
}

float4 PS_SPRITE(PS_INPUT psInput) : SV_Target
{
	float4 c = spriteTex.Sample(linearSampler, psInput.TexCoord);
	return c;
}

//user interface-------------------------------------------------------------------------
PS_INPUT VS_UI(float4 Pos: POSITION, float4 Color : COLOR, float2 TexCoord : TEXCOORD)
{
	PS_INPUT psIn;
	psIn.Pos = Pos;
	
	psIn.Color = Color;
	psIn.TexCoord = TexCoord;

	return psIn;
}

float4 PS_UI_FOREGROUND(PS_INPUT psInput) : SV_Target
{
	return uiTexForeground.Sample(linearSampler, psInput.TexCoord);
}

float4 PS_UI_BACKGROUND(PS_INPUT psInput) : SV_Target
{
	return uiTexBackground.Sample(linearSampler, psInput.TexCoord);
}

technique10 Render
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS_MAIN()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS_MAIN()));
	}

	pass SpritePass
	{
		SetVertexShader(CompileShader(vs_4_0, VS_SPRITE()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS_SPRITE()));
	}

	pass UIForegroundPass
	{
		SetVertexShader(CompileShader(vs_4_0, VS_UI()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS_UI_FOREGROUND()));
	}

	pass UIBackgroundPass
	{
		SetVertexShader(CompileShader(vs_4_0, VS_UI()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS_UI_BACKGROUND()));
	}

}