#version 110

varying vec3 pos;
uniform vec3 playerpos;

const vec3 lightdir = normalize(vec3(-1, 1, -1));


void main()
{
	vec3 normal = normalize(vec3(sin(pos.x / 8 + sin(pos.z) * 2) * 0.02 + sin(pos.x * 0.74) * 0.02 + sin(pos.x * 2.3) * 0.01, 0.7, cos(pos.z / 7.9 + sin(pos.x)) * 0.02 + sin(pos.z * 0.9 + pos.x) * 0.02 + sin(pos.z * 2.5) * 0.01));
	vec4 watercolor = mix(vec4(0.5, 0.8, 0.9, 0.7), vec4(0.3, 0.4, 1.0, 0.7), dot(normalize(playerpos - pos), normal));
	gl_FragColor = vec4(watercolor.rgb * (0.9 + dot(normal, lightdir) * 0.1) + pow(dot(normalize(normalize(playerpos - pos) + lightdir), normal), 30) * vec3(0.7, 0.7, 0.65), watercolor.a);
}