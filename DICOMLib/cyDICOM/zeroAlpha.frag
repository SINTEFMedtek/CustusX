sampler3D tex;
uniform float brightness;
uniform float contrast;
uniform float reject;

void main()
{
	vec4 col = texture3D(tex, gl_TexCoord[0].xyz);
	float lowest;

	if ( reject < contrast )
	{
		lowest = contrast;
	}
	else
	{
		lowest = reject;
	}

	if ( col.r <= lowest || col.r >= 1.0 - contrast )
	{
		discard;
	}
	else
	{
		gl_FragColor = col * brightness;
	}
}
