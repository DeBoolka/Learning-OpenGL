#version 330

uniform float alpha;
uniform mat4 MVP;

in vec3 inColors;
in vec3 inVertex;

smooth out vec3 outColor;

void main (void)
{
	mat4 R2 = mat4 ( 
		cos(alpha), 0.0, -sin(alpha),	0.0,
		0.0,		1.0, 0.0,			0.0,
		sin(alpha), 0.0, cos(alpha),	0.0,
		0.0,		0.0, 0.0,			1.0
	);

	mat4 R1 = mat4 ( 
		cos(alpha), -sin(alpha),	 0.0, 0.0,
		sin(alpha), cos(alpha),	 	 0.0, 0.0,
		0.0,		0.0,			 1.0, 0.0,
		0.0,		0.0,			 0.0, 1.0
	);
	
	mat4 R3 = mat4 ( 
		abs(sin(alpha)),		0.0,  		0.0, 	0.0,
		0.0,		abs(sin(alpha)),	 	0.0, 	0.0,
		0.0,		0.0,		abs(sin(alpha)), 	0.0,
		0.0,		0.0,		0.0, 	1.0
	);

	outColor = inColors;
	//gl_Position = vec4 (inVertex, 1.0) * R2 * R1;
	gl_Position = MVP * R2 * vec4(inVertex, 1.0);
}
