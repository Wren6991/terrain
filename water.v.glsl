#version 110

varying vec3 pos;

void main()
{
	gl_Position = gl_ProjectionMatrix * gl_Vertex;
	pos = gl_Vertex.xyz;
}