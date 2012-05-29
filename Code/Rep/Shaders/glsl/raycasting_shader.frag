#version 120
#extension GL_EXT_gpu_shader4 : enable
#pragma debug(on)

const int volumes=${NUMBER_OF_VOLUMES};
const int maxIterations = 450;
uniform float stepsize;
uniform vec2 viewport;

uniform int lutSize[4];
uniform sampler3D volumeTexture[4];
uniform float threshold[4];
uniform int renderMode;
uniform float window[4];
uniform float level[4];
uniform samplerBuffer lut[4];
uniform float transparency[4];
uniform mat4 M[4];
uniform sampler2D depthBuffer;

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
	vec2 result;
	result.x = fragment.x/viewport.x;
	result.y = fragment.y/viewport.y;
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
	float alphaAccumulator = 0.0; // The  dest alpha for blending
	vec4 colorSample; // The src color 
	float alphaSample; // The src alpha
	float n = 0.0;
	float thau = 0.02;
	gl_FragDepth = 1.0;
	bool found_depth = false;

	vec2 depthLookup;
	depthLookup = depthTexCoords(gl_FragCoord, viewport);
	depthLookup.x = fract(depthLookup.x);
	depthLookup.y = fract(depthLookup.y); 
	vec4 depth = texture2D(depthBuffer, depthLookup);
	if (depth.r*1.01 < gl_FragCoord.z)
	{
		discard;
		return;
	}

	rayDirection = computeRayDirection(gl_FragCoord, viewport);
	
	vec4 rayDeltaVector = rayDirection * delta;
	if (renderMode == 5) alphaAccumulator = 1.0;

	bool beenHit[4];
	for(int i = 0; i < maxIterations; i++)
	{
		colorSample = vec4(0);
		int hit = 0;
		int contributingVolumes = 0;
		for (int i = 0; i < volumes; ++i)
		{
			if (inUnitCube(M[i]*vect))
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
			// We didn't hit any volume, but there could be some volumes left to hit, continue iteration
			// TODO: compute intersection with next volume here
			vect += rayDeltaVector;
			continue;
		}

		if (!found_depth)
		{
			vec4 depth = gl_ModelViewProjectionMatrix * vect;
			depth.z = depth.z/depth.w;
			depth.z = (depth.z + 1.0)*0.5;
			gl_FragDepth = depth.z;
			found_depth = true;
		}

		if (renderMode == 0) // Accumulated average (compositing)
		{
			alphaSample = colorSample.a * stepsize;
			colorAccumulator += (1.0 - alphaAccumulator) * colorSample * alphaSample;
			alphaAccumulator += (1.0 - alphaAccumulator) * alphaSample;
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
			alphaSample = colorSample.a * stepsize;
			colorAccumulator   += (1.0 - alphaAccumulator) * colorSample * alphaSample * 3.0;
			alphaAccumulator += alphaSample;
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
				colorAccumulator += alphaAccumulator * alphaSample;
				alphaAccumulator = alphaAccumulator * exp((-1.0 * thau) * alphaSample);
			}
		}
		if (renderMode == 6)
		{
			colorAccumulator = colorSample;
			break;
		}

		vect += rayDeltaVector;

		if (renderMode == 5)
		{
			if (alphaAccumulator < 0.01) break;
		}
		else
		{
			if (alphaAccumulator > 0.95) break; // terminate if opacity > 1 or the ray is outside the volume
		}
	}


	// Averaging
	if (renderMode == 2)
	{
		colorAccumulator = colorAccumulator / n;
	}

	if (renderMode == 0)
	{
		colorAccumulator.a = alphaAccumulator;
	}
	gl_FragColor = colorAccumulator;
}
