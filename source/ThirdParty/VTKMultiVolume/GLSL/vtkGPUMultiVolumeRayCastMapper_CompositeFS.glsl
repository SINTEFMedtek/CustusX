/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGPUVolumeRayCastMapper_CompositeFS.glsl

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Fragment program part with ray cast and composite method.

#version 110


uniform sampler3D dataSetTexture[10]; //Mehdi
uniform sampler1D opacityTexture[10]; //Mehdi


uniform mat4 P1toPN[9];


uniform vec3 lowBounds[10]; //Mehdi 
uniform vec3 highBounds[10]; //Mehdi 


uniform int Number_Of_Volumes;

// Entry position (global scope)
vec3 pos;
vec3 posX[9];



// Incremental vector in texture space (global scope)
vec3 rayDir;

float tMax;

// from cropping vs no cropping
vec4 initialColor();

// from 1 vs 4 component shader.
float scalarFromValue(vec4 value);
vec4 colorFromValue(int volNumber,vec4 value);


// from noshade vs shade.
void initShade();
vec4 shade(int volNumber, vec4 value);


void trace(void)
{
  

  bool inside=true;
  

  vec4 destColor=initialColor();
  float remainOpacity=1.0-destColor.a;
  
  vec4 color[10];
  vec4 opacity[10];
  

  
  initShade();

  float t=0.0;
  
  
  // We NEED two nested while loops. It is trick to work around hardware
  // limitation about the maximum number of loops.

  while(inside)
  {  
    while(inside)
      {
     	for(int iii=0;iii<Number_Of_Volumes-1;iii++) //Mehdi
			posX[iii] = vec3(P1toPN[iii]*vec4(pos,1)); //Mehdi
		
		vec4 value[10];
		float scalar[10];
		
		//Texture 1
		value[0]=texture3D(dataSetTexture[0],pos);
		scalar[0]=scalarFromValue(value[0]);
		// opacity is the sampled texture value in the 1D opacity texture at scalarValue
 
		opacity[0]=texture1D(opacityTexture[0],scalar[0]);//Mehdi

        if(opacity[0].a>0.0)
        {
			color[0]=shade(0,value[0]);
			color[0]=color[0]*opacity[0].a;
			destColor=destColor+color[0]*remainOpacity;
			remainOpacity=remainOpacity*(1.0-opacity[0].a);
        }
		
    //Texture2 and upper

			
		for(int xx=1;(xx<7) && (xx < Number_Of_Volumes);xx++)//? //Help 		
		{
				if (all(greaterThanEqual(posX[xx-1],lowBounds[xx]))
				&& all(lessThanEqual(posX[xx-1],highBounds[xx])))
			{
				value[xx]=texture3D(dataSetTexture[xx],posX[xx-1]);
				scalar[xx]=scalarFromValue(value[xx]);
        
				// opacity is the sampled texture value in the 1D opacity texture at
				// scalarValue
				opacity[xx]=texture1D(opacityTexture[xx],scalar[xx]);

				if(opacity[xx].a>0.0)
				{
					color[xx]=shade(xx,value[xx]);
					color[xx]=color[xx]*opacity[xx].a;
					destColor=destColor+color[xx]*remainOpacity;
					remainOpacity=remainOpacity*(1.0-opacity[xx].a);
				}
			}
				
		}

      pos=pos+rayDir;
      t+=1.0;
      inside=t<tMax && all(greaterThanEqual(pos,lowBounds[0]))
        && all(lessThanEqual(pos,highBounds[0]))
        && (remainOpacity>=0.0039); // 1/255=0.0039
      }
  }
  gl_FragColor = destColor;
  gl_FragColor.a = 1.0-remainOpacity;
}
