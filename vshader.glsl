#version 330

in  vec4 vPosition;
in  vec4 vColor;
in	vec4 vNormal;
out vec4 color;

uniform mat4 uMat;
uniform vec4 uColor;
uniform mat4 uModelMat;

void main()
{
	gl_Position  = uMat*vPosition;
	gl_Position *= vec4(1,1,-1,1);	// z�� ������ �ݴ���
    
	if(uColor.w<1)
		color = vColor;
	else 
		color = uColor;
}
