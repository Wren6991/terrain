#version 110

uniform sampler2D grass, rock, sand;

varying vec3 normal;
varying vec2 texcoord;
varying float height;

const vec3 lightdir = normalize(vec3(1, 1, 1));
const vec3 ambient = vec3(0.2, 0.2, 0.3);
const vec3 diffuse = vec3(1.0, 0.95, 0.9);

void main()
{
	gl_FragColor = vec4(mix(
		mix(texture2D(grass, texcoord).rgb, texture2D(rock, texcoord).rgb, smoothstep(5, 7, height)), 
		texture2D(rock, texcoord).rgb, 
		0)//min((sqrt(normal.x * normal.x + normal.z * normal.z) + 1) * 0.5, 1.0))
		 *  mix(ambient, diffuse, dot(normalize(normal), lightdir)), 1.0);
	//gl_FragColor = vec4(normalize(normal), 1.0); 
}