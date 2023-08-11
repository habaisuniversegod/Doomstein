#version 400

uniform vec3 cameraPos;
uniform float cameraLook;
uniform vec2 screenSize;
uniform float textureSize;
uniform float projFOV;
uniform vec2 projSize;
uniform float wallHeight;
uniform sampler2D floorTex;
uniform sampler2D ceilTex;
uniform float fogBase;

float fogColor(float distance){
	return pow(fogBase, distance);

}
void main(){
	vec2 leftRay = vec2(cos(cameraLook - projFOV / 2.0f), sin(cameraLook - projFOV / 2.0f)) / cos(projFOV / 2.0f) + cameraPos.xy;
	vec2 rightRay = vec2(cos(cameraLook + projFOV / 2.0f), sin(cameraLook + projFOV / 2.0f)) / cos(projFOV / 2.0f) + cameraPos.xy;

	vec2 stepW = (rightRay - leftRay) / screenSize.x;
	vec2 toPoint = leftRay + (stepW * gl_FragCoord.x) - cameraPos.xy;

	float stepH = projSize.y / 2 / (screenSize.y / 2);
	vec2 uvOffsetF = cameraPos.xy + toPoint * (cameraPos.z / (stepH * ((screenSize.y / 2) - gl_FragCoord.y)));
	vec2 uvOffsetC = cameraPos.xy + toPoint * ((wallHeight - cameraPos.z) / (-stepH * ((screenSize.y / 2) -gl_FragCoord.y)));

	float distF = length(uvOffsetF - cameraPos.xy);
	float distC = length(uvOffsetC - cameraPos.xy);

	if (gl_FragCoord.y < screenSize.y / 2){
		gl_FragColor = vec4(texture(floorTex, mod(uvOffsetF, textureSize) / textureSize).rgb, fogColor(distF));
	}
	else {
		gl_FragColor = vec4(texture(ceilTex, mod(uvOffsetC, textureSize) / textureSize).rgb, fogColor(distC));
	}
}
