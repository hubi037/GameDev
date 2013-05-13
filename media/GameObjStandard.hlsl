void main_plain_texture_vp(
        // Vertex Inputs
        float4 position        : POSITION,    // Vertex position in model space
        float2 texCoord0    : TEXCOORD0,    // Texture UV set 0
		float4 normal:        NORMAL, 
 
        // Outputs
        out float4 oPosition    : POSITION,    // Transformed vertex position
		out float4 oPositionO	: TEXCOORD1,
		out float4 oNormal		: TEXCOORD2,
        out float2 uv0        : TEXCOORD0,    // UV0
 
        // Model Level Inputs
        uniform float4x4 worldViewProj)
{
    // Calculate output position
    oPosition = mul(worldViewProj, position);
	oNormal = normal;
	oPositionO = position;
 
    // Simply copy the input vertex UV to the output
    uv0 = texCoord0;
}

static const float PI = 3.14159265f;

float2 sphere(float3 U, float3 N)
{
	return N.xz;
	/*float3 R;
	R = reflect(U, N);
	R.z += 1.0;
	R = normalize(R);
	return R.xy * 0.5 + 0.5;*/
}
 
void main_plain_texture_fp(
        // Pixel Inputs
        float2 uv0        : TEXCOORD0,    // UV interpolated for current pixel
		float3 p	: TEXCOORD1,
		float4 n	: TEXCOORD2,
 
        // Outputs
        out float4 color    : COLOR,    // Output color we want to write
 
		uniform float3 cameraPosition_OS,
		uniform float4 colorModulate,
        // Model Level Inputs
        uniform sampler2D tex : register(s0))        // Texture we're going to use
	   //uniform samplerCUBE tex :   TEXUNIT0
{
    // Just sample texture using supplied UV
    color = tex2D(tex, uv0);
	float3 R;
	R = reflect(normalize(p - cameraPosition_OS), normalize(n));
	R.z += 1.0;
	R = normalize(R);
	//color = tex2D(tex, R.xy * 0.5 + 0.5);
	//color = tex2D(tex, float2(0.5,-0.5) * n.xy + float2(0.5, 0.5));
	//color = texCUBE(tex, normalize(n));

	float2 uv;
	uv.x = asin(n.x)/PI + 0.5 ;
	uv.y = asin(n.y)/PI + 0.5 ;
	color = tex2D(tex, asin(n)/PI + 0.5 );
	color.a *= colorModulate.a;
}