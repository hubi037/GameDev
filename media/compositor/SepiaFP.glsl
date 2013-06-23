#version 130

uniform sampler2D RT;
varying vec2 uv0;
varying vec2 uv1;

void main()
{
	vec4 color = texture(RT, uv0);
 
    vec4 outputColor = color;
    outputColor.r = (color.r * 0.393) + (color.g * 0.769) + (color.b * 0.189);
    outputColor.g = (color.r * 0.349) + (color.g * 0.686) + (color.b * 0.168);    
    outputColor.b = (color.r * 0.272) + (color.g * 0.534) + (color.b * 0.131);

	gl_FragColor = outputColor;
}
