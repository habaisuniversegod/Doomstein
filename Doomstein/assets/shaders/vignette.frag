uniform vec2 resolution;
uniform float exp;
uniform float radius;
uniform float damage;

void main(){
	vec2 current = gl_FragCoord.xy / resolution;
	float distance_to_center = length(vec2(0.5) - current) * 2.0 / radius;
	float alpha = (damage / 2.0f) + pow(distance_to_center, exp) * (1.0 - (damage / 2.0f));
	gl_FragColor = vec4(damage * 1.33, damage / 12.0, 0.0, alpha);
}