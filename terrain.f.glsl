#version 110

uniform sampler2D rock;
uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D shadowmap;

uniform vec3 playerpos;
uniform vec3 lightdir;
uniform mat4 lightmatrix;

varying vec3 normal;
varying vec3 pos;

const vec3 ambient = vec3(0.3, 0.3, 0.4);
const vec3 diffuse = vec3(1.0, 0.95, 0.9);

float depthfromz(float z)
{
	float n = 1.0;
	float f = 3.0;
	
	return(2.0 * n) / (f + n - z * (f - n));	
}

void main()
{
	/*vec2 texcoord = pos.xz * 0.25;
	vec3 norm = normalize(normal);
	float slope = sqrt(norm.x * norm.x + norm.z * norm.z);
	gl_FragColor = vec4(mix(
		mix(texture2D(sand, texcoord).rgb, texture2D(grass, texcoord).rgb, smoothstep(5, 7, pos.y)), 
		texture2D(rock, texcoord).rgb, 
		max(0, slope * 2 - 1))
		 * mix(ambient, diffuse, dot(norm, lightdir))
		 + vec3(0.13, 0.13, 0.1) * pow(dot(normalize(normalize(playerpos - pos) + lightdir), normal), 4), 1.0);*/

	vec4 lightprojcoord = lightmatrix * vec4(pos, 1.0);
	gl_FragColor = vec4(vec3(texture2D(shadowmap, lightprojcoord.xy * 0.5 + 0.5).r, lightprojcoord.z * 0.5 + 0.5, 0.0), 1.0);
	

}