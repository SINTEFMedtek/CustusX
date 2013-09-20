uniform sampler2D colors;
uniform sampler2D depth;
void main()
{
	vec4 start = gl_TexCoord[0];
	gl_FragColor = texture2D(colors, start.xy);
	gl_FragDepth = texture2D(depth, start.xy).r;
}
