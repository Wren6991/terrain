#version 110

uniform sampler2D rock;
uniform sampler2D sand;
uniform sampler2D grass;

varying vec3 normal;
varying vec2 texcoord;
varying float height;

const vec3 lightdir = normalize(vec3(1, 1, 1));
const vec3 ambient = vec3(0.3, 0.3, 0.4);
const vec3 diffuse = vec3(1.0, 0.95, 0.9);

void main()
{
	vec3 norm = normalize(normal);
	float slope = sqrt(norm.x * norm.x + norm.z * norm.z);
	gl_FragColor = vec4(mix(
		mix(texture2D(sand, texcoord).rgb, texture2D(grass, texcoord).rgb, smoothstep(5, 7, height)), 
		texture2D(rock, texcoord).rgb, 
		max(0, slope * 2 - 1))
		 *  mix(ambient, diffuse, dot(norm, lightdir)), 1.0);
	

}