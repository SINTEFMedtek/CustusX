/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGPUVolumeRayCastMapper_OneComponentFS.glsl

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Fragment shader that implements scalarFromValue() and colorFromValue() in
// the case of a one-component dataset.
// The functions are used in composite mode.

#version 110

// "value" is a sample of the dataset.
// Think of "value" as an object.

uniform sampler1D colorTexture[10];//Mehdi

float scalarFromValue(vec4 value)
{
  return value.x;
}

vec4 colorFromValue(int volNumber,vec4 value) //Mehdi
{
  return texture1D(colorTexture[volNumber],value.x);//Mehdi
}
