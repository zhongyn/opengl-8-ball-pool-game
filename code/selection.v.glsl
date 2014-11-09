in vec3 select_v_coord;
uniform mat4 select_pro;
uniform mat4 select_m;
uniform mat4 select_v;


void main()
{
	gl_Position = select_pro*select_v*select_m*vec4(select_v_coord,1.0);
}