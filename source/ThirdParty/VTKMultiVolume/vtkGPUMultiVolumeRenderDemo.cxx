/*=========================================================================

  Program:   Visualization Toolkit
  Module:    GPURenderDemo.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// VTK includes
#include "vtkBoxWidget.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkColorTransferFunction.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkImageResample.h"
#include "vtkMetaImageReader.h"
#include <vtkVolume16Reader.h>
#include "vtkPiecewiseFunction.h"
#include "vtkPlanes.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkXMLImageDataReader.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"
#include <vtkSmartPointer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkTransform.h>
#include <vtkCallbackCommand.h>//Mehdi
  
#define VTI_FILETYPE 1
#define MHA_FILETYPE 2
#define V16_FILETYPE 3

//------------------------------------------------------------------------------
// Callback for moving the planes from the box widget to the mapper
//------------------------------------------------------------------------------
class vtkBoxWidgetCallback : public vtkCommand
{
public:
  static vtkBoxWidgetCallback *New()
    { return new vtkBoxWidgetCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
      if (this->Mapper)
        {
        vtkPlanes *planes = vtkPlanes::New();
        widget->GetPlanes(planes);
        this->Mapper->SetClippingPlanes(planes);
        planes->Delete();
        }
    }
  void SetMapper(vtkVolumeMapper* m) 
    { this->Mapper = m; }

protected:
  vtkBoxWidgetCallback() 
    { this->Mapper = 0; }

  vtkVolumeMapper *Mapper;
};

//------------------------------------------------------------------------------
// Callback for moving the planes from the box widget to the mapper
//------------------------------------------------------------------------------
class vtkTransformBoxWidgetCallback : public vtkCommand
{
public:
  static vtkTransformBoxWidgetCallback *New(int VolNum) //Mehdi
  { 
	  return new vtkTransformBoxWidgetCallback(VolNum); //Mehdi
  }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
      vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
      if (this->Mapper)
      {
        vtkTransform* t = vtkTransform::New();
        widget->GetTransform(t);
		this->Mapper->SetAdditionalInputUserTransform(volumeNumber,t); //Mehdi
        t->Delete();
      }
  }
  void SetMapper(vtkOpenGLGPUMultiVolumeRayCastMapper* m) 
  { 
	  this->Mapper = m; 
  }

protected:
  int volumeNumber;
  vtkTransformBoxWidgetCallback(int VolNumber) 
  { 
	  this->Mapper = 0;
	  volumeNumber = VolNumber;
  }
  vtkOpenGLGPUMultiVolumeRayCastMapper *Mapper;
};


void CallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData) )
{
  vtkRenderer* renderer = static_cast<vtkRenderer*>(caller);
 
  double timeInSeconds = renderer->GetLastRenderTimeInSeconds();
  double fps = 1.0/timeInSeconds;
  std::cout << "FPS: " << fps << std::endl;
 
}


void PrintUsage()
{
  cout << "Usage: " << endl;
  cout << endl;
  cout << "  GPURenderDemo  <voloptions> -NextVol <voloptions> <options> -NextVol..." << endl; //Mehdi
  cout << endl;
  cout << "where voloptions may include: " << endl;
  cout << endl;
  cout << "  -DICOM <directory>" << endl;
  cout << "  -VTI <filename>" << endl;
  cout << "  -MHA <filename>" << endl;
  cout << "  -V16 DATADIR/headsq/quarter" << endl;
  cout << "  -DependentComponents" << endl;
  cout << "  -MIP <window> <level>" << endl;
  cout << "  -CompositeRamp <window> <level>" << endl;
  cout << "  -CompositeShadeRamp <window> <level>" << endl;
  cout << "  -CT_Skin" << endl;
  cout << "  -CT_Bone" << endl;
  cout << "  -CT_Muscle" << endl;
  cout << "  -DataReduction <factor>" << endl;
  cout << endl;
  cout << "and where options may include: " << endl;
  cout << "  -FrameRate <rate>" << endl;
  cout << "  -Clip" << endl;
  cout << endl;
  cout << "You must use either the -DICOM option to specify the directory where" << endl;
  cout << "the data is located or the -VTI or -MHA option to specify the path of a .vti file." << endl;
  cout << endl;
  cout << "By default, the program assumes that the file has independent components," << endl;
  cout << "use -DependentComponents to specify that the file has dependent components." << endl;
  cout << endl;
  cout << "Use the -Clip option to display a cube widget for clipping the volume." << endl;
  cout << "Use the -FrameRate option with a desired frame rate (in frames per second)" << endl;
  cout << "which will control the interactive rendering rate." << endl;
  cout << "Use the -DataReduction option with a reduction factor (greater than zero and" << endl;
  cout << "less than one) to reduce the data before rendering." << endl;
  cout << "Use one of the remaining options to specify the blend function" << endl;
  cout << "and transfer functions. The -MIP option utilizes a maximum intensity" << endl;
  cout << "projection method, while the others utilize compositing. The" << endl;
  cout << "-CompositeRamp option is unshaded compositing, while the other" << endl;
  cout << "compositing options employ shading." << endl;
  cout << endl;
  cout << "Note: MIP, CompositeRamp, CompositeShadeRamp, CT_Skin, CT_Bone," << endl;
  cout << "and CT_Muscle are appropriate for DICOM data. MIP, CompositeRamp," << endl;
  cout << "and RGB_Composite are appropriate for RGB data." << endl;
  cout << endl;
  cout << "Example: GPURenderDemo -DICOM CTNeck -MIP 4096 1024" << endl;
  cout << endl;
}


class VolumeInfo 
{
public:
  char *dirname;
  double opacityWindow;
  double opacityLevel;
  int blendType;
  double reductionFactor;
  char *fileName;
  int fileType;

  VolumeInfo() 
  {
    dirname = NULL;
    opacityWindow = 4096;
    opacityLevel = 2048;
    blendType = 0;
    reductionFactor = 1.0;
    fileName=0;
    fileType=0;
  }
};

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

// Information and parameters for each volume
VolumeInfo vol_info[MAX_NUMBER_OF_ADDITIONAL_VOLUMES+1];//Mehdi
int clip = 0;
double frameRate = 10.0;
bool independentComponents=true;
int numberOfAdditionalVolumes=0;

//------------------------------------------------------------------------------
// ParseArguments
//------------------------------------------------------------------------------
void ParseArguments(int argc, char *argv[])
{
  int current_volume=0;
  int count = 1;
  while ( count < argc )
    {
    if ( !strcmp( argv[count], "?" ) )
      {
      PrintUsage();
      exit(EXIT_SUCCESS);
      }
    else if ( !strcmp( argv[count], "-NextVol" ) )
      {
        current_volume++;
        count++;
      }
    else if ( !strcmp( argv[count], "-DICOM" ) )
      {
      vol_info[current_volume].dirname = new char[strlen(argv[count+1])+1];
      sprintf( vol_info[current_volume].dirname, "%s", argv[count+1] );
      count += 2;
      }
    else if ( !strcmp( argv[count], "-VTI" ) )
      {
      vol_info[current_volume].fileName = new char[strlen(argv[count+1])+1];
      vol_info[current_volume].fileType = VTI_FILETYPE;
      sprintf( vol_info[current_volume].fileName, "%s", argv[count+1] );
      count += 2;
      }
    else if ( !strcmp( argv[count], "-MHA" ) )
      {
      vol_info[current_volume].fileName = new char[strlen(argv[count+1])+1];
      vol_info[current_volume].fileType = MHA_FILETYPE;
      sprintf( vol_info[current_volume].fileName, "%s", argv[count+1] );
      count += 2;
      }
    else if ( !strcmp( argv[count], "-V16" ) )
      {
        std::cout << "-V16 found " << std::endl;
      vol_info[current_volume].fileName = new char[strlen(argv[count+1])+1];
      vol_info[current_volume].fileType = V16_FILETYPE;
      sprintf( vol_info[current_volume].fileName, "%s", argv[count+1] );
      count += 2;
      }
    else if ( !strcmp( argv[count], "-Clip") )
      {
      clip = 1;
      count++;
      }
    else if ( !strcmp( argv[count], "-MIP" ) )
      {
      vol_info[current_volume].opacityWindow = atof( argv[count+1] );
      vol_info[current_volume].opacityLevel  = atof( argv[count+2] );
      vol_info[current_volume].blendType = 0;
      count += 3;
      }
    else if ( !strcmp( argv[count], "-CompositeRamp" ) )
      {
      vol_info[current_volume].opacityWindow = atof( argv[count+1] );
      vol_info[current_volume].opacityLevel  = atof( argv[count+2] );
      vol_info[current_volume].blendType = 1;
      count += 3;
      }
    else if ( !strcmp( argv[count], "-CompositeShadeRamp" ) )
      {
      vol_info[current_volume].opacityWindow = atof( argv[count+1] );
      vol_info[current_volume].opacityLevel  = atof( argv[count+2] );
      vol_info[current_volume].blendType = 2;
      count += 3;
      }
    else if ( !strcmp( argv[count], "-CT_Skin" ) )
      {
      vol_info[current_volume].blendType = 3;
      count += 1;
      }
    else if ( !strcmp( argv[count], "-CT_Bone" ) )
      {
      vol_info[current_volume].blendType = 4;
      count += 1;
      }
    else if ( !strcmp( argv[count], "-CT_Muscle" ) )
      {
      vol_info[current_volume].blendType = 5;
      count += 1;
      }
    else if ( !strcmp( argv[count], "-RGB_Composite" ) )
      {
      vol_info[current_volume].blendType = 6;
      count += 1;
      }    
    else if ( !strcmp( argv[count], "-FrameRate") )
      {
      frameRate = atof( argv[count+1] );
      if ( frameRate < 0.01 || frameRate > 60.0 )
        {
        cout << "Invalid frame rate - use a number between 0.01 and 60.0" << endl;
        cout << "Using default frame rate of 10 frames per second." << endl;
        frameRate = 10.0;
        }
      count += 2;
      }
    else if ( !strcmp( argv[count], "-ReductionFactor") )
      {
      vol_info[current_volume].reductionFactor = atof( argv[count+1] );
      if (  vol_info[current_volume].reductionFactor <= 0.0 || 
            vol_info[current_volume].reductionFactor >= 1.0 )
        {
        cout << "Invalid reduction factor - use a number between 0 and 1 (exclusive)" << endl;
        cout << "Using the default of no reduction." << endl;
        vol_info[current_volume].reductionFactor = 1.0;
        }
      count += 2;
      }
     else if ( !strcmp( argv[count], "-DependentComponents") )
      {
      independentComponents=false;
      count += 1;
      }
    else
      {
      cout << "Unrecognized option: " << argv[count] << endl;
      cout << endl;
      PrintUsage();
      exit(EXIT_FAILURE);
      }    
    }
  
  if ( !vol_info[current_volume].dirname && !vol_info[current_volume].fileName)
    {
    cout << "Error: you must specify a directory of DICOM data or a .vti file or a .mha!" << endl;
    cout << endl;
    PrintUsage();
    exit(EXIT_FAILURE);    
    }
  numberOfAdditionalVolumes=current_volume;
}

//------------------------------------------------------------------------------
// CreateTransferFunctions
//------------------------------------------------------------------------------
void CreateTransferFunctions( int vol, const VolumeInfo& vi, 
                              vtkColorTransferFunction* colorFun,                              vtkPiecewiseFunction*     opacityFun,
                              vtkVolumeProperty*        property,
                              vtkOpenGLGPUMultiVolumeRayCastMapper *mapper
                            )
{
  // Depending on the blend type selected as a command line option, 
  // adjust the transfer function
  switch ( vi.blendType )
    {
    // MIP
    // Create an opacity ramp from the window and level values.
    // Color is white. Blending is MIP.
    case 0:
      colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 );
      opacityFun->AddSegment( vi.opacityLevel - 0.5*vi.opacityWindow, 0.0, 
                              vi.opacityLevel + 0.5*vi.opacityWindow, 1.0 );
      mapper->SetBlendModeToMaximumIntensity();
      break;
      
    // CompositeRamp
    // Create a ramp from the window and level values. Use compositing
    // without shading. Color is a ramp from black to white.
    case 1:
      colorFun->AddRGBSegment(vi.opacityLevel - 0.5*vi.opacityWindow, 
                              0.0, 0.0, 0.0, 
                              vi.opacityLevel + 0.5*vi.opacityWindow, 
                              1.0, 1.0, 1.0 );
      opacityFun->AddSegment( vi.opacityLevel - 0.5*vi.opacityWindow, 0.0, 
                              vi.opacityLevel + 0.5*vi.opacityWindow, 1.0 );
      mapper->SetBlendModeToComposite();
      property->ShadeOff();
      break;
      
    // CompositeShadeRamp
    // Create a ramp from the window and level values. Use compositing
    // with shading. Color is white.
    case 2:
		switch(vol)//Mehdi
		{
			case 0:	colorFun->AddRGBSegment(0.0, 0.0, 0.0, 0.0, 255.0, 1.0, 1.0, 1.0 ); break;//? /*M*/
			case 1:	colorFun->AddRGBSegment(0.0, 1.0, 0.0, 0.0, 255.0, 1.0, 1.0, 1.0 ); break;//? /*M*/
			case 2:	colorFun->AddRGBSegment(0.0, 0.0, 1.0, 0.0, 255.0, 1.0, 1.0, 1.0 ); break;//? /*M*/
			case 3:	colorFun->AddRGBSegment(0.0, 0.0, 0.0, 1.0, 255.0, 1.0, 1.0, 1.0 ); break;//? /*M*/
			case 4:	colorFun->AddRGBSegment(0.0, 1.0, 1.0, 0.0, 255.0, 1.0, 1.0, 1.0 ); break;//? /*M*/
			case 5:	colorFun->AddRGBSegment(0.0, 0.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 ); break;//? /*M*/
			case 6:	colorFun->AddRGBSegment(0.0, 1.0, 0.0, 1.0, 255.0, 1.0, 1.0, 1.0 ); break;//? /*M*/
			default:colorFun->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0 ); break;//? /*M*/
		}
      opacityFun->AddSegment( vi.opacityLevel - 0.5*vi.opacityWindow, 0.0, 
                              vi.opacityLevel + 0.5*vi.opacityWindow, 1.0 );
      mapper->SetBlendModeToComposite();
      property->ShadeOn();

      break;

    // CT_Skin
    // Use compositing and functions set to highlight skin in CT data
    // Not for use on RGB data
    case 3:
      colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
      colorFun->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );
      colorFun->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
      colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );
      
      opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-1000, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-500, 1.0, 0.33, 0.45 );
      opacityFun->AddPoint(3071, 1.0, 0.5, 0.0);

      mapper->SetBlendModeToComposite();
      property->ShadeOn();
      property->SetAmbient(0.1);
      property->SetDiffuse(0.9);
      property->SetSpecular(0.2);
      property->SetSpecularPower(10.0);
      property->SetScalarOpacityUnitDistance(0.8919);
      break;
      
    // CT_Bone
    // Use compositing and functions set to highlight bone in CT data
    // Not for use on RGB data
    case 4:
      colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
      colorFun->AddRGBPoint( -16, 0.73, 0.25, 0.30, 0.49, .61 );
      colorFun->AddRGBPoint( 641, .90, .82, .56, .5, 0.0 );
      colorFun->AddRGBPoint( 3071, 1, 1, 1, .5, 0.0 );
      
      opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-16, 0, .49, .61 );
      opacityFun->AddPoint(641, .72, .5, 0.0 );
      opacityFun->AddPoint(3071, .71, 0.5, 0.0);

      mapper->SetBlendModeToComposite();
      property->ShadeOn();
      property->SetAmbient(0.1);
      property->SetDiffuse(0.9);
      property->SetSpecular(0.2);
      property->SetSpecularPower(10.0);
      property->SetScalarOpacityUnitDistance(0.8919);
      break;
      
    // CT_Muscle
    // Use compositing and functions set to highlight muscle in CT data
    // Not for use on RGB data
    case 5:
      colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
      colorFun->AddRGBPoint( -155, .55, .25, .15, 0.5, .92 );
      colorFun->AddRGBPoint( 217, .88, .60, .29, 0.33, 0.45 );
      colorFun->AddRGBPoint( 420, 1, .94, .95, 0.5, 0.0 );
      colorFun->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );
      
      opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
      opacityFun->AddPoint(-155, 0, 0.5, 0.92 );
      opacityFun->AddPoint(217, .68, 0.33, 0.45 );
      opacityFun->AddPoint(420,.83, 0.5, 0.0);
      opacityFun->AddPoint(3071, .80, 0.5, 0.0);

      mapper->SetBlendModeToComposite();
      property->ShadeOn();
      property->SetAmbient(0.1);
      property->SetDiffuse(0.9);
      property->SetSpecular(0.2);
      property->SetSpecularPower(10.0);
      property->SetScalarOpacityUnitDistance(0.8919);
      break;
      
    // RGB_Composite
    // Use compositing and functions set to highlight red/green/blue regions
    // in RGB data. Not for use on single component data
    case 6:
      opacityFun->AddPoint(0, 0.0);
      opacityFun->AddPoint(5.0, 0.0);
      opacityFun->AddPoint(30.0, 0.05);
      opacityFun->AddPoint(31.0, 0.0);
      opacityFun->AddPoint(90.0, 0.0);
      opacityFun->AddPoint(100.0, 0.3);
      opacityFun->AddPoint(110.0, 0.0);
      opacityFun->AddPoint(190.0, 0.0);
      opacityFun->AddPoint(200.0, 0.4);
      opacityFun->AddPoint(210.0, 0.0);
      opacityFun->AddPoint(245.0, 0.0);
      opacityFun->AddPoint(255.0, 0.5);

      mapper->SetBlendModeToComposite();
      property->ShadeOff();
      property->SetScalarOpacityUnitDistance(1.0);
      break;
    default:
      vtkGenericWarningMacro("Unknown blend type.");
      break;
    }
}


//------------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  
  // Parse the parameters
  ParseArguments(argc,argv);

  printf("\n%d",numberOfAdditionalVolumes);

  // Create the renderer, render window and interactor
  vtkSmartPointer<vtkRenderer>   renderer = vtkRenderer::New();
  vtkSmartPointer<vtkRenderWindow> renWin = vtkRenderWindow::New();
  renWin->AddRenderer(renderer);

  // Connect it all. Note that funny arithematic on the 
  // SetDesiredUpdateRate - the vtkRenderWindow divides it
  // allocated time across all renderers, and the renderer
  // divides it time across all props. If clip is
  // true then there are two props
  vtkSmartPointer<vtkRenderWindowInteractor> iren = 
    vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  iren->SetDesiredUpdateRate(frameRate / (1+clip) );
  
  iren->GetInteractorStyle()->SetDefaultRenderer(renderer);

  vtkSmartPointer<vtkAlgorithm>     reader  [MAX_NUMBER_OF_ADDITIONAL_VOLUMES+1]; //Mehdi
  vtkSmartPointer<vtkImageData>     input   [MAX_NUMBER_OF_ADDITIONAL_VOLUMES+1]; //Mehdi
  vtkSmartPointer<vtkImageResample> resample[MAX_NUMBER_OF_ADDITIONAL_VOLUMES+1]; //Mehdi

  // transfer functions and properties 
  vtkSmartPointer<vtkColorTransferFunction>  colorFun   [MAX_NUMBER_OF_ADDITIONAL_VOLUMES+1];
  vtkSmartPointer<vtkPiecewiseFunction>      opacityFun [MAX_NUMBER_OF_ADDITIONAL_VOLUMES+1];
  vtkSmartPointer<vtkVolumeProperty>         property   [MAX_NUMBER_OF_ADDITIONAL_VOLUMES+1];
    
  // Create our volume and mapper
  vtkSmartPointer<vtkVolume> volume = vtkVolume::New();
  vtkSmartPointer<vtkOpenGLGPUMultiVolumeRayCastMapper> mapper = 
    vtkOpenGLGPUMultiVolumeRayCastMapper::New();
  mapper->setNumberOfAdditionalVolumes(numberOfAdditionalVolumes);//Mehdi
  
  volume->SetMapper( mapper );

  // Read the 2 datasets
  for(int vol=0;vol<numberOfAdditionalVolumes+1;vol++) { //Mehdi
    if(vol_info[vol].dirname)
      {
      vtkSmartPointer<vtkDICOMImageReader> dicomReader = 
        vtkDICOMImageReader::New();
      dicomReader->SetDirectoryName(vol_info[vol].dirname);
      dicomReader->Update();
      input[vol]  = dicomReader->GetOutput();
      reader[vol] = dicomReader;
      }
    else if ( vol_info[vol].fileType == VTI_FILETYPE )
      {
      vtkSmartPointer<vtkXMLImageDataReader> xmlReader = 
        vtkXMLImageDataReader::New();
      xmlReader->SetFileName(vol_info[vol].fileName);
      xmlReader->Update();
      input[vol]  = xmlReader->GetOutput();
      reader[vol] = xmlReader;
      }
    else if ( vol_info[vol].fileType == MHA_FILETYPE )
      {
      vtkSmartPointer<vtkMetaImageReader> metaReader = 
        vtkMetaImageReader::New();
      metaReader->SetFileName(vol_info[vol].fileName);
      metaReader->Update();
      input[vol]  = metaReader->GetOutput();
      reader[vol] = metaReader;
      }
    else if ( vol_info[vol].fileType == V16_FILETYPE )
      {
        std::cout << "v16" << std::endl;
      vtkSmartPointer<vtkVolume16Reader> v16 =
        vtkSmartPointer<vtkVolume16Reader>::New();
      v16->SetDataDimensions (64,64);
      v16->SetImageRange (1,93);
      v16->SetDataByteOrderToLittleEndian();
      v16->SetFilePrefix (vol_info[vol].fileName);
      v16->SetDataSpacing (3.2, 3.2, 1.5);
      v16->Update();
      input[vol]  = v16->GetOutput();
      reader[vol] = v16;
      }
    else
      {
      cout << "Error! Not VTI or MHA!" << endl;
      exit(EXIT_FAILURE);
      }

    // Verify that we actually have a volume
    int dim[3];
    input[vol]->GetDimensions(dim);
    if ( dim[0] < 2 ||
        dim[1] < 2 ||
        dim[2] < 2 )
      {
      cout << "Error loading data!" << endl;
      exit(EXIT_FAILURE);    
      }
    
    resample[vol] = vtkImageResample::New();
    if ( vol_info[vol].reductionFactor < 1.0 )
      {
      resample[vol]->SetInputConnection( reader[vol]->GetOutputPort() );
      resample[vol]->SetAxisMagnificationFactor(0, 
                                                vol_info[vol].reductionFactor);
      resample[vol]->SetAxisMagnificationFactor(1, 
                                                vol_info[vol].reductionFactor);
      resample[vol]->SetAxisMagnificationFactor(2, 
                                                vol_info[vol].reductionFactor);
      }

    // Set input to mapper
    if ( vol_info[vol].reductionFactor < 1.0 )
      {
      mapper->SetInputConnection( vol, resample[vol]->GetOutputPort() );
      }
    else
      {
      mapper->SetInputConnection( vol, reader[vol]->GetOutputPort() );
      }
    
/*    // Set the sample distance on the ray to be 1/2 the average spacing
    double spacing[3];
    if ( reductionFactor < 1.0 )
      {
      resample->GetOutput()->GetSpacing(spacing);
      }
    else
      {
      input->GetSpacing(spacing);
      }
    //  mapper->SetSampleDistance( (spacing[0]+spacing[1]+spacing[2])/6.0 );
    //  mapper->SetMaximumImageSampleDistance(10.0);*/

    // Create our transfer function
    colorFun  [vol] = vtkColorTransferFunction::New();
    opacityFun[vol] = vtkPiecewiseFunction    ::New();
    
    // Create the property and attach the transfer functions
    property[vol] = vtkVolumeProperty::New();
    property[vol]->SetIndependentComponents(independentComponents);
    property[vol]->SetColor( colorFun[vol] );
    property[vol]->SetScalarOpacity( opacityFun[vol] );
    property[vol]->SetInterpolationTypeToLinear();


    // Create transfer functions and change properties based on selected
    // options
    CreateTransferFunctions(vol, vol_info[vol], colorFun[vol], opacityFun[vol], 
                            property[vol], mapper);

    // connect up the volume to the property and the mapper
    // TODO
    if (vol==0)
      volume->SetProperty( property[vol] );
    else if (vol>0)
      //Mehdi mapper->SetProperty2( property[vol] );
	  mapper->SetAdditionalProperty(vol-1, property[vol] );//Mehdi

  }
  
  
  // Add a box widget if the clip option was selected
  vtkSmartPointer<vtkBoxWidget> box = vtkBoxWidget::New();
  if (clip)
    {
    box->SetInteractor(iren);
    box->SetPlaceFactor(1.01);
    if ( vol_info[0].reductionFactor < 1.0 )
      {      
      box->SetInput(resample[0]->GetOutput());
      }
    else
      {
      box->SetInput(input[0]);
      }
    
    box->SetDefaultRenderer(renderer);
    box->InsideOutOn();
    box->PlaceWidget();
    vtkSmartPointer<vtkBoxWidgetCallback> callback = 
      vtkBoxWidgetCallback::New();
    callback->SetMapper(mapper);
    box->AddObserver(vtkCommand::InteractionEvent, callback);
    box->EnabledOn();
    box->GetSelectedFaceProperty()->SetOpacity(0.0);
    }

  //--------------------------------------------------------------------------------------Mehdi
  // Add a transform box widget if the clip option was not selected
  vtkSmartPointer<vtkBoxWidget> Additionaltransform_box[MAX_NUMBER_OF_ADDITIONAL_VOLUMES];
  vtkSmartPointer<vtkTransformBoxWidgetCallback> Additionaltransform_callback[MAX_NUMBER_OF_ADDITIONAL_VOLUMES];
  for(int iii=0;iii<numberOfAdditionalVolumes;iii++)
  {
  Additionaltransform_box[iii]= vtkBoxWidget::New();
  Additionaltransform_box[iii]->SetInteractor(iren);
  Additionaltransform_box[iii]->SetPlaceFactor(1.0);
  if ( vol_info[iii+1].reductionFactor < 1.0 )
    {      
    Additionaltransform_box[iii]->SetInput(resample[iii+1]->GetOutput());
    }
  else
    {
    Additionaltransform_box[iii]->SetInput(input[iii+1]);
    }
  
  Additionaltransform_box[iii]->SetDefaultRenderer(renderer);
  Additionaltransform_box[iii]->InsideOutOn();
  Additionaltransform_box[iii]->PlaceWidget();
  Additionaltransform_callback[iii] = vtkTransformBoxWidgetCallback::New(iii);
  Additionaltransform_callback[iii]->SetMapper(mapper);
  Additionaltransform_box[iii]->AddObserver(vtkCommand::InteractionEvent, Additionaltransform_callback[iii]);
  Additionaltransform_box[iii]->EnabledOn();
  Additionaltransform_box[iii]->GetSelectedFaceProperty()->SetOpacity(0.0);
  }
  //--------------------------------------------------------------------------------------Mehdi
  
  // Set the default window size
  renWin->SetSize(600,600);
  renWin->Render();

  vtkSmartPointer<vtkInteractorStyleTrackballCamera> interact = 
      vtkInteractorStyleTrackballCamera::New();
  iren->SetInteractorStyle(interact);
  
  // Add the volume to the scene
  renderer->AddVolume( volume );
  renderer->ResetCamera();

  vtkSmartPointer<vtkCallbackCommand> callback4 = vtkSmartPointer<vtkCallbackCommand>::New();//Mehdi
 
  callback4->SetCallback(CallbackFunction);
  renderer->AddObserver(vtkCommand::EndEvent, callback4);

  // interact with data
  renWin->Render();
  iren->Start();

  return 0;
}
