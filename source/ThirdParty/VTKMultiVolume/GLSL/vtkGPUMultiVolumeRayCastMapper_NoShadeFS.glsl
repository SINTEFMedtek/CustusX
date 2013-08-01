/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGPUVolumeRayCastMapper_NoShadeFS.glsl

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Fragment shader that implements initShade() and shade() in the case of no
// shading.
// The functions are used in composite mode.

#version 110

// "value" is a sample of the dataset.
// Think of "value" as an object.

// from 1- vs 4-component shader.
vec4 colorFromValue(int volNumber,vec4 value);
/*
vec4 colorFromValue2(vec4 value);
vec4 colorFromValue3(vec4 value);
vec4 colorFromValue4(vec4 value);
*/
// ----------------------------------------------------------------------------
void initShade()
{
  // empty, nothing to do.
}

// ----------------------------------------------------------------------------
vec4 shade(int volNumber, vec4 value)//Mehdi
{
  return colorFromValue(volNumber,value); //Mehdi
}
