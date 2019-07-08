#version 330

in  vec4 vPosition;
in  vec4 vColor;
in  vec3 vNormal;
out vec4 color;
out vec3 N3;
out vec3 L3;
out vec3 V3;
out vec3 R3;

uniform mat4 uProjMat;
uniform mat4 uModelMat;
uniform vec4 uSpc;
uniform float uShininess;

void main()
{
	gl_Position  = uProjMat * (uModelMat * vPosition);
	gl_Position *= vec4(1,1,-1,1);	// z축 방향이 반대임
    
	vec4 N = uModelMat*vec4(vNormal,0);
	N3 = normalize(vec3(N.xyz));

	vec4 P = uModelMat * vPosition;
	vec4 Lpos = vec4(1,1,0,1);
	vec4 L = Lpos - P;
	L3 = normalize(vec3(L.xyz));

	R3 = 2*dot(L3, N3)*N3 - L3;	
	vec4 V = vec4(0,0,0,1);
	V3 = normalize(vec3((V - P).xyz));
	
	color = vColor;
}
