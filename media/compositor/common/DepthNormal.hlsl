void depthNormal_vs(	float4 position			: POSITION,
						float3 normal 			: NORMAL, 
						out float4 oPosition	: POSITION,
						out float2 oDepth		: TEXCOORD0,
						out float3 oNormal		: TEXCOORD1,
						uniform float4x4 wvpMat)
{
	oPosition = mul(wvpMat, position);
	oDepth.x = oPosition.z;
	oDepth.y = oPosition.w;
	oNormal = normal;
}

void depthNormal_ps(float2 depth			: TEXCOORD0,
					float3 normal			: TEXCOORD1,
					out float4 oColour		: COLOR)
{
	float finalDepth = depth.x / depth.y;
	oColour = float4(normal.x, normal.y, normal.z, finalDepth);
}