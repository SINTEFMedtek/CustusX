uniform sampler3D volume_tex;
uniform float stepsize;
uniform float threshold;
uniform int renderMode;
uniform float window;
uniform float level;

float applyWindowLevel(float input, float window, float level)
{
	return (input-level)/window + 0.5;
}

vec4 applyWindowLevel(vec4 input, float window, float level)
{
	return (input-level)/window + 0.5;
}

void main()
{
    vec4 start = gl_TexCoord[0];
    vec4 norm_dir = gl_TexCoord[3];
    float delta = stepsize;
    vec4 delta_dir = norm_dir * delta;
    vec4 vect = start;
    vec4 col_acc = vec4(0,0,0,1); // The dest color
    float alpha_acc = 0.0; // The  dest alpha for blending
    vec4 color_sample; // The src color 
    float alpha_sample; // The src alpha
    float n = 0.0;
    float thau = 0.02;
    float real_threshold = applyWindowLevel(threshold, window, level);

    if (renderMode == 5) alpha_acc = 1.0;
    
    for(int i = 0; i < 450; i++)
    {
	    color_sample = texture3D(volume_tex, vect.xyz);
	    color_sample = applyWindowLevel(color_sample, window, level);

		if (renderMode == 0) // Accumulated average (compositing)
		{
			alpha_sample = color_sample.a * stepsize;
			col_acc += (1.0 - alpha_acc) * color_sample * alpha_sample;
			alpha_acc += (1.0 - alpha_acc) * alpha_sample;
		}

		if (renderMode == 1) // Maximum intensity
		{
			if (color_sample.x > col_acc.x)
			{
				col_acc = color_sample;
			}
		}

		if (renderMode == 2) // Average (x-ray)
		{
			if (color_sample.x > 0.0)
			{
				n += 1.0;
				col_acc += color_sample;
			}
		}

		if (renderMode == 3) // Frank's doodle
		{
			alpha_sample = color_sample.a * stepsize;
			col_acc   += (1.0 - alpha_acc) * color_sample * alpha_sample * 3.0;
			alpha_acc += alpha_sample;
		}

		if (renderMode == 4) // Accumulated average (compositing) with gradient
		{
			vec3 gradient;
			float vsX, vsY, vsZ = 0.0;

			// X
			vec4 val1 = texture3D(volume_tex, vec3(vect.x - vsX, vect.y, vect.z));
			vec4 val2 = texture3D(volume_tex, vec3(vect.x + vsX, vect.y, vect.z));
			gradient.x = (val2.x - val1.x) * 0.5;

			// Y
			val1 = texture3D(volume_tex, vec3(vect.x, vect.y - vsY, vect.z));
			val2 = texture3D(volume_tex, vec3(vect.x, vect.y + vsY, vect.z));
			gradient.y = (val2.y - val1.y) * 0.5;

			// X
			val1 = texture3D(volume_tex, vec3(vect.x, vect.y, vect.z - vsZ));
			val2 = texture3D(volume_tex, vec3(vect.x, vect.y, vect.z + vsZ));
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
			float voxelValue = (color_sample.r + color_sample.g + color_sample.b) / 3.0;
			if ((voxelValue - threshold) > 0.0)
			{
				alpha_sample = voxelValue - threshold;
				col_acc += alpha_acc * alpha_sample;
				alpha_acc = alpha_acc * exp((-1.0 * thau) * alpha_sample);
			}
		}
		if (renderMode == 6)
		{
			float voxelValue = (color_sample.x + color_sample.y + color_sample.z)/3.0;
			if (voxelValue > real_threshold)
			{
				vec4 color_sample2 = texture3D(volume_tex,(vect+0.5*delta_dir).xyz);
				color_sample2 = applyWindowLevel(color_sample2, window, level);
				vec4 color_sample3 = texture3D(volume_tex,(vect-0.5*delta_dir).xyz);
				color_sample3 = applyWindowLevel(color_sample3, window, level);
				col_acc = mix(color_sample, color_sample2,0.5);
				col_acc = col_acc*0.66 + color_sample3*0.33;
				break;
			}
		}

		vect += delta_dir;

		if (vect.x > 1.0 || vect.y > 1.0 || vect.z > 1.0 || vect.x < 0.0 || vect.y < 0.0 || vect.z < 0.0) break;

		if (renderMode == 5)
			if (alpha_acc < 0.01) break;
		else
			if (alpha_acc > 0.95) break; // terminate if opacity > 1 or the ray is outside the volume
    }


	// Averaging
	if (renderMode == 2)
	{
		col_acc = col_acc / n;
	}

	if (renderMode == 0)
	{
		col_acc.a = alpha_acc;
	}

    gl_FragColor = col_acc;
}
