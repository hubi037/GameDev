sampler2D RT : register(s0);

float4 Sepia_ps (float4 pos : POSITION, float2 iTexCoord : TEXCOORD0) : COLOR
{
	float4 color = tex2D(RT, iTexCoord);
 
    float4 outputColor = color;
    outputColor.r = (color.r * 0.393) + (color.g * 0.769) + (color.b * 0.189);
    outputColor.g = (color.r * 0.349) + (color.g * 0.686) + (color.b * 0.168);    
    outputColor.b = (color.r * 0.272) + (color.g * 0.534) + (color.b * 0.131);

	return outputColor;
}
