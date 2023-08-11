#version 150

uniform vec2 screen_size;
uniform sampler2D TX;

uniform vec2 offset;

void main(){
	vec2 red_offset = -offset;
	vec2 green_offset = vec2(0.0);
	vec2 blue_offset = offset;

	gl_FragColor.r = texture(TX, (gl_FragCoord.xy + red_offset) / screen_size).r;
	gl_FragColor.g = texture(TX, (gl_FragCoord.xy + green_offset) / screen_size).g;
	gl_FragColor.b = texture(TX, (gl_FragCoord.xy + blue_offset) / screen_size).b;
	gl_FragColor.a = 1.0;
}