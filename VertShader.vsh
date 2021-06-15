attribute highp   vec4  inVertex;

uniform mediump mat4  MVPMatrix;

void main()
{
	gl_Position = MVPMatrix * inVertex;
}
