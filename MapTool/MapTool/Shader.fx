Texture2D texDiffuse;
SamplerState samLinear;

cbuffer ConstantBuffer
{
	float4x4 wvp;
	float4x4 world;

	float4 lightDir;
	float4 lightColor;
}; 

struct VertexIn
{
	float3 pos : POSITION;
	float4 color : COLOR;

	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

struct VertexOut
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;

	float4 normal : NORMAL;
	float2 tex : TEXCOORD;
};


VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;

	vOut.pos = mul(float4(vIn.pos, 1.0f), wvp);
	vOut.color = vIn.color;
	vOut.normal = mul(float4(vIn.normal, 0.0f), world);
	vOut.tex = vIn.tex;

	return vOut;
}


float4 PS(VertexOut vOut) : SV_TARGET
{
	float4   finalColor = 0;
	half hLambert = dot(vOut.normal, -lightDir)*0.5 + 0.5;
	finalColor = saturate(hLambert * lightColor);
	
	float4 texColor = texDiffuse.Sample(samLinear, vOut.tex) * finalColor;
	texColor.a = 1.0f;

	//return  texColor;
	return texDiffuse.Sample(samLinear, vOut.tex);
}

