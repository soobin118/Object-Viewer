#version 330

in  vec4 color;
in  vec3 N3;
in  vec3 L3;
in  vec3 R3;
in  vec3 V3;

out vec4 fColor;

uniform float uShininess;
uniform vec4 uSpc;
void main()
{
	vec4 Il = vec4(1,1,1,1);
	vec4 IAmb = vec4(0.5,0.5,0.5,1);
	vec4 ka = vec4(0.3,0.3,0.5,1);
	vec4 kd = color;
	vec4 ks = uSpc;

							// color = amb + dif + spec
							// I = ka*Il + kd*Il*(N.L)

	float dif = max(dot(N3,L3),0);

							// L3 + R3 = 2*dot(N3, L3)*N3
	float gloss = uShininess;
	float spec = pow(max(dot(V3, R3),0), gloss);
	
	fColor = ka*IAmb + kd*Il*dif + ks*Il*spec;
}
