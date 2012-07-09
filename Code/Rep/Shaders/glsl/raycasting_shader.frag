#version 120
#extension GL_EXT_gpu_shader4 : enable
#pragma debug(on)

const int volumes=${NUMBER_OF_VOLUMES};
const int maxIterations = 450;
const int maxVolumes = ${MAX_VOLUMES};
const float stepsize = ${STEPSIZE};
const float maxDistance = float(maxIterations) * stepsize;
uniform vec4 viewport;

uniform int lutSize[maxVolumes];
uniform sampler3D volumeTexture[maxVolumes];
uniform float threshold[maxVolumes];
uniform float window[maxVolumes];
uniform float level[maxVolumes];
uniform samplerBuffer lut[maxVolumes];
uniform float alpha[maxVolumes];
uniform mat4 M[maxVolumes];
uniform bool useCutPlane[maxVolumes];
uniform float maxValue[maxVolumes];
uniform sampler2D depthBuffer;
uniform sampler2D backgroundBuffer;
uniform vec3 cutPlaneNormal;
uniform vec3 cutPlaneOffset;

vec4 applyWindowLevel(vec4 value, float window, float level)
{
	return (value - level) / window + 0.5;
}

vec4 applyLut(in float value, in samplerBuffer lut, in int lutSize2)
{
	// map through lookup table - interpolated
	value = clamp(value, 0.0, 1.0);	
	float pos = value * (float(lutSize2)-1.0); // floating-point lut index
	int p0 = int(floor(pos)); // integer part of lut index
	vec4 c0 = texelFetchBuffer(lut, p0);
	vec4 c1 = texelFetchBuffer(lut, p0+1);
	vec4 col = mix(c0,c1,pos-float(p0)); // interpolate lut.
	return col;
}

vec4 blendRGBA(in vec4 a, in vec4 b)
{
	vec4 ret;
	ret.a = a.a + b.a*(1.0-a.a);
	ret.rgb = (a.rgb * a.a + b.rgb * b.a * (1.0 - a.a)) / ret.a;
	return ret;
}

vec4 viewVolumePosition( vec4 fragment, vec4 viewport)
{
	vec4 result;
	result.x = 2.0*(fragment.x-viewport.x)/viewport.z - 1.0;
	result.y = 2.0*(fragment.y-viewport.y)/viewport.w - 1.0;
	result.z = 2.0*fragment.z - 1.0;
	result.w = 1.0;
	return result;
}

vec2 depthTexCoords( vec4 fragment, vec4 viewport)
{
	vec2 result = (fragment.xy - viewport.xy) / viewport.zw;
	return result;
}

vec4 unproject( vec4 viewportPosition, vec4 viewport )
{
	vec4 viewVolumePos = viewVolumePosition(viewportPosition, viewport);
	vec4 worldPosition = gl_ModelViewProjectionMatrixInverse * viewVolumePos;
	worldPosition /= worldPosition.w;
	return worldPosition;
}

bool inUnitCube( vec4 pos)
{
	return !(any(greaterThan(pos.xyz, vec3(1))) || any(lessThan(pos.xyz, vec3(0))));
}

float toHit( mat4 Matrix, vec4 position, vec4 rayDirection)
{
	vec4 pos = Matrix * position;
	vec4 delta = Matrix * rayDirection;
	float minimum = maxDistance;
	if (inUnitCube(pos))
	{
		return stepsize;
	}
	if (pos.x < 0.0 && delta.x > 0.0)
	{
		float distance = -pos.x/delta.x;
		distance += 0.001;
		if (distance < minimum && inUnitCube(pos + distance*delta))
		{
			minimum = distance;
		}
	}
	if (pos.x > 1.0 && delta.x < 0.0)
	{
		float distance = (pos.x-1.0)/-delta.x;
		distance += 0.001;
		if (distance < minimum && inUnitCube(pos + distance*delta))
		{
			minimum = distance;
		}
	}
	if (pos.y < 0.0 && delta.y > 0.0)
	{
		float distance = -pos.y/delta.y;
		distance += 0.001;
		if (distance < minimum && inUnitCube(pos + distance*delta))
		{
			minimum = distance;
		}
	}
	if (pos.y > 1.0 && delta.y < 0.0)
	{
		float distance = (pos.y-1.0)/-delta.y;
		distance += 0.001;
		if (distance < minimum && inUnitCube(pos + distance*delta))
		{
			minimum = distance;
		}
	}
	if (pos.z < 0.0 && delta.z > 0.0)
	{
		float distance = -pos.z/delta.z;
		distance += 0.001;
		if (distance < minimum && inUnitCube(pos + distance*delta))
		{
			minimum = distance;
		}
	}
	if (pos.z > 1.0 && delta.z < 0.0)
	{
		float distance = (pos.z-1.0)/-delta.z;
		distance += 0.001;
		if (distance < minimum && inUnitCube(pos + distance*delta))
		{
			minimum = distance;
		}
	}
	if (minimum == maxDistance)
	{
		return -1.0;
	}
	return min(minimum, stepsize);
}

