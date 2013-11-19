#version 120
const int layers = ${LAYERS};

uniform sampler3D texture[layers];
uniform sampler1D lut[layers];
uniform int lutsize[layers];
uniform float window[layers];
uniform float level[layers];
uniform float llr[layers];
uniform float alpha[layers];

const vec3 bounds_lo = vec3(0.0,0.0,0.0);
const vec3 bounds_hi = vec3(1.0,1.0,1.0);
varying vec4 gl_TexCoord[2*layers];

bool clampMe(int index)
{
	vec3 coord = gl_TexCoord[index].xyz;
	return any(lessThan(coord, bounds_lo)) || any(greaterThan(coord, bounds_hi));
}

float windowLevel(float x, float window_, float level_)
{
	return (x-level_)/window_  + 0.5;
}

/** Map Luminance volume layer N through a window/level/llr + lut
 *
 */
vec4 applyLutLayerN(in vec4 base,in int index)
{
	if (clampMe(2*index)) // ignore drawing outside texture
	{
		return base;
	}

	// get volume intensity value (same in all of rgb)
	float idx = texture3D(texture[index], gl_TexCoord[2*index].xyz).r;

	// ignore drawing below llr
	if (idx < llr[index])
	{
		return base;
	}

	// apply window+level transform
	idx = windowLevel(idx, window[index], level[index]);
	idx = clamp(idx, 0.0, 1.0);
	// map through lookup table - interpolated
	vec4 col = texture1D(lut[index], idx);
	col.a = alpha[index];

	col =  mix(base, col, alpha[index]);
	return col;
}

/** Map RGBA layer N through a window/level/llr
 *
 */
vec4 applyRGBALayerN(in vec4 base,in int index)
{
	if (clampMe(2*index)) // ignore drawing outside texture
	{
		return base;
	}

	// get rgb color from texture
	vec4 col = texture3D(texture[index], gl_TexCoord[2*index].xyz);

	// ignore drawing below llr
	if (all(lessThan(col.rgb, vec3(llr[index]))))
	{
		return base;
	}

	// apply window+level transform to each component
	col.r = windowLevel(col.r, window[index], level[index]);
	col.g = windowLevel(col.g, window[index], level[index]);
	col.b = windowLevel(col.b, window[index], level[index]);
	col.a = alpha[index];

	col =  mix(base, col, alpha[index]);
	return col;
}

/** Apply colors from layer N. Decide if it is type luminance or rgb.
 *
 */
vec4 applyLayerN(in vec4 base, in int index)
{
	if (lutsize[index]>0)
	{
		return applyLutLayerN(base,index);
	}
	else
	{
		return applyRGBALayerN(base,index);
	}
}


void main()
{
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);

    for (int i=0; i<layers; ++i)
    {
        col = applyLayerN(col, i);
    }

	gl_FragColor = col;
}
