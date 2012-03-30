#version 110

varying vec3 normal;
varying vec2 texcoord;
varying float height;

const vec3 lightdir = normalize(vec3(1, -1, 1));
const vec3 ambient = vec3(0.2, 0.2, 0.3);
const vec3 diffuse = vec3(1.0, 0.95, 0.9);

void main()
{
	gl_FragColor = vec4(mix(vec3(0.0, 0.0, 1.0), vec3(0.0, 0.9, 0.0), step(5, height)) * mix(ambient, diffuse, dot(normalize(normal), lightdir)), 1.0);
}