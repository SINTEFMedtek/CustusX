#version 120
#extension GL_EXT_gpu_shader4 : enable
#pragma debug(on)

uniform sampler3D tex;
uniform samplerBuffer lut;
uniform int lutsize;
uniform float llr;	// low level reject

void main()
{
	vec4 idx = texture3D(tex, gl_TexCoord[0].xyz);
	int pos = int(idx.r * lutsize);
	vec4 col = texelFetchBuffer(lut, pos);

	if (col.r <= llr)
	{
		discard;
	}
	else
	{
		gl_FragColor = col;
	}
}
