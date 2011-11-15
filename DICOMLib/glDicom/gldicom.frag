#version 120
#extension GL_EXT_gpu_shader4 : enable
#pragma debug(on)

uniform sampler3D tex1, tex2, tex3, tex4;
uniform samplerBuffer lut1, lut2, lut3, lut4;
uniform vec4 llr1, llr2, llr3, llr4;	// low level reject
uniform int layers;
uniform int start1, start2, start3, start4;
uniform int lutsize1, lutsize2, lutsize3, lutsize4;
in vec4 gl_TexCoord[8];

void main()
{
	vec4 idx;
	int pos;
	vec4 col1 = vec4(-1.0f);
	vec4 col2 = vec4(-1.0f);
	vec4 col3 = vec4(-1.0f);
	vec4 col4 = vec4(-1.0f);
	vec4 final = vec4(0.0f);
	bvec4 result1, result2, result3, result4;

	// First layer
	idx = texture3D(tex1, gl_TexCoord[0].xyz);
	pos = int(idx.r * lutsize1);
	col1 = texelFetchBuffer(lut1, pos);

	if (layers > 1)
	{
		idx = texture3D(tex2, gl_TexCoord[2].xyz);
		pos = int(idx.r * lutsize2);
		col2 = texelFetchBuffer(lut2, pos);
	}
	if (layers > 2)
	{
		idx = texture3D(tex3, gl_TexCoord[4].xyz);
		pos = int(idx.r * lutsize3);
		col3 = texelFetchBuffer(lut3, pos);
	}
	if (layers > 3)
	{
		idx = texture3D(tex4, gl_TexCoord[6].xyz);
		pos = int(idx.r * lutsize4);
		col4 = texelFetchBuffer(lut4, pos);
	}

	result1 = greaterThan(col1, llr1);
	result2 = greaterThan(col2, llr2);
	result3 = greaterThan(col3, llr3);
	result4 = greaterThan(col4, llr4);

	if (any(result1))
	{
		gl_FragColor = col1;
	}
	else if (any(result2))
	{
		gl_FragColor = col2;
	}
	else if (any(result3))
	{
		gl_FragColor = col3;
	}
	else if (any(result4))
	{
		gl_FragColor = col4;
	}
	else
	{
		discard;
	}
}