vec4 computeRayDirection( vec4 position, vec4 viewport)
{
	vec4 near, far;
	near = position;
	near.z = 0.0;
	near = unproject(near, viewport);
	far = position;
	far.z = 1.0;
	far = unproject(far, viewport);

	vec4 rayDirection = far-near;
	rayDirection = normalize(rayDirection);
	return rayDirection;
}

float opacityTransfer( float intensity, float threshold, float alpha, float maxVal)
{
	return alpha * (intensity - threshold) / (maxVal - threshold);
}

void main()
{
	vec4 vect = gl_TexCoord[1];
	vec4 colorAccumulator = vec4(0, 0, 0, 0); // The dest color
	vec4 colorSample; // The src color
	float n = 0.0;
	const float thau = 0.02;
	vec2 depthLookup = depthTexCoords(gl_FragCoord, viewport);
	float maxLength = length(unproject(vec4(gl_FragCoord.xy, texture2D(depthBuffer, depthLookup).r, gl_FragCoord.w), viewport) - gl_TexCoord[1]);
	vec4 rayDirection = computeRayDirection(gl_FragCoord, viewport);
	vec4 rayDeltaVector = rayDirection * stepsize;
	bool beenHit[volumes];
	bool doBreak = false;

	for (int j = 0; j < volumes; ++j)
	{
		beenHit[j] = false;
	}
	for(int i = 0; i < maxIterations; i++)
	{
		colorSample = vec4(0);
		if (i > maxLength / stepsize)
		{
			colorSample = texture2D(backgroundBuffer, depthLookup);
			colorSample.a = 1.0;
			doBreak = true;
		}
		else
		{
			int hit = 0;
			int contributingVolumes = 0;
			for (int i = 0; i < volumes; ++i)
			{
				if (inUnitCube(M[i]*vect) && (!useCutPlane[i] || dot(cutPlaneNormal, cutPlaneOffset-vect.xyz) < 0.0))
				{
					beenHit[i] = true;
					++hit;
					vec4 volumeColorSample = texture3D(volumeTexture[i], (M[i]*vect).xyz);
					if (!all(lessThan(volumeColorSample.rgb, vec3(threshold[i]))))
					{
						++contributingVolumes;
						float intensity;
						vec4 windowed = applyWindowLevel(volumeColorSample, window[i], level[i]);;
						if (lutSize[i] > 0)
						{
							intensity = volumeColorSample.r;
							windowed = applyLut(windowed.r, lut[i], lutSize[i]);
						}
						else
						{
							intensity = 0.33 * (volumeColorSample.r + volumeColorSample.g + volumeColorSample.b);
							windowed = applyWindowLevel(windowed, window[i], level[i]);
						}
						windowed.a = stepsize * opacityTransfer(intensity, threshold[i], alpha[i], maxValue[i]);
						colorSample = blendRGBA(colorSample, windowed);
					}
				}
			}

			bool allVolumesBeenHit = true;
			for (int i = 0; i < volumes; ++i)
			{
				allVolumesBeenHit = allVolumesBeenHit && beenHit[i];
			}
			if (allVolumesBeenHit && hit == 0 && i > 0)
			{
				// We left the last volume
				colorSample = texture2D(backgroundBuffer, depthLookup);
				colorSample.a = 1.0;
				doBreak = true;
			} else if (hit > 0 && contributingVolumes == 0)
			{
				// We're inside at least one volume, but none of the volumes exceeded the threshold
				vect += rayDeltaVector;
				continue;			
			} else if (hit==0)
			{
				// We didn't hit any volume, but there could be some volumes left to hit. Skip distance to next volume, or break if no more volumes will be hit.
				float minimum = maxDistance;
				for (int j = 0; j < volumes; ++j)
				{
					if (!beenHit[j])
					{
						float hit = toHit(M[j], vect, rayDirection);
						beenHit[j] = (hit < 0.0); // haven't been hit, but will not hit
						minimum = min(hit, minimum);
					}
				}
				if (minimum == maxDistance)
				{
					colorSample = texture2D(backgroundBuffer, depthLookup);
					colorSample.a = 1.0;
					doBreak = true;
				}
				vect += minimum * rayDirection;
				continue;
			}
		}

		colorAccumulator = blendRGBA(colorAccumulator, colorSample);

		vect += rayDeltaVector;

		if (colorAccumulator.a > 0.95 || doBreak) break; // terminate if opacity > 1 or the ray is outside the volume
	}

	gl_FragColor = colorAccumulator;
	gl_FragColor.a = 1.0;
}
