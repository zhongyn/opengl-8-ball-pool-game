
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float shine;

in vec2 texcoord;
uniform sampler2D mytexture;

in vec3 fN;
in vec3 fE;
in vec3 fL;

void main()
{
	vec3 N = normalize(fN);
	vec3 E = normalize(fE);
	vec3 L = normalize(fL);

	vec3 H = normalize(L+E);

	vec4 color = texture2D(mytexture, texcoord);

	vec4 amb = ambient*color;

	float kd = max(dot(L,N),0.0); 
	vec4 dif = kd*color*diffuse;

	float ks = pow(max(dot(N, H), 0.0), shine);
	vec4 spe = ks*color*specular;
	if (dot(L,N) < 0.0)
	{
		spe = vec4(0.0, 0.0, 0.0, 1.0);
	}

	gl_FragColor = vec4((amb+dif+spe).xyz, 1.0);

}