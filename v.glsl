#version 110

attribute vec3 pos;
attribute vec3 v_normal;

varying vec3 normal;
varying vec2 texcoord;
varying float height;

void main()
{
	gl_Position = gl_ProjectionMatrix * vec4(/*pos.x, pos.y - sqrt(10000000 - pos.x * pos.x - pos.z * pos.z), pos.z*/pos, 1.0);
	normal = normalize(gl_NormalMatrix * v_normal);
	height = pos.y;
}