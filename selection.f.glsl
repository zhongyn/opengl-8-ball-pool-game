uniform int ballID;

void main()
{
	gl_FragColor = vec4(ballID/255.0, 0.0, 0.0, 1.0);
}