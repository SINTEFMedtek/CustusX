/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGPUMultiVolumeRayCastMapper.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  Modified by:  Carlos Falcón cfalcon@ctim.es 
                Karl Krissian karl@ctim.es 

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkGPUMultiVolumeRayCastMapper.h"

#include "vtkVolumeRenderingFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkTimerLog.h"
#include "vtkImageResample.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include <assert.h>
#include "vtkCommand.h" // for VolumeMapperRender{Start|End|Progress}Event
#include "vtkCamera.h"
#include "vtkRendererCollection.h"
#include "vtkMultiThreader.h"
#include "vtkGPUInfoList.h"
#include "vtkGPUInfo.h"
#include "vtkTransform.h"

// new required classes
#include "vtkVolumeMapper.h"
#include "vtkDataSet.h"
#include "vtkExecutive.h"

vtkInstantiatorNewMacro(vtkGPUMultiVolumeRayCastMapper);
vtkCxxSetObjectMacro(vtkGPUMultiVolumeRayCastMapper, MaskInput, 
                     vtkImageData);
vtkCxxSetObjectMacro(vtkGPUMultiVolumeRayCastMapper, TransformedInput,  
                     vtkImageData);
//Mehdi vtkCxxSetObjectMacro(vtkGPUMultiVolumeRayCastMapper, TransformedInput2, vtkImageData*);
//Mehdi :
#define vtkSetObjectBodyMacro2(name,type,args, volNumber)                           \
  {																					\
  if (this->name[volNumber] != args)										    	\
    {																				\
    type* tempSGMacroVar = this->name[volNumber];                                   \
    this->name[volNumber] = args;                                                   \
    if (this->name[volNumber] != NULL) { this->name[volNumber]->Register(this); }   \
    if (tempSGMacroVar != NULL)														\
      {																				\
      tempSGMacroVar->UnRegister(this);												\
      }																				\
    this->Modified();																\
    }																				\
  }
//Mehdi :
#define vtkCxxSetObjectMacro2(class,name,type)				\
void class::Set##name (type* _arg, int volNumber)			\
  {															\
  vtkSetObjectBodyMacro2(name,type,_arg, volNumber);        \
  }

vtkCxxSetObjectMacro2(vtkGPUMultiVolumeRayCastMapper, AdditionalTransformedInput, vtkImageData);
void vtkGPUMultiVolumeRayCastMapper::setNumberOfAdditionalVolumes(int numAddVolumes)//Mehdi
{
	NUMBER_OF_ADDITIONAL_VOLUMES=numAddVolumes;
	this->SetNumberOfInputPorts(numAddVolumes+1);
}
vtkGPUMultiVolumeRayCastMapper::vtkGPUMultiVolumeRayCastMapper()
{
	NUMBER_OF_ADDITIONAL_VOLUMES=MAX_NUMBER_OF_ADDITIONAL_VOLUMES;
//Mehdi  this->Property2                  = NULL;
  for(int iii=0; iii<MAX_NUMBER_OF_ADDITIONAL_VOLUMES; iii++)//Mehdi
	this->Property[iii]			   = NULL;//Mehdi

  this->AutoAdjustSampleDistances  = 1;
  this->ImageSampleDistance        = 1.0;
  this->MinimumImageSampleDistance = 1.0;
  this->MaximumImageSampleDistance = 10.0;
  this->SampleDistance             = 1.0;
  this->SmallVolumeRender          = 0;
  this->BigTimeToDraw              = 0.0;
  this->SmallTimeToDraw            = 0.0;
  this->FinalColorWindow           = 1.0;
  this->FinalColorLevel            = 0.5;
  this->GeneratingCanonicalView    = 0;
  this->CanonicalViewImageData     = NULL;

  this->MaskInput                  = NULL;
  this->MaskBlendFactor            = 1.0f;
  this->MaskType
    = vtkGPUMultiVolumeRayCastMapper::LabelMapMaskType;


  this->AMRMode=0;
  this->ClippedCroppingRegionPlanes[0]=VTK_DOUBLE_MAX;
  this->ClippedCroppingRegionPlanes[1]=VTK_DOUBLE_MIN;
  this->ClippedCroppingRegionPlanes[2]=VTK_DOUBLE_MAX;
  this->ClippedCroppingRegionPlanes[3]=VTK_DOUBLE_MIN;
  this->ClippedCroppingRegionPlanes[4]=VTK_DOUBLE_MAX;
  this->ClippedCroppingRegionPlanes[5]=VTK_DOUBLE_MIN;

  this->MaxMemoryInBytes=0;
  vtkGPUInfoList *l=vtkGPUInfoList::New();
  l->Probe();
  if(l->GetNumberOfGPUs()>0)
    {
    vtkGPUInfo *info=l->GetGPUInfo(0);
    this->MaxMemoryInBytes=info->GetDedicatedVideoMemory();
    if(this->MaxMemoryInBytes==0)
      {
      this->MaxMemoryInBytes=info->GetDedicatedSystemMemory();
      }
    // we ignore info->GetSharedSystemMemory(); as this is very slow.
    }
  l->Delete();

  if(this->MaxMemoryInBytes==0) // use some default value: 128MB.
    {
    this->MaxMemoryInBytes=128*1024*1024;
    }

  this->MaxMemoryFraction = 0.75;//?

  this->ReportProgress=true;

  this->TransformedInput = NULL;
  this->LastInput = NULL;
  
  //--------------------------------------------------------------------------------Mehdi
  /*Mehdi
  this->SetNumberOfInputPorts(2);
  this->TransformedInput2 = NULL;
  this->LastInput2 = NULL;
  this->SecondInputUserTransform = (vtkTransform*) vtkTransform::New();
  this->SecondInputUserTransform->Identity();
  Mehdi*/

  this->SetNumberOfInputPorts(NUMBER_OF_ADDITIONAL_VOLUMES+1);
  for(int iii=0;iii<NUMBER_OF_ADDITIONAL_VOLUMES;iii++)//Mehdi
  {
	  this->AdditionalTransformedInput[iii] = NULL;//Mehdi
	  this->AdditionalLastInput[iii] = NULL;//Mehdi
	  this->AdditionalInputUserTransform[iii] = (vtkTransform*) vtkTransform::New(); //Mehdi
	  this->AdditionalInputUserTransform[iii]->Identity(); //Mehdi
  }
  //--------------------------------------------------------------------------------Mehdi
 
}

// ----------------------------------------------------------------------------
vtkGPUMultiVolumeRayCastMapper::~vtkGPUMultiVolumeRayCastMapper()
{
  this->SetMaskInput(NULL);
  this->SetTransformedInput(NULL);
  this->LastInput = NULL;
  //Mehdi this->SetTransformedInput2(NULL);
  //Mehdi this->LastInput2 = NULL;
  //Mehdi this->SecondInputUserTransform->Delete();
  for(int iii=0; iii<NUMBER_OF_ADDITIONAL_VOLUMES; iii++) //Mehdi
  {
	  this->SetAdditionalTransformedInput(NULL, iii); //Mehdi
	  this->AdditionalLastInput[iii] = NULL;//Mehdi
	  this->AdditionalInputUserTransform[iii]->Delete();//Mehdi
  }
  
}

// ----------------------------------------------------------------------------
void vtkGPUMultiVolumeRayCastMapper::SetAdditionalInputUserTransform(int volNumber,
      vtkTransform *t)//Mehdi
{
  this->AdditionalInputUserTransform[volNumber]->DeepCopy(t);//Mehdi
}

// ----------------------------------------------------------------------------
/*Mehdi
vtkTransform *vtkGPUMultiVolumeRayCastMapper::
    GetSecondInputUserTransform()
{
  return this->SecondInputUserTransform;
}
Mehdi*/
vtkTransform *vtkGPUMultiVolumeRayCastMapper::
    GetAdditionalInputUserTransform(int volNumber)//Mehdi
{
	return this->AdditionalInputUserTransform[volNumber];//Mehdi
}

// ----------------------------------------------------------------------------
//New functions added
void vtkGPUMultiVolumeRayCastMapper::SetInput( int port, vtkDataSet *genericInput )
{
  vtkImageData *input = 
    vtkImageData::SafeDownCast( genericInput );
  
  if ( input )
    {
    SetInput( port, input );
    }
  else
    {
    vtkErrorMacro("The SetInput method of this mapper requires vtkImageData as input");
    }
}

// ----------------------------------------------------------------------------
void vtkGPUMultiVolumeRayCastMapper::SetInput( int port, vtkImageData *input )
{
  if(input)
    {
    this->SetInputConnection(port, input->GetProducerPort());
    }
  else
    {
    // Setting a NULL input removes the connection.
    this->SetInputConnection(port,0);
    }
}


vtkImageData * vtkGPUMultiVolumeRayCastMapper::GetInput(int port)
{
  if (this->GetNumberOfInputConnections(port) < 1)
    {
    return 0;
    }
  
  return vtkImageData::SafeDownCast(
    this->GetExecutive()->GetInputData(port, 0));
}


/*Mehdi
void vtkGPUMultiVolumeRayCastMapper::SetProperty2(vtkVolumeProperty *property)
{
  if( this->Property2 != property )
    {
    if (this->Property2 != NULL) {this->Property2->UnRegister(this);}
    this->Property2 = property;
    if (this->Property2 != NULL) 
      {
      this->Property2->Register(this);
      this->Property2->UpdateMTimes();
      }
    this->Modified();
    }

}
Mehdi*/
void vtkGPUMultiVolumeRayCastMapper::SetAdditionalProperty(int volNumber, vtkVolumeProperty *property) //Mehdi
{

	if( this->Property[volNumber] != property )
    {
    if (this->Property[volNumber] != NULL) {this->Property[volNumber]->UnRegister(this);}
    this->Property[volNumber] = property;
    if (this->Property[volNumber] != NULL) 
      {
      this->Property[volNumber]->Register(this);
      this->Property[volNumber]->UpdateMTimes();
      }
    this->Modified();
    }

}

/*Mehdi
vtkVolumeProperty *vtkGPUMultiVolumeRayCastMapper::GetProperty2()
{
  if( this->Property2 == NULL )
    {
    this->Property2 = vtkVolumeProperty::New();
    this->Property2->Register(this);
    this->Property2->Delete();
    }
  return this->Property2;

}
Mehdi*/
vtkVolumeProperty *vtkGPUMultiVolumeRayCastMapper::GetAdditionalProperty(int volNumber) //Mehdi
{
	
  if( this->Property[volNumber] == NULL )
    {
    this->Property[volNumber] = vtkVolumeProperty::New();
    this->Property[volNumber]->Register(this);
    this->Property[volNumber]->Delete();
    }
  return this->Property[volNumber];

}


// ----------------------------------------------------------------------------
vtkGPUMultiVolumeRayCastMapper *vtkGPUMultiVolumeRayCastMapper::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret =
    vtkVolumeRenderingFactory::CreateInstance("vtkGPUMultiVolumeRayCastMapper");
  return static_cast<vtkGPUMultiVolumeRayCastMapper*>(ret);
}

// ----------------------------------------------------------------------------
// The render method that is called from the volume. If this is a canonical
// view render, a specialized version of this method will be called instead.
// Otherwise we will
//   - Invoke a start event
//   - Start timing
//   - Check that everything is OK for rendering
//   - Render
//   - Stop the timer and record results
//   - Invoke an end event
// ----------------------------------------------------------------------------
void vtkGPUMultiVolumeRayCastMapper::Render( vtkRenderer *ren, vtkVolume *vol )
{
  // Catch renders that are happening due to a canonical view render and
  // handle them separately.
  if (this->GeneratingCanonicalView )
    {
    this->CanonicalViewRender(ren, vol);
    return;
    }

  // Invoke a VolumeMapperRenderStartEvent
  this->InvokeEvent(vtkCommand::VolumeMapperRenderStartEvent,0);

  // Start the timer to time the length of this render
  vtkTimerLog *timer = vtkTimerLog::New();
  timer->StartTimer();

  // Make sure everything about this render is OK.
  // This is where the input is updated.
  if ( this->ValidateRender(ren, vol ) )
    {
    // Everything is OK - so go ahead and really do the render
    this->GPURender( ren, vol);
    }

  // Stop the timer
  timer->StopTimer();
  double t = timer->GetElapsedTime();

//  cout << "Render Timer " << t << " seconds, " << 1.0/t << " frames per second" << endl;

  this->TimeToDraw = t;
  timer->Delete();

  if ( vol->GetAllocatedRenderTime() < 1.0 )
    {
    this->SmallTimeToDraw = t;
    }
  else
    {
    this->BigTimeToDraw = t;
    }

  // Invoke a VolumeMapperRenderEndEvent
  this->InvokeEvent(vtkCommand::VolumeMapperRenderEndEvent,0);
}

// ----------------------------------------------------------------------------
// Special version for rendering a canonical view - we don't do things like
// invoke start or end events, and we don't capture the render time.
// ----------------------------------------------------------------------------
void vtkGPUMultiVolumeRayCastMapper::CanonicalViewRender(vtkRenderer *ren,
                                                    vtkVolume *vol )
{
  // Make sure everything about this render is OK
  if ( this->ValidateRender(ren, vol ) )
    {
    // Everything is OK - so go ahead and really do the render
    this->GPURender( ren, vol);
    }
}

// ----------------------------------------------------------------------------
// This method us used by the render method to validate everything before
// attempting to render. This method returns 0 if something is not right -
// such as missing input, a null renderer or a null volume, no scalars, etc.
// In some cases it will produce a vtkErrorMacro message, and in others
// (for example, in the case of cropping planes that define a region with
// a volume or 0 or less) it will fail silently. If everything is OK, it will
// return with a value of 1.
// ----------------------------------------------------------------------------
int vtkGPUMultiVolumeRayCastMapper::ValidateRender(vtkRenderer *ren,
                                              vtkVolume *vol)
{
  // Check that we have everything we need to render.
  int goodSoFar = 1;

  // Check for a renderer - we MUST have one
  if ( !ren )
    {
    goodSoFar = 0;
    vtkErrorMacro("Renderer cannot be null.");
    }

  // Check for the volume - we MUST have one
  if ( goodSoFar && !vol )
    {
    goodSoFar = 0;
    vtkErrorMacro("Volume cannot be null.");
    }

  // Don't need to check if we have a volume property
  // since the volume will create one if we don't. Also
  // don't need to check for the scalar opacity function
  // or the RGB transfer function since the property will
  // create them if they do not yet exist.

  // However we must currently check that the number of
  // color channels is 3
  // TODO: lift this restriction - should work with
  // gray functions as well. Right now turning off test
  // because otherwise 4 component rendering isn't working.
  // Will revisit.
  if ( goodSoFar && vol->GetProperty()->GetColorChannels() != 3 )
    {
//    goodSoFar = 0;
//    vtkErrorMacro("Must have a color transfer function.");
    }

  // Check the cropping planes. If they are invalid, just silently
  // fail. This will happen when an interactive widget is dragged
  // such that it defines 0 or negative volume - this can happen
  // and should just not render the volume.
  // Check the cropping planes
  if( goodSoFar && this->Cropping &&
     (this->CroppingRegionPlanes[0]>=this->CroppingRegionPlanes[1] ||
      this->CroppingRegionPlanes[2]>=this->CroppingRegionPlanes[3] ||
      this->CroppingRegionPlanes[4]>=this->CroppingRegionPlanes[5] ))
    {
    // No error message here - we want to be silent
    goodSoFar = 0;
    }

  // Check that we have input data
  vtkImageData *input =this->GetInput(0);

  //Mehdi vtkImageData *input2=this->GetInput(1);
  vtkImageData *Additionalinput[MAX_NUMBER_OF_ADDITIONAL_VOLUMES];//Mehdi
  for(int iii=0;iii<NUMBER_OF_ADDITIONAL_VOLUMES;iii++)//Mehdi
	Additionalinput[iii]=this->GetInput(iii+1);
  
  //-----------------------------------------------------------------------------Mehdi
  /*Mehdi
  if(goodSoFar && (input==0||input2==0))
  {
	
	vtkErrorMacro("at least one of Inputs is NULL but is required");
	goodSoFar = 0;
  }
  Mehdi*/
  if(goodSoFar)//Mehdi
  {
	int tmp=(input==0);
	for(int iii=0; iii<=NUMBER_OF_ADDITIONAL_VOLUMES;iii++) //Mehdi
	{
		if (tmp)//Mehdi
		{
			vtkErrorMacro("at least one of Inputs is NULL but is required");//Mehdi
			goodSoFar = 0;//Mehdi
			break;//Mehdi
		}
		tmp=(Additionalinput[iii]==0);//Mehdi
	}
  }
  //-----------------------------------------------------------------------------Mehdi
  if(goodSoFar)
    {
		input->Update();
		//Mehdi input2->Update();
		for(int iii=0; iii<NUMBER_OF_ADDITIONAL_VOLUMES;iii++)//Mehdi
			Additionalinput[iii]->Update();//Mehdi
    }

  // If we have a timestamp change or data change then create a new clone.
  if(goodSoFar)
  {
	  int tmp=((input != this->LastInput) || (input->GetMTime() > this->TransformedInput->GetMTime()));
	  for(int iii=0;iii<=NUMBER_OF_ADDITIONAL_VOLUMES;iii++)
	  {
		  if(tmp)
		  {
			this->LastInput  = input;
			//Mehdi this->LastInput2 = input2;
			for(int jjj=0;jjj<NUMBER_OF_ADDITIONAL_VOLUMES;jjj++) //Mehdi
				this->AdditionalLastInput[jjj] = Additionalinput[jjj];//Mehdi 

			vtkImageData* clone;
			if(!this->TransformedInput)
			{
			  clone = vtkImageData::New();
			  this->SetTransformedInput(clone);
			  clone->Delete();
			}
			else
			{
			  clone = this->TransformedInput;
			}

			clone->ShallowCopy(input);
			//------------------------------------------------------------------------Mehdi
			/*Mehdi
			vtkImageData* clone2;
    
			if(!this->TransformedInput2)
			{
			  clone2 = vtkImageData::New();
			  this->SetTransformedInput2(clone2);
			  clone2->Delete();
			}
			else
			{
			  clone2 = this->TransformedInput2;
			}

			clone2->ShallowCopy(input2);
			Mehdi*/
			vtkImageData* Additionalclone[MAX_NUMBER_OF_ADDITIONAL_VOLUMES];
			for (int i=0;i<NUMBER_OF_ADDITIONAL_VOLUMES;i++)//Mehdi
			{
				if(!this->AdditionalTransformedInput[i])//Mehdi
				{
				Additionalclone[i] = vtkImageData::New();//Mehdi
				this->SetAdditionalTransformedInput(Additionalclone[i], i);//Mehdi
				Additionalclone[i]->Delete();//Mehdi
				}
				else
				{
					Additionalclone[i] = this->AdditionalTransformedInput[i];//Mehdi
				}

				Additionalclone[i]->ShallowCopy(Additionalinput[i]);//Mehdi
			}
			//------------------------------------------------------------------------Mehdi

			// @TODO: This is the workaround to deal with GPUVolumeRayCastMapper
			// not able to handle extents starting from non zero values.
			// There is not an easy fix in the GPU volume ray cast mapper hence
			// this fix has been introduced.

			// Get the current extents.
			int extents[6], real_extents[6];
			clone->GetExtent(extents);
			clone->GetExtent(real_extents);

			// Get the current origin and spacing.
			double origin[3], spacing[3];
			clone->GetOrigin(origin);
			clone->GetSpacing(spacing);

			for (int cc=0; cc < 3; cc++)
			{
			  // Transform the origin and the extents.
			  origin[cc] = origin[cc] + extents[2*cc]*spacing[cc];
			  extents[2*cc+1] -= extents[2*cc];
			  extents[2*cc] -= extents[2*cc];
			}

			clone->SetOrigin(origin);
			clone->SetExtent(extents);

			//-----------------------------------------------------------Mehdi
			/*Mehdi
			// Get the current extents.
			int extents2[6], real_extents2[6];
			clone2->GetExtent(extents2);
			clone2->GetExtent(real_extents2);

			// Get the current origin and spacing.
			double origin2[3], spacing2[3];
			clone2->GetOrigin(origin2);
			clone2->GetSpacing(spacing2);

			for (int cc=0; cc < 3; cc++)
			  {
			  // Transform the origin and the extents.
			  origin2[cc] = origin2[cc] + extents2[2*cc]*spacing2[cc];
			  extents2[2*cc+1] -= extents2[2*cc];
			  extents2[2*cc] -= extents2[2*cc];
			  }

			clone2->SetOrigin(origin2);
			clone2->SetExtent(extents2);
			Mehdi*/
			int extents2[6], real_extents2[6];//Mehdi
			double origin2[3], spacing2[3];//Mehdi
			for(int iii=0; iii<NUMBER_OF_ADDITIONAL_VOLUMES;iii++)//Mehdi //?
			{
	
				Additionalclone[iii]->GetExtent(extents2);//Mehdi
				Additionalclone[iii]->GetExtent(real_extents2);//Mehdi

			// Get the current origin and spacing.
    
				Additionalclone[iii]->GetOrigin(origin2);//Mehdi
				Additionalclone[iii]->GetSpacing(spacing2);//Mehdi

				for (int cc=0; cc < 3; cc++)
				{
					// Transform the origin and the extents.
					origin2[cc] = origin2[cc] + extents2[2*cc]*spacing2[cc];
					extents2[2*cc+1] -= extents2[2*cc];
					extents2[2*cc] -= extents2[2*cc];
				}

				Additionalclone[iii]->SetOrigin(origin2);
				Additionalclone[iii]->SetExtent(extents2);
			}
		  //-----------------------------------------------------------Mehdi
		  break;//Mehdi
		  }
		  tmp=((Additionalinput[iii] != this->AdditionalLastInput[iii]) || (Additionalinput[iii]->GetMTime() > this->AdditionalTransformedInput[iii]->GetMTime()));//Mehdi
	  }
  }
  // Update the date then make sure we have scalars. Note
  // that we must have point or cell scalars because field
  // scalars are not supported.
  vtkDataArray *scalars = NULL;

  //Mehdi  vtkDataArray *scalars2 = NULL;
  vtkDataArray *Additionalscalars[MAX_NUMBER_OF_ADDITIONAL_VOLUMES];//Mehdi
  for(int iii=0;iii<NUMBER_OF_ADDITIONAL_VOLUMES;iii++)//Mehdi
	  Additionalscalars[iii]=NULL;//Mehdi


  if ( goodSoFar )
  {
    // Here is where we update the input
    this->TransformedInput->UpdateInformation();
    this->TransformedInput->SetUpdateExtentToWholeExtent();
    this->TransformedInput->Update();

    // Now make sure we can find scalars
    scalars=this->GetScalars(this->TransformedInput,this->ScalarMode,
                             this->ArrayAccessMode,
                             this->ArrayId,
                             this->ArrayName,
                             this->CellFlag);

    // We couldn't find scalars
    if ( !scalars )
      {
      vtkErrorMacro("No scalars found on input.");
      goodSoFar = 0;
      }
    // Even if we found scalars, if they are field data scalars that isn't good
    else if ( this->CellFlag == 2 )
      {
      vtkErrorMacro("Only point or cell scalar support - found field scalars instead.");
      goodSoFar = 0;
      }

    // Here is where we update the second input
	/*Mehdi
    this->TransformedInput2->UpdateInformation();
    this->TransformedInput2->SetUpdateExtentToWholeExtent();
    this->TransformedInput2->Update();
	Mehdi*/
	for(int iii=0;iii<NUMBER_OF_ADDITIONAL_VOLUMES;iii++)//Mehdi
	{
		this->AdditionalTransformedInput[iii]->UpdateInformation();//Mehdi
		this->AdditionalTransformedInput[iii]->SetUpdateExtentToWholeExtent();//Mehdi
		this->AdditionalTransformedInput[iii]->Update();//Mehdi
	

		// Now make sure we can find scalars
		Additionalscalars[iii]=this->GetScalars(this->AdditionalTransformedInput[iii],this->ScalarMode,//Mehdi
								 this->ArrayAccessMode,
								 this->ArrayId,
								 this->ArrayName,
								 this->CellFlag);

		// We couldn't find scalars
		if ( !Additionalscalars[iii] )
		{
		  vtkErrorMacro("No scalars found on input.");
		  goodSoFar = 0;
		}
		// Even if we found scalars, if they are field data scalars that isn't good
		else if ( this->CellFlag == 2 )
		{
		  vtkErrorMacro("Only point or cell scalar support - found field scalars instead.");
		  goodSoFar = 0;
		}
      
	}
      
  }

  // Make sure the scalar type is actually supported. This mappers supports
  // almost all standard scalar types.
  if ( goodSoFar )
    {
    switch(scalars->GetDataType())
      {
      case VTK_CHAR:
        vtkErrorMacro(<< "scalar of type VTK_CHAR is not supported "
                      << "because this type is platform dependent. "
                      << "Use VTK_SIGNED_CHAR or VTK_UNSIGNED_CHAR instead.");
        goodSoFar = 0;
        break;
      case VTK_BIT:
        vtkErrorMacro("scalar of type VTK_BIT is not supported by this mapper.");
        goodSoFar = 0;
        break;
      case VTK_ID_TYPE:
        vtkErrorMacro("scalar of type VTK_ID_TYPE is not supported by this mapper.");
        goodSoFar = 0;
        break;
      case VTK_STRING:
        vtkErrorMacro("scalar of type VTK_STRING is not supported by this mapper.");
        goodSoFar = 0;
        break;
      default:
        // Don't need to do anything here
        break;
      }
    
	//---------------------------------------------------------------------------------------------Mehdi
	/*Mehdi
    // for second input
	switch(scalars2->GetDataType())
      {
      case VTK_CHAR:
        vtkErrorMacro(<< "scalar of type VTK_CHAR is not supported "
                      << "because this type is platform dependent. "
                      << "Use VTK_SIGNED_CHAR or VTK_UNSIGNED_CHAR instead.");
        goodSoFar = 0;
        break;
      case VTK_BIT:
        vtkErrorMacro("scalar of type VTK_BIT is not supported by this mapper.");
        goodSoFar = 0;
        break;
      case VTK_ID_TYPE:
        vtkErrorMacro("scalar of type VTK_ID_TYPE is not supported by this mapper.");
        goodSoFar = 0;
        break;
      case VTK_STRING:
        vtkErrorMacro("scalar of type VTK_STRING is not supported by this mapper.");
        goodSoFar = 0;
        break;
      default:
        // Don't need to do anything here
        break;
      }
    
	Mehdi*/
	for(int iii=0; iii<NUMBER_OF_ADDITIONAL_VOLUMES; iii++)//Mehdi
	{
	
	  switch(Additionalscalars[iii]->GetDataType())//Mehdi
      {
      case VTK_CHAR:
        vtkErrorMacro(<< "scalar of type VTK_CHAR is not supported "
                      << "because this type is platform dependent. "
                      << "Use VTK_SIGNED_CHAR or VTK_UNSIGNED_CHAR instead.");
        goodSoFar = 0;
        break;
      case VTK_BIT:
        vtkErrorMacro("scalar of type VTK_BIT is not supported by this mapper.");
        goodSoFar = 0;
        break;
      case VTK_ID_TYPE:
        vtkErrorMacro("scalar of type VTK_ID_TYPE is not supported by this mapper.");
        goodSoFar = 0;
        break;
      case VTK_STRING:
        vtkErrorMacro("scalar of type VTK_STRING is not supported by this mapper.");
        goodSoFar = 0;
        break;
      default:
        // Don't need to do anything here
        break;
      }
	
	}
    //---------------------------------------------------------------------------------------------Mehdi
  }
  // Check on the blending type - we support composite and min / max intensity
  if ( goodSoFar )
    {
    if(this->BlendMode!=vtkVolumeMapper::COMPOSITE_BLEND &&
       this->BlendMode!=vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND &&
       this->BlendMode!=vtkVolumeMapper::MINIMUM_INTENSITY_BLEND &&
       this->BlendMode!=vtkVolumeMapper::ADDITIVE_BLEND)
      {
      goodSoFar = 0;
      vtkErrorMacro(<< "Selected blend mode not supported. "
                    << "Only Composite, MIP, MinIP and additive modes "
                    << "are supported by the current implementation.");
      }
    }

  /*Mehdi-TODO
  // This mapper supports 1 component data, or 4 component if it is not independent
  // component (i.e. the four components define RGBA)
  int numberOfComponents = 0;
  //Mehdi int numberOfComponents2 = 0;
  int AdditionalnumberOfComponents[MAX_NUMBER_OF_VOLUMES];//Mehdi
  for(int i=0;i<MAX_NUMBER_OF_VOLUMES;i++)//Mehdi
	  AdditionalnumberOfComponents[i]=0;//Mehdi

  
  if ( goodSoFar )
    {
    numberOfComponents=scalars->GetNumberOfComponents();
    
	//Mehdi numberOfComponents2=scalars2->GetNumberOfComponents();
	AdditionalnumberOfComponents[i]=scalars2->GetNumberOfComponents();
    if( 
         !( numberOfComponents==1 ||
           (numberOfComponents==4 &&
            vol->GetProperty()->GetIndependentComponents()==0))
        ||
         !( numberOfComponents2==1 ||
           (numberOfComponents2==4 &&
            vol->GetProperty()->GetIndependentComponents()==0))
      )
      {
      goodSoFar = 0;
      vtkErrorMacro(<< "Only one component scalars, or four "
                    << "component with non-independent components, "
                    << "are supported by this mapper.");
      }
    }
	
  // If this is four component data, then it better be unsigned char (RGBA).
  if( goodSoFar &&
      numberOfComponents == 4 &&
      scalars->GetDataType() != VTK_UNSIGNED_CHAR)
    {
    goodSoFar = 0;
    vtkErrorMacro("Only unsigned char is supported for 4-component scalars!");
    }

  if(goodSoFar && numberOfComponents!=1 &&
     this->BlendMode==vtkVolumeMapper::ADDITIVE_BLEND)
    {
    goodSoFar=0;
    vtkErrorMacro("Additive mode only works with 1-component scalars!");
    }
  */

  // return our status
  return goodSoFar;
}


// ----------------------------------------------------------------------------
// Description:
// Called by the AMR Volume Mapper.
// Set the flag that tells if the scalars are on point data (0) or
// cell data (1).
void vtkGPUMultiVolumeRayCastMapper::SetCellFlag(int cellFlag)
{
  this->CellFlag=cellFlag;
}

// ----------------------------------------------------------------------------
void vtkGPUMultiVolumeRayCastMapper::CreateCanonicalView(
  vtkRenderer *ren,
  vtkVolume *volume,
  vtkImageData *image,
  int vtkNotUsed(blend_mode),
  double viewDirection[3],
  double viewUp[3])
{
  this->GeneratingCanonicalView = 1;
  int oldSwap = ren->GetRenderWindow()->GetSwapBuffers();
  ren->GetRenderWindow()->SwapBuffersOff();


  int dim[3];
  image->GetDimensions(dim);
  int *size = ren->GetRenderWindow()->GetSize();

  vtkImageData *bigImage = vtkImageData::New();
  bigImage->SetDimensions(size[0], size[1], 1);
  bigImage->SetScalarTypeToUnsignedChar();
  bigImage->SetNumberOfScalarComponents(3);
  bigImage->AllocateScalars();

  this->CanonicalViewImageData = bigImage;


  double scale[2];
  scale[0] = dim[0] / static_cast<double>(size[0]);
  scale[1] = dim[1] / static_cast<double>(size[1]);


  // Save the visibility flags of the renderers and set all to false except
  // for the ren.
  vtkRendererCollection *renderers=ren->GetRenderWindow()->GetRenderers();
  int numberOfRenderers=renderers->GetNumberOfItems();

  bool *rendererVisibilities=new bool[numberOfRenderers];
  renderers->InitTraversal();
  int i=0;
  while(i<numberOfRenderers)
    {
    vtkRenderer *r=renderers->GetNextItem();
    rendererVisibilities[i]=r->GetDraw()==1;
    if(r!=ren)
      {
      r->SetDraw(false);
      }
    ++i;
    }

  // Save the visibility flags of the props and set all to false except
  // for the volume.

  vtkPropCollection *props=ren->GetViewProps();
  int numberOfProps=props->GetNumberOfItems();

  bool *propVisibilities=new bool[numberOfProps];
  props->InitTraversal();
  i=0;
  while(i<numberOfProps)
    {
    vtkProp *p=props->GetNextProp();
    propVisibilities[i]=p->GetVisibility()==1;
    if(p!=volume)
      {
      p->SetVisibility(false);
      }
    ++i;
    }

  vtkCamera *savedCamera=ren->GetActiveCamera();
  savedCamera->Modified();
  vtkCamera *canonicalViewCamera=vtkCamera::New();

  // Code from vtkFixedPointVolumeRayCastMapper:
  double *center=volume->GetCenter();
  double bounds[6];
  volume->GetBounds(bounds);
  double d=sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
                (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
                (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));

  // For now use x distance - need to change this
  d=bounds[1]-bounds[0];

  // Set up the camera in parallel
  canonicalViewCamera->SetFocalPoint(center);
  canonicalViewCamera->ParallelProjectionOn();
  canonicalViewCamera->SetPosition(center[0] - d*viewDirection[0],
                                   center[1] - d*viewDirection[1],
                                   center[2] - d*viewDirection[2]);
  canonicalViewCamera->SetViewUp(viewUp);
  canonicalViewCamera->SetParallelScale(d/2);

  ren->SetActiveCamera(canonicalViewCamera);
  ren->GetRenderWindow()->Render();


  ren->SetActiveCamera(savedCamera);
  canonicalViewCamera->Delete();


  // Shrink to image to the desired size
  vtkImageResample *resample = vtkImageResample::New();
  resample->SetInput( bigImage );
  resample->SetAxisMagnificationFactor(0,scale[0]);
  resample->SetAxisMagnificationFactor(1,scale[1]);
  resample->SetAxisMagnificationFactor(2,1);
  resample->UpdateWholeExtent();

  // Copy the pixels over
  image->DeepCopy(resample->GetOutput());

  bigImage->Delete();
  resample->Delete();

  // Restore the visibility flags of the props
  props->InitTraversal();
  i=0;
  while(i<numberOfProps)
    {
    vtkProp *p=props->GetNextProp();
    p->SetVisibility(propVisibilities[i]);
    ++i;
    }

  delete[] propVisibilities;

  // Restore the visibility flags of the renderers
  renderers->InitTraversal();
  i=0;
  while(i<numberOfRenderers)
    {
    vtkRenderer *r=renderers->GetNextItem();
    r->SetDraw(rendererVisibilities[i]);
    ++i;
    }

  delete[] rendererVisibilities;

  ren->GetRenderWindow()->SetSwapBuffers(oldSwap);
  this->CanonicalViewImageData = NULL;
  this->GeneratingCanonicalView = 0;
}

// ----------------------------------------------------------------------------
// Print method for vtkGPUMultiVolumeRayCastMapper
void vtkGPUMultiVolumeRayCastMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "AutoAdjustSampleDistances: "
     << this->AutoAdjustSampleDistances << endl;
  os << indent << "MinimumImageSampleDistance: "
     << this->MinimumImageSampleDistance << endl;
  os << indent << "MaximumImageSampleDistance: "
     << this->MaximumImageSampleDistance << endl;
  os << indent << "ImageSampleDistance: " << this->ImageSampleDistance << endl;
  os << indent << "SampleDistance: " << this->SampleDistance << endl;
  os << indent << "FinalColorWindow: " << this->FinalColorWindow << endl;
  os << indent << "FinalColorLevel: " << this->FinalColorLevel << endl;
  os << indent << "MaskInput: " << this->MaskInput << endl;
  os << indent << "MaskType: " << this->MaskType << endl;
  os << indent << "MaskBlendFactor: " << this->MaskBlendFactor << endl;
  os << indent << "MaxMemoryInBytes: " << this->MaxMemoryInBytes << endl;
  os << indent << "MaxMemoryFraction: " << this->MaxMemoryFraction << endl;
  os << indent << "ReportProgress: " << this->ReportProgress << endl;
}

// ----------------------------------------------------------------------------
// Description:
// Compute the cropping planes clipped by the bounds of the volume.
// The result is put into this->ClippedCroppingRegionPlanes.
// NOTE: IT WILL BE MOVED UP TO vtkVolumeMapper after bullet proof usage
// in this mapper. Other subclasses will use the ClippedCroppingRegionsPlanes
// members instead of CroppingRegionPlanes.
// \pre volume_exists: this->GetInput()!=0
// \pre valid_cropping: this->Cropping &&
//             this->CroppingRegionPlanes[0]<this->CroppingRegionPlanes[1] &&
//             this->CroppingRegionPlanes[2]<this->CroppingRegionPlanes[3] &&
//             this->CroppingRegionPlanes[4]<this->CroppingRegionPlanes[5])
void vtkGPUMultiVolumeRayCastMapper::ClipCroppingRegionPlanes()
{
  assert("pre: volume_exists" && this->GetInput(0)!=0);
  assert("pre: valid_cropping" && this->Cropping &&
         this->CroppingRegionPlanes[0]<this->CroppingRegionPlanes[1] &&
         this->CroppingRegionPlanes[2]<this->CroppingRegionPlanes[3] &&
         this->CroppingRegionPlanes[4]<this->CroppingRegionPlanes[5]);

  // vtkVolumeMapper::Render() will have something like:
//  if(this->Cropping && (this->CroppingRegionPlanes[0]>=this->CroppingRegionPlanes[1] ||
//                        this->CroppingRegionPlanes[2]>=this->CroppingRegionPlanes[3] ||
//                        this->CroppingRegionPlanes[4]>=this->CroppingRegionPlanes[5]))
//    {
//    // silentely  stop because the cropping is not valid.
//    return;
//    }

  double volBounds[6];
  this->GetInput(0)->GetBounds(volBounds);

  int i=0;
  while(i<6)
    {
    // max of the mins
    if(this->CroppingRegionPlanes[i]<volBounds[i])
      {
      this->ClippedCroppingRegionPlanes[i]=volBounds[i];
      }
    else
      {
      this->ClippedCroppingRegionPlanes[i]=this->CroppingRegionPlanes[i];
      }
    ++i;
    // min of the maxs
    if(this->CroppingRegionPlanes[i]>volBounds[i])
      {
      this->ClippedCroppingRegionPlanes[i]=volBounds[i];
      }
    else
      {
      this->ClippedCroppingRegionPlanes[i]=this->CroppingRegionPlanes[i];
      }
    ++i;
    }
}

//----------------------------------------------------------------------------
void vtkGPUMultiVolumeRayCastMapper::SetMaskTypeToBinary()
{
  this->MaskType = vtkGPUMultiVolumeRayCastMapper::BinaryMaskType;
}

//----------------------------------------------------------------------------
void vtkGPUMultiVolumeRayCastMapper::SetMaskTypeToLabelMap()
{
  this->MaskType = vtkGPUMultiVolumeRayCastMapper::LabelMapMaskType;
}
