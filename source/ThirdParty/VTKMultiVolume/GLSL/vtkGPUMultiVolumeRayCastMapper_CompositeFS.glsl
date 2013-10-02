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

/*

float findXonPlane(vec3 n, vec3 p0, vec3 point)
{
	return   ((n.x*p0.x)-(n.y*(point.y-p0.y))-( n.z*(point.z-p0.z)))/n.x;
}

float findYonPlane(vec3 n, vec3 p0, vec3 point)
{
	return   ((n.y*p0.y)-(n.x*(point.x-p0.x))-( n.z*(point.z-p0.z)))/n.y;
}

float findZonPlane(vec3 n, vec3 p0, vec3 point)
{
	return   ((n.z*p0.z)-(n.x*(point.x-p0.x))-( n.y*(point.y-p0.y)))/n.z;
}

*/

float xMin[10];
float yMin[10];
float zMin[10];

uniform	vec3 ClippingplanesNormal[10];
uniform	vec3 ClippingplanesOrigins[10];
uniform int clipped[10];

void clip (int i, vec3 pos)
{
	
	xMin[i]=-10.0;
	yMin[i]=-10.0;
	zMin[i]=-10.0;
	/*
	ClippingplanesNormal[0]=vec3(1.0,1.0,-1.0);
	ClippingplanesOrigins[0]=vec3(.5,.5,.5);
	ClippingplanesNormal[1]=vec3(1.0,1.0,0.0);
	ClippingplanesOrigins[1]=vec3(.5,.5,.5);
	*/

	float partX=ClippingplanesNormal[i].x*(pos.x-ClippingplanesOrigins[i].x);
	float partY=ClippingplanesNormal[i].y*(pos.y-ClippingplanesOrigins[i].y);
	float partZ=ClippingplanesNormal[i].z*(pos.z-ClippingplanesOrigins[i].z);

	float temp;
		
	if(ClippingplanesNormal[i].x!=0.0)
	{
		temp=((ClippingplanesNormal[i].x*ClippingplanesOrigins[i].x)-partY-partZ)/ClippingplanesNormal[i].x;
		//temp=findXonPlane(ClippingplanesNormal[i], ClippingplanesOrigins[i], pos);
		if(temp>pos.x)
		{
			if(temp>xMin[i])
				xMin[i]=temp;

		}
		
		else
		{
			if(temp<xMin[i])
				xMin[i]=temp;

		}
		if (ClippingplanesNormal[i].x<0.0) xMin[i]=-xMin[i];

	}
	if(ClippingplanesNormal[i].y!=0.0)
	{
		temp=((ClippingplanesNormal[i].y*ClippingplanesOrigins[i].y)-partX-partZ)/ClippingplanesNormal[i].y;
		//temp=findYonPlane(ClippingplanesNormal[i], ClippingplanesOrigins[i], pos);
		if(temp>pos.y)
		{
			if(temp>yMin[i])
				yMin[i]=temp;

		}
		
		else
		{
			if(temp<=yMin[i])
				yMin[i]=temp;

		}
		if (ClippingplanesNormal[i].y<0.0) yMin[i]=-yMin[i];
	}
	
	if(ClippingplanesNormal[i].z!=0.0)
	{
		temp=((ClippingplanesNormal[i].z*ClippingplanesOrigins[i].z)-partX-partY)/ClippingplanesNormal[i].z;
		//temp=findZonPlane(ClippingplanesNormal[i], ClippingplanesOrigins[i], pos);
		if(temp>pos.z)
		{
			if(temp>zMin[i])
				zMin[i]=temp;

		}
		
		else
		{
			if(temp<=zMin[i])
				zMin[i]=temp;

		}
		if (ClippingplanesNormal[i].z<0.0) zMin[i]=-zMin[i];
	}
		
		
}

/*
vec3 x;
int intersectWithLine(vec3 p1, vec3 p2, vec3 n, vec3 p0)
{
  float t;
  float num, den;
  float fabsden, fabstolerance;

  // Compute line vector
  // 
  vec3 p21;
  p21.x = p2.x - p1.x;
  p21.y = p2.y - p1.y;
  p21.z = p2.z - p1.z;

  // Compute denominator.  If ~0, line and plane are parallel.
  // 
  num = dot(n,p0) - ( n.x*p1.x + n.y*p1.y + n.z*p1.z ) ;
  den = n.x*p21.x + n.y*p21.y + n.z*p21.z;
  //
  // If denominator with respect to numerator is "zero", then the line and
  // plane are considered parallel. 
  //

  // trying to avoid an expensive call to fabs()
  if (den < 0.0)
    {
    fabsden = -den;
    }
  else
    {
    fabsden = den;
    }
  if (num < 0.0)
    {
    fabstolerance = -num*0.001;
    }
  else
    {
    fabstolerance = num*0.001;
    }
  if ( fabsden <= fabstolerance )
    {
    t = 999.0;
    return 0;
    }

  // valid intersection
  t = num / den;

  x.x = p1.x + t*p21.x;
  x.y = p1.y + t*p21.y;
  x.z = p1.z + t*p21.z;

  if ( t >= 0.0 && t <= 1.0 )
    {
    return 1;
    }
  else
    {
    return 0;
    }
}

vec3 end;
vec3 start;
void findRaySegment(void)
{
	end=pos;
	start=pos;
	for (float tt=0.0;tt<=tMax;tt+=1.0)
		end=end+rayDir;
	//vec3 x=vec3(0.0,0.0,0.0);
	if (intersectWithLine(start, end, vec3(0.0,0.0,-1.0), vec3(0.5,0.5,0.3))==1)
		end=x;
	else 
		discard;


}
bool isPointOnTheLineZ(vec3 p1, vec3 p2, vec3 p)
{
	
	if (p2.z>p1.z)
	{
		if ( (p.z>=p1.z)&&(p2.z>=p.z) )
		{
			
			return true;
		}
		else
		{
			//gl_FragColor = vec4(1.0,0.0,0.0,0.0);
			return false;
		}
	}
	
	if (p2.z<p1.z)
	{
		if ( (p.z>=p2.z)&&(p1.z>=p.z) )
		{			
			
			return false;
		}
		else
		{
			//gl_FragColor = vec4(0.0,1.0,0.0,0.0);
			return true;
		}
	}

	if (p1.z==p2.z)
	{
		//gl_FragColor = vec4(0.0,0.0,1.0,0.0);
		return false;
	}
	
}
*/


void trace(void)
{
  
  bool inside=true;
  

  vec4 destColor=initialColor();
  float remainOpacity=1.0-destColor.a;
  
  vec4 color[10];
  vec4 opacity[10];
  

  
  initShade();

  float t=0.0;
  

  //findRaySegment();


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
    //Texture2 and upper
		
		for(int xx = 1;((xx<5)&&(xx < Number_Of_Volumes));xx++)//? //Help 		
		{

			if (all(greaterThanEqual(posX[xx-1],lowBounds[xx]))
			&& all(lessThanEqual(posX[xx-1],highBounds[xx])))
			{
				
				if(clipped[xx]==1)
					clip(xx,posX[xx-1]);
				
				if(((xMin[xx]<posX[xx-1].x)&&(yMin[xx]<posX[xx-1].y)&&(zMin[xx]<posX[xx-1].z))||(clipped[xx]==0))
				{
					value[xx]=texture3D(dataSetTexture[xx],posX[xx-1]);
					scalar[xx]=scalarFromValue(value[xx]);
        
					// opacity is the sampled texture value in the 1D opacity texture at
					// scalarValue
					opacity[xx]= texture1D(opacityTexture[xx],scalar[xx]);

					if(opacity[xx].a>0.0)
					{
						color[xx]=shade(xx,value[xx]);
						color[xx]=color[xx]*opacity[xx].a;
						destColor=destColor+color[xx]*remainOpacity;
						remainOpacity=remainOpacity*(1.0-opacity[xx].a);
					}
				}
			}
				
		}

      pos=pos+rayDir;
      t+=1.0;
      inside=t<tMax && (remainOpacity>=0.0039); // 1/255=0.0039
      }
  }
  gl_FragColor = destColor;
  gl_FragColor.a = 1.0-remainOpacity;
}
