#version 150

uniform vec2 screen_size;
uniform sampler2D TX;

uniform float pixelSize;

void main(){
	vec2 newPos = round(gl_FragCoord.xy / pixelSize) * pixelSize / screen_size;
	gl_FragColor = round(texture(TX, newPos) / (pixelSize / screen_size.y)) * (pixelSize / screen_size.y);
}