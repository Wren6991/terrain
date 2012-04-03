#version 110

varying vec3 pos;
uniform vec3 playerpos;
uniform float time;
uniform vec3 lightdir;

void main()
{
	vec3 normal = normalize(vec3(sin(pos.x / 8 + sin(pos.z - time) * 2) * 0.02 + sin(pos.x * 0.74 + time) * 0.02 + sin(pos.x * 2.3 - time * 2) * 0.01, 0.7, cos(pos.z / 7.9 + sin(pos.x) + sin(time)) * 0.02 + sin(pos.z * 0.9 + pos.x) * sin(time * 2) * 0.03 + sin(pos.z * 2.5 + cos(time * 5)) * 0.01));
	vec3 eyedir = normalize(playerpos - pos);
	vec4 watercolor = mix(vec4(0.5, 0.8, 0.9, 0.7), vec4(0.3, 0.4, 1.0, 0.7), dot(eyedir, normal));
	gl_FragColor = vec4(watercolor.rgb * (0.9 + dot(normal, lightdir) * 0.1) + pow(dot(normalize(eyedir + lightdir), normal), 30) * vec3(0.7, 0.7, 0.65), watercolor.a);
}