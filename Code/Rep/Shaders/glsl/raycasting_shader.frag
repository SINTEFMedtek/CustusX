#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle : enable
#pragma debug(on)

const int volumes=${NUMBER_OF_VOLUMES};
const int maxIterations = 450;
const int maxVolumes = 4;
uniform float stepsize;
uniform vec2 viewport;

uniform int lutSize[maxVolumes];
uniform sampler3D volumeTexture[maxVolumes];
uniform float threshold[maxVolumes];
uniform int renderMode;
uniform float window[maxVolumes];
uniform float level[maxVolumes];
uniform samplerBuffer lut[maxVolumes];
uniform float transparency[maxVolumes];
uniform mat4 M[maxVolumes];
uniform bool useCutPlane[maxVolumes];
uniform sampler2DRect depthBuffer;
uniform sampler2DRect backgroundBuffer;
uniform vec3 cutPlaneNormal;
uniform vec3 cutPlaneOffset;

float applyWindowLevel(float input, float window, float level)
{
	return (input - level) / window + 0.5;
}

vec4 applyWindowLevel(vec4 input, float window, float level)
{
	return (input - level) / window + 0.5;
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

vec4 viewVolumePosition( vec4 fragment, vec2 viewport)
{
	vec4 result;
	result.x = 2.0*fragment.x/viewport.x - 1.0;
	result.y = 2.0*fragment.y/viewport.y - 1.0;
	result.z = 2.0*fragment.z - 1.0;
	result.w = 1.0;
	return result;
}

vec2 depthTexCoords( vec4 fragment, vec2 viewport)
{

	vec2 result = fragment.xy;
	return result;
}

vec4 unproject( vec4 viewportPosition, vec2 viewport )
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
	float min = maxIterations*stepsize;
	if (inUnitCube(pos))
	{
		return 0.0;
	}
	if (pos.x < 0.0 && delta.x > 0.0)
	{
		float distance = -pos.x/delta.x;
		if (distance < min && inUnitCube(pos + distance*delta))
		{
			min = distance;
		}
	}
	if (pos.x > 1.0 && delta.x < 0.0)
	{
		float distance = (pos.x-1.0)/-delta.x;
		if (distance < min && inUnitCube(pos + distance*delta))
		{
			min = distance;
		}
	}
	if (pos.y < 0.0 && delta.y > 0.0)
	{
		float distance = -pos.y/delta.y;
		if (distance < min && inUnitCube(pos + distance*delta))
		{
			min = distance;
		}
	}
	if (pos.y > 1.0 && delta.y < 0.0)
	{
		float distance = (pos.y-1.0)/-delta.y;
		if (distance < min && inUnitCube(pos + distance*delta))
		{
			min = distance;
		}
	}
	if (pos.z < 0.0 && delta.z > 0.0)
	{
		float distance = -pos.z/delta.z;
		if (distance < min && inUnitCube(pos + distance*delta))
		{
			min = distance;
		}
	}
	if (pos.z > 1.0 && delta.z < 0.0)
	{
		float distance = (pos.z-1.0)/-delta.z;
		if (distance < min && inUnitCube(pos + distance*delta))
		{
			min = distance;
		}
	}
	if (min == maxIterations * stepsize)
	{
		return -1.0;
	}
	return min;
}

vec4 computeRayDirection( vec4 position, vec2 viewport)
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

void main()
{
	vec4 start = gl_TexCoord[1];
	vec4 rayDirection;
	float delta = stepsize;
	vec4 vect = start;
	vec4 colorAccumulator = vec4(0, 0, 0, 0); // The dest color
	vec4 colorSample; // The src color 
	float alphaSample; // The src alpha
	float n = 0.0;
	float thau = 0.02;
	bool found_depth = false;

	vec2 depthLookup;
	depthLookup = depthTexCoords(gl_FragCoord, viewport);
	vec4 depth = texture2DRect(depthBuffer, depthLookup);
	vec4 end = gl_FragCoord;
	end.z = depth.r;
	end = unproject(end, viewport);
	float maxLength = length(end-start);

	rayDirection = computeRayDirection(gl_FragCoord, viewport);
	
	vec4 rayDeltaVector = rayDirection * delta;
	if (renderMode == 5) colorAccumulator.a = 1.0;

	bool beenHit[volumes];
	bool doBreak = false;
	for (int j = 0; j < volumes; ++j)
	{
		beenHit[j] = false;
	}
	for(int i = 0; i < maxIterations; i++)
	{
		colorSample = vec4(0);
		if (i > maxLength/delta)
		{
			colorSample = texture2DRect(backgroundBuffer, depthLookup);
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
					volumeColorSample = applyWindowLevel(volumeColorSample, window[i], level[i]);
					if ( lutSize[i] > 0)
					{
						volumeColorSample = applyLut( volumeColorSample.r, lut[i], lutSize[i]);
					}
					volumeColorSample.a = transparency[i];
					colorSample = blendRGBA(colorSample, volumeColorSample);
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
			break;
		} else if (hit > 0 && contributingVolumes == 0)
		{
			// We're inside at least one volume, but none of the volumes exceeded the threshold
			vect += rayDeltaVector;
			continue;			
		} else if (hit==0)
		{
			// We didn't hit any volume, but there could be some volumes left to hit. Skip distance to next volume, or break if no more volumes will be hit.
			float min = maxIterations*stepsize;
			for (int j = 0; j < volumes; ++j)
			{
				if (!beenHit[j])
				{
					float hit = toHit(M[j], vect, rayDirection);
					if (hit < 0.0)
					{
						beenHit[j] = true; // haven't been hit, but will not hit
					}
					else if (hit == 0.0)
					{
						min = delta;
					}
					else if (hit < min)
					{
						min = hit;
					}
				}
			}
			if (min == maxIterations * stepsize)
			{
				break;
			}
			vect += min * rayDirection;
			continue;
		}
		}

		if (renderMode == 0) // Accumulated average (compositing)
		{
			colorAccumulator = blendRGBA(colorAccumulator, colorSample);
		}

		if (renderMode == 1) // Maximum intensity
		{
			if (colorSample.x > colorAccumulator.x)
			{
				colorAccumulator = colorSample;
			}
		}

		if (renderMode == 2) // Average (x-ray)
		{
			if (colorSample.x > 0.0)
			{
				n += 1.0;
				colorAccumulator += colorSample;
			}
		}

		if (renderMode == 3) // Frank's doodle
		{
			alphaSample = colorSample.a;
			colorAccumulator.rgb   += (1.0 - colorAccumulator.a) * colorSample.rgb * alphaSample * 3.0;
			colorAccumulator.a += alphaSample;
		}

		if (renderMode == 4) // Accumulated average (compositing) with gradient
		{
			vec3 gradient;
			float vsX, vsY, vsZ = 0.0;

			// X
			vec4 val1 = texture3D(volumeTexture[0], vec3(vect.x - vsX, vect.y, vect.z));
			vec4 val2 = texture3D(volumeTexture[0], vec3(vect.x + vsX, vect.y, vect.z));
			gradient.x = (val2.x - val1.x) * 0.5;

			// Y
			val1 = texture3D(volumeTexture[0], vec3(vect.x, vect.y - vsY, vect.z));
			val2 = texture3D(volumeTexture[0], vec3(vect.x, vect.y + vsY, vect.z));
			gradient.y = (val2.y - val1.y) * 0.5;

			// X
			val1 = texture3D(volumeTexture[0], vec3(vect.x, vect.y, vect.z - vsZ));
			val2 = texture3D(volumeTexture[0], vec3(vect.x, vect.y, vect.z + vsZ));
			gradient.z = (val2.z - val1.z) * 0.5;

			vec3 n_gradient = normalize(gradient);

			// Apply Phong shading parameters
			// --- Not done implementing ---
			vec3 viewVector;
			vec3 lightVector;
/*			
			mat4 invRotMat; // Get this as uniform val!!!

			vec4 transLightVector = invRotMat * vec4(lightVector.xyz, 0.0);
			lightVector = transLightVector.xyz;
			vec4 transViewVector = invRotMat * vec4(viewVector.xyz, 0.0);
			viewVector = transViewVector.xyz;

			lightVector = normalize(lightVector);
			viewVector = normalize(viewVector);
*/
		}

		if (renderMode == 5) // --- Torgrim's algorithm --- (Cloud light scattering)
		{
			float voxelValue = (colorSample.r + colorSample.g + colorSample.b) / 3.0;
			if ((voxelValue - threshold[0]) > 0.0)
			{
				alphaSample = voxelValue - threshold[0];
				colorAccumulator.rgb += colorAccumulator.a * alphaSample;
				colorAccumulator.a = colorAccumulator.a * exp((-1.0 * thau) * alphaSample);
			}
		}
		if (renderMode == 6)
		{
			colorAccumulator = colorSample;
			colorAccumulator.a = 1.0;
			break;
		}

		if (doBreak)
		{
			break;
		}
		vect += rayDeltaVector;

		if (renderMode == 5)
		{
			if (colorAccumulator.a < 0.01) break;
		}
		else if (renderMode != 1 && renderMode != 2)
		{
			if (colorAccumulator.a > 0.95) break; // terminate if opacity > 1 or the ray is outside the volume
		}
	}


	// Averaging
	if (renderMode == 2)
	{
		colorAccumulator = colorAccumulator / n;
	}

	gl_FragColor = colorAccumulator;
	gl_FragColor.a = 1.0;
}
