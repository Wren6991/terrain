#version 110

varying float depth;

void main()
{
	float ndepth = depth * 0.5 + 0.5;
	float rounded = int(ndepth * 256) / 256.0;
	gl_FragColor = vec4(rounded, ndepth - rounded, 0.0, 1.0);
}