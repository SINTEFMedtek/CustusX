uniform sampler3D volumeTexture;
uniform float stepsize;
uniform float threshold;
uniform int renderMode;
uniform float window;
uniform float level;

float applyWindowLevel(float input, float window, float level)
{
	return (input - level) / window + 0.5;
}

vec4 applyWindowLevel(vec4 input, float window, float level)
{
       return (input - level) / window + 0.5;
}

void main()
{
    vec4 start = gl_TexCoord[0];
    vec4 rayDirection = gl_TexCoord[3];
    float delta = stepsize;
    vec4 rayDeltaVector = rayDirection * delta;
    vec4 vect = start;
    vec4 colorAccumulator = vec4(0, 0, 0, 1); // The dest color
    float alphaAccumulator = 0.0; // The  dest alpha for blending
    vec4 colorSample; // The src color 
    float alphaSample; // The src alpha
    float n = 0.0;
    float thau = 0.02;
    float real_threshold = applyWindowLevel(threshold, window, level);

    if (renderMode == 5) alphaAccumulator = 1.0;
    
    for(int i = 0; i < 450; i++)
    {
	    colorSample = texture3D(volumeTexture, vect.xyz);
	    colorSample = applyWindowLevel(colorSample, window, level);

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
			float voxelValue = (colorSample.x + colorSample.y + colorSample.z)/3.0;
			if (voxelValue > real_threshold)
			{
				vec4 colorSample2 = texture3D(volumeTexture,(vect+0.5*rayDeltaVector).xyz);
				colorSample2 = applyWindowLevel(colorSample2, window, level);
				vec4 colorSample3 = texture3D(volumeTexture,(vect-0.5*rayDeltaVector).xyz);
				colorSample3 = applyWindowLevel(colorSample3, window, level);
				colorAccumulator = mix(colorSample, colorSample2, 0.5);
				colorAccumulator = colorAccumulator * 0.66 + colorSample3 * 0.33;
				break;
			}
		}

		vect += rayDeltaVector;

		if (any(greaterThan(vect.xyz, vec3(1, 1, 1))) || any(lessThan(vect.xyz, vec3(0, 0, 0)))) break;

		if (renderMode == 5)
			if (alphaAccumulator < 0.01) break;
		else
			if (alphaAccumulator > 0.95) break; // terminate if opacity > 1 or the ray is outside the volume
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
