#version 110

attribute vec3 pos;

varying float depth;

void main()
{
	gl_Position = gl_ProjectionMatrix * vec4(pos, 1.0);
	depth = gl_Position.z;
}