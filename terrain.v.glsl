#version 110

attribute vec3 v_pos;
attribute vec3 v_normal;

varying vec3 normal;
varying vec3 pos;

const float radius = 10000;

void main()
{
	gl_Position = gl_ProjectionMatrix * vec4(/*v_pos.x, v_pos.y - radius + sqrt(radius * radius - v_pos.x * v_pos.x - v_pos.z * v_pos.z), v_pos.z*/v_pos, 1.0);
	normal = normalize(v_normal);
	pos = v_pos;
}