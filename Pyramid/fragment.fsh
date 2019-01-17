#version 330

smooth in vec3 outColor;

out vec4 FragColor;

void main (void)
{
	FragColor = vec4 (outColor, 1.0);
}