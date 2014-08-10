
in vec3 v_coord;
in vec3 v_normal;
in vec2 v_texcoord;

uniform mat4 m;
uniform mat4 v;

uniform mat4 pro;
uniform vec4 lightPos;

out vec3 fN;
out vec3 fE;
out vec3 fL;

out vec2 texcoord;

void main()
{	
	mat4 mv = v*m;
	vec4 pos = mv*vec4(v_coord,1.0);
	fL = (v*lightPos-pos).xyz;
	fE = -pos.xyz;
	fN = (transpose(inverse(mv))*vec4(v_normal,1.0)).xyz;

	gl_Position = pro*pos;

	texcoord = v_texcoord;
}
