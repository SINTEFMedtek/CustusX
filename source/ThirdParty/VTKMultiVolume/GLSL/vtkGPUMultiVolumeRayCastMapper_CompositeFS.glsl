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


#ifdef __APPLE__
	#define MAX_NUMBER_OF_VOLUMES 4						
#else
	#define MAX_NUMBER_OF_VOLUMES 5
#endif

uniform sampler3D dataSetTexture[MAX_NUMBER_OF_VOLUMES]; //Mehdi
uniform sampler1D opacityTexture[MAX_NUMBER_OF_VOLUMES]; //Mehdi

uniform mat4 P1toPN[MAX_NUMBER_OF_VOLUMES-1];


uniform vec3 lowBounds[MAX_NUMBER_OF_VOLUMES]; //Mehdi
uniform vec3 highBounds[MAX_NUMBER_OF_VOLUMES]; //Mehdi


uniform int Number_Of_Volumes;

// Entry position (global scope)
vec3 pos;
vec3 posX[MAX_NUMBER_OF_VOLUMES-1];



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



vec3 clipMin[10];

uniform	vec3 ClippingplanesNormal[10];
uniform	vec3 ClippingplanesOrigins[10];
uniform int clipped[10];

void clip (int i, vec3 position)
{
	
	clipMin[i]=vec3(-10.0, -10.0, -10.0);

	float partX=ClippingplanesNormal[i].x*(position.x-ClippingplanesOrigins[i].x);
	float partY=ClippingplanesNormal[i].y*(position.y-ClippingplanesOrigins[i].y);
	float partZ=ClippingplanesNormal[i].z*(position.z-ClippingplanesOrigins[i].z);

	float temp;
		
	if(ClippingplanesNormal[i].x!=0.0)
	{
		temp=((ClippingplanesNormal[i].x*ClippingplanesOrigins[i].x)-partY-partZ)/ClippingplanesNormal[i].x;
		//temp=findXonPlane(ClippingplanesNormal[i], ClippingplanesOrigins[i], pos);
		if(temp>pos.x)
		{
			if(temp>clipMin[i].x)
				clipMin[i].x=temp;

		}		
		else
		{
			if(temp<clipMin[i].x)
				clipMin[i].x=temp;

		}
		if (ClippingplanesNormal[i].x<0.0) clipMin[i].x=-clipMin[i].x;

	}
	if(ClippingplanesNormal[i].y!=0.0)
	{
		temp=((ClippingplanesNormal[i].y*ClippingplanesOrigins[i].y)-partX-partZ)/ClippingplanesNormal[i].y;
		//temp=findYonPlane(ClippingplanesNormal[i], ClippingplanesOrigins[i], pos);
		if(temp>pos.y)
		{
			if(temp>clipMin[i].y)
				clipMin[i].y=temp;

		}
		
		else
		{
			if(temp<=clipMin[i].y)
				clipMin[i].y=temp;

		}
		if (ClippingplanesNormal[i].y<0.0) clipMin[i].y=-clipMin[i].y;
	}
	
	if(ClippingplanesNormal[i].z!=0.0)
	{
		temp=((ClippingplanesNormal[i].z*ClippingplanesOrigins[i].z)-partX-partY)/ClippingplanesNormal[i].z;
		//temp=findZonPlane(ClippingplanesNormal[i], ClippingplanesOrigins[i], pos);
		if(temp>pos.z)
		{
			if(temp>clipMin[i].z)
				clipMin[i].z=temp;

		}
		
		else
		{
			if(temp<=clipMin[i].z)
				clipMin[i].z=temp;

		}
		if (ClippingplanesNormal[i].z<0.0) clipMin[i].z=-clipMin[i].z;
	}
		
		
}



void trace(void)
{
  
  bool inside=true;
  

  vec4 destColor=initialColor();
  float remainOpacity=1.0-destColor.a;
  
  vec4 color[MAX_NUMBER_OF_VOLUMES];
  vec4 opacity[MAX_NUMBER_OF_VOLUMES];
  

  
  initShade();

  float t=0.0;
  

  //findRaySegment();


  // We NEED two nested while loops. It is trick to work around hardware
  // limitation about the maximum number of loops.

    while(inside)
      {

     	for(int iii=0;iii<Number_Of_Volumes-1;iii++) //Mehdi
			posX[iii] = vec3(P1toPN[iii]*vec4(pos,1)); //Mehdi
		
		vec4 value[MAX_NUMBER_OF_VOLUMES];
		float scalar[MAX_NUMBER_OF_VOLUMES];
		
		
			//Texture 1
			/*
			if(all(greaterThanEqual(pos,lowBounds[0]))
			&& all(lessThanEqual(pos,highBounds[0])))
			{
				//if(isPointOnTheLineZ(end,start,pos))
				if(clipped[0]==1)
					clip(0,pos);
		
				if(((xMin[0]<pos.x)&&(yMin[0]<pos.y)&&(zMin[0]<pos.z))||(clipped[0]==0))
				{

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
				}
			}
			*/
				
			//Texture2 and upper
		
			//int xx=1;
		
		//	while(xx<Number_Of_Volumes)	
			for(int xx=1; (xx<MAX_NUMBER_OF_VOLUMES)&&(xx<Number_Of_Volumes); xx++)
			{
		
				if (all(greaterThanEqual(posX[xx-1],lowBounds[xx]))
				&& all(lessThanEqual(posX[xx-1],highBounds[xx])))
				{
				
					if(clipped[xx]==1)
						clip(xx,posX[xx-1]);
				
					//if(((xMin[xx]<posX[xx-1].x)&&(yMin[xx]<posX[xx-1].y)&&(zMin[xx]<posX[xx-1].z))||(clipped[xx]==0))
					if(all(lessThan(clipMin[xx],posX[xx-1]))||(clipped[xx]==0))
					{
				                    
						#ifdef __APPLE__

							if(xx==1)
								value[1]=texture3D(dataSetTexture[1],posX[0]);
							if(xx==2)
								value[2]=texture3D(dataSetTexture[2],posX[1]);
							if(xx==3)
								value[3]=texture3D(dataSetTexture[3],posX[2]);
						#else
							value[xx]=texture3D(dataSetTexture[xx],posX[xx-1]);
						#endif
                                      	
						scalar[xx]=scalarFromValue(value[xx]);
        
						// opacity is the sampled texture value in the 1D opacity texture at
						// scalarValue
					
						#ifdef __APPLE__
						
							if(xx==1)
								opacity[1]= texture1D(opacityTexture[1],scalar[1]);     	
							if(xx==2)
								opacity[2]= texture1D(opacityTexture[2],scalar[2]);
							if(xx==3)
								opacity[3]= texture1D(opacityTexture[3],scalar[3]);
						#else
							opacity[xx]= texture1D(opacityTexture[xx],scalar[xx]);
						#endif
					
	
						if(opacity[xx].a>0.0)
						{
					
							color[xx]=shade(xx,value[xx]);
							color[xx]=color[xx]*opacity[xx].a;
							destColor=destColor+color[xx]*remainOpacity;
							remainOpacity=remainOpacity*(1.0-opacity[xx].a);
						}
					}
				}
				
			//	xx+=1;
			}
			
			pos=pos+rayDir;
			t+=1.0;
			inside=(t<tMax) && (remainOpacity>=0.0039)
			&& (all(greaterThanEqual(pos,lowBounds[0]))
			&& all(lessThanEqual(pos,highBounds[0]))); // 1/255=0.0039
		}
	
	
	gl_FragColor = destColor;
	gl_FragColor.a = 1.0-remainOpacity;

}
