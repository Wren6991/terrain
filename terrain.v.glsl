#version 110

attribute vec3 pos;
attribute vec3 v_normal;

varying vec3 normal;
varying vec2 texcoord;
varying float height;

const float radius = 10000;

void main()
{
	gl_Position = gl_ProjectionMatrix * vec4(/*pos.x, pos.y - radius + sqrt(radius * radius - pos.x * pos.x - pos.z * pos.z), pos.z*/pos, 1.0);
	normal = normalize(v_normal);
	height = pos.y;
	texcoord = pos.xz * 0.25;
}