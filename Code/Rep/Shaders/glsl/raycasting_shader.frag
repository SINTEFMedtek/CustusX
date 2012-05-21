#version 120
#extension GL_EXT_gpu_shader4 : enable
#pragma debug(on)
uniform int lutSize;
uniform sampler3D volumeTexture;
uniform float stepsize;
uniform float threshold;
uniform int renderMode;
uniform float window;
uniform float level;
uniform samplerBuffer lut;
uniform float transparency;
uniform vec2 viewport;
uniform mat4 M;

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

void main()
{
	vec4 start = gl_TexCoord[1];
	vec4 rayDirection;
	float delta = stepsize;
	vec4 vect = start;
	vec4 colorAccumulator = vec4(0, 0, 0, 1); // The dest color
	float alphaAccumulator = 0.0; // The  dest alpha for blending
	vec4 colorSample; // The src color 
	float alphaSample; // The src alpha
	float n = 0.0;
	float thau = 0.02;
	gl_FragDepth = 1.0;
	bool found_depth = false;

	vec4 near, far;
	near.x = 2.0*gl_FragCoord.x/viewport.x-1.0;
	near.y = 2.0*gl_FragCoord.y/viewport.y-1.0;
	near.z = -1.0;
	near.w = 1.0;
	far.xyw = near.xyw;
	far.z = 1.0;
	far.w = 1.0;
	near =  gl_ModelViewProjectionMatrixInverse * near;
	far = gl_ModelViewProjectionMatrixInverse * far;
	near = near/near.w;
	far = far/far.w;
			
	rayDirection = far-near;
	rayDirection = normalize(rayDirection);

	vec4 rayDeltaVector = rayDirection * delta;
	if (renderMode == 5) alphaAccumulator = 1.0;

	for(int i = 0; i < 450; i++)
	{
		colorSample = texture3D(volumeTexture, (M*vect).xyz);
		if (renderMode == 7)
		{
			colorAccumulator = 0.5*(rayDirection + vec4(1,1,1,0));
			colorAccumulator.a = 1.0;
			gl_FragDepth = gl_FragCoord.z;
			break;
		}
	    
		if (renderMode == 8)
		{
			colorAccumulator = M*vect;
			colorAccumulator.a = 1.0;
			gl_FragDepth = gl_FragCoord.z;
			break;
		}
		if (all(lessThan(colorSample.rgb, vec3(threshold))))
		{
			vect = vect + rayDeltaVector;
			if (any(greaterThan((M*vect).xyz, vec3(1, 1, 1))) || any(lessThan((M*vect).xyz, vec3(0, 0, 0))))
			{
				break;
			}

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
		colorSample = applyWindowLevel(colorSample, window, level);
		if ( lutSize > 0)
		{
			colorSample = applyLut( colorSample.r, lut,lutSize);
		}

		if (renderMode == 0) // Accumulated average (compositing)
		{
//			alphaSample = colorSample.a * stepsize;
			alphaSample = transparency;
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
			vec4 val1 = texture3D(volumeTexture, vec3(vect.x - vsX, vect.y, vect.z));
			vec4 val2 = texture3D(volumeTexture, vec3(vect.x + vsX, vect.y, vect.z));
			gradient.x = (val2.x - val1.x) * 0.5;

			// Y
			val1 = texture3D(volumeTexture, vec3(vect.x, vect.y - vsY, vect.z));
			val2 = texture3D(volumeTexture, vec3(vect.x, vect.y + vsY, vect.z));
			gradient.y = (val2.y - val1.y) * 0.5;

			// X
			val1 = texture3D(volumeTexture, vec3(vect.x, vect.y, vect.z - vsZ));
			val2 = texture3D(volumeTexture, vec3(vect.x, vect.y, vect.z + vsZ));
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
			if ((voxelValue - threshold) > 0.0)
			{
				alphaSample = voxelValue - threshold;
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

		if (any(greaterThan((M*vect).xyz, vec3(1, 1, 1))) || any(lessThan((M*vect).xyz, vec3(0, 0, 0))))
		{
			break;
		}

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
