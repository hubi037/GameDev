#version 150

uniform sampler2D RT;
in vec2 oUv0;
in vec2 oUv1;
out vec4 fragColour;

void main()
{
    vec4 color = texture(RT, oUv0);
 
    vec4 outputColor = color;
    outputColor.r = (color.r * 0.393) + (color.g * 0.769) + (color.b * 0.189);
    outputColor.g = (color.r * 0.349) + (color.g * 0.686) + (color.b * 0.168);    
    outputColor.b = (color.r * 0.272) + (color.g * 0.534) + (color.b * 0.131);
 
   fragColour = outputColor;
}