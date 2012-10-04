#version 120
#extension GL_EXT_gpu_shader4 : enable
#pragma debug(on)
uniform int layers;
uniform sampler3D texture0,texture1,texture2,texture3,texture4;
uniform samplerBuffer lut0,lut1,lut2,lut3,lut4;
uniform int lutsize0,lutsize1,lutsize2,lutsize3,lutsize4;
uniform float window0,window1,window2,window3,window4;
uniform float level0,level1,level2,level3,level4;
uniform float llr0,llr1,llr2,llr3,llr4;	// low level reject
uniform float alpha0,alpha1,alpha2,alpha3,alpha4;	// low level reject
//in vec4 gl_TexCoord[10];
vec3 bounds_lo = vec3(0.0,0.0,0.0);
vec3 bounds_hi = vec3(1.0,1.0,1.0);

bool clampMe(int index)
{
	vec3 coord = gl_TexCoord[index].xyz;
	return any(lessThan(coord, bounds_lo)) || any(greaterThan(coord, bounds_hi));
}

float windowLevel(float x, float window, float level)
{
	return (x-level)/window  + 0.5;
}

/** Map Luminance volume layer N through a window/level/llr + lut
 * 
 */
vec4 applyLutLayerN(in vec4 base,in int index,in int lutsize,in samplerBuffer lut,in sampler3D texture,in float window,in float level,in float llr,in float alpha)
{
	if (clampMe(2*index)) // ignore drawing outside texture
	{
		return base;
	}	
	
	// get volume intensity value (same in all of rgb)
	float idx = texture3D(texture, gl_TexCoord[2*index].xyz).r;
		
	// ignore drawing below llr
	if (idx < llr)
	{
		return base;
	}
	
	// apply window+level transform
	idx = windowLevel(idx, window, level);
	
	// map through lookup table - interpolated
	idx = clamp(idx, 0.0, 1.0);	
	float pos = idx * (lutsize-1); // floating-point lut index
	int p0 = int(floor(pos)); // integer part of lut index
	vec4 c0 = texelFetchBuffer(lut, p0);
	vec4 c1 = texelFetchBuffer(lut, p0+1);
	vec4 col = mix(c0,c1,pos-p0); // interpolate lut.
	
	col.a = alpha;

	col =  mix(base, col, alpha);
	return col;
}

/** Map RGBA layer N through a window/level/llr 
 * 
 */
vec4 applyRGBALayerN(in vec4 base,in int index,in sampler3D texture,in float window,in float level,in float llr,in float alpha)
{
	if (clampMe(2*index)) // ignore drawing outside texture
	{
		return base;
	}	

	// get rgb color from texture
	vec4 col = texture3D(texture, gl_TexCoord[2*index].xyz);
		
	// ignore drawing below llr
	if (all(lessThan(col.rgb, vec3(llr))))
	{
		return base;
	}
	
	// apply window+level transform to each component
	col.r = windowLevel(col.r, window, level);
	col.g = windowLevel(col.g, window, level);
	col.b = windowLevel(col.b, window, level);	
	col.a = alpha;

	col =  mix(base, col, alpha);
	return col;
}

/** Apply colors from layer N. Decide if it is type luminance or rgb.
 * 
 */
vec4 applyLayerN(in vec4 base,in int index,in int lutsize,in samplerBuffer lut,in sampler3D texture,in float window,in float level,in float llr,in float alpha)
{
	if (lutsize>0)
	{
		return applyLutLayerN(base,index,lutsize,lut,texture,window,level,llr,alpha);
	}
	else
	{
		return applyRGBALayerN(base,index,texture,window,level,llr,alpha);
	}
}

void main()
{	
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
	
	col = applyLayerN(col,0,lutsize0,lut0,texture0, window0, level0, llr0, alpha0);
		
	if (layers>1)
	{
		col = applyLayerN(col,1,lutsize1,lut1,texture1, window1, level1, llr1, alpha1);
	} 
	if (layers>2)
	{
		col = applyLayerN(col,2,lutsize2,lut2,texture2, window2, level2, llr2, alpha2);
	} 
	if (layers>3)
	{
		col = applyLayerN(col,3,lutsize3,lut3,texture3, window3, level3, llr3, alpha3);
	}	
	
	gl_FragColor = col;
}
