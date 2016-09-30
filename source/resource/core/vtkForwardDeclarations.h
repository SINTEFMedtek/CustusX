/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#ifndef VTKFORWARDDECLARATIONS_H_
#define VTKFORWARDDECLARATIONS_H_

#include <vtkSmartPointer.h>
#include "cxConfig.h"

/**
 * \brief File for vtk forwarddeclarations
 *
 * \date 26. okt. 2010
 * \author: Janne Beate Bakeng, SINTEF
 */

// KEEP SORTED AND UNIQUE!

typedef vtkSmartPointer<class SNWXOpenGLRenderWindow> SNWXOpenGLRenderWindowPtr;
typedef vtkSmartPointer<class vtkAbstractTransform> vtkAbstractTransformPtr;
typedef vtkSmartPointer<class vtkActor2D> vtkActor2DPtr;
typedef vtkSmartPointer<class vtkActor> vtkActorPtr;
typedef vtkSmartPointer<class vtkAppendPolyData> vtkAppendPolyDataPtr;
typedef vtkSmartPointer<class vtkAssembly> vtkAssemblyPtr;
typedef vtkSmartPointer<class vtkAxesActor> vtkAxesActorPtr;
typedef vtkSmartPointer<class vtkBoxRepresentation> vtkBoxRepresentationPtr;
typedef vtkSmartPointer<class vtkBoxWidget2> vtkBoxWidget2Ptr;
typedef vtkSmartPointer<class vtkBoxWidget> vtkBoxWidgetPtr;
typedef vtkSmartPointer<class vtkCallbackCommand> vtkCallbackCommandPtr;
typedef vtkSmartPointer<class vtkCamera> vtkCameraPtr;
typedef vtkSmartPointer<class vtkCaptionActor2D> vtkCaptionActor2DPtr;
typedef vtkSmartPointer<class vtkCellArray> vtkCellArrayPtr;
typedef vtkSmartPointer<class vtkCellLocator> vtkCellLocatorPtr;
typedef vtkSmartPointer<class vtkClipPolyData> vtkClipPolyDataPtr;
typedef vtkSmartPointer<class vtkColorTransferFunction> vtkColorTransferFunctionPtr;
typedef vtkSmartPointer<class vtkConeSource> vtkConeSourcePtr;
typedef vtkSmartPointer<class vtkContourFilter> vtkContourFilterPtr;
typedef vtkSmartPointer<class vtkCursor2D> vtkCursor2DPtr;
typedef vtkSmartPointer<class vtkCursor3D> vtkCursor3DPtr;
typedef vtkSmartPointer<class vtkCutter> vtkCutterPtr;
typedef vtkSmartPointer<class vtkCylinderSource> vtkCylinderSourcePtr;
typedef vtkSmartPointer<class vtkDataArray> vtkDataArrayPtr;
typedef vtkSmartPointer<class vtkDataSetAttributes> vtkDataSetAttributesPtr;
typedef vtkSmartPointer<class vtkDataSetAlgorithm> vtkDataSetAlgorithmPtr;
typedef vtkSmartPointer<class vtkDecimatePro> vtkDecimateProPtr;
typedef vtkSmartPointer<class vtkDoubleArray> vtkDoubleArrayPtr;
typedef vtkSmartPointer<class vtkEventQtSlotConnect> vtkEventQtSlotConnectPtr;
typedef vtkSmartPointer<class vtkFloatArray> vtkFloatArrayPtr;
typedef vtkSmartPointer<class vtkFollower> vtkFollowerPtr;
typedef vtkSmartPointer<class vtkGeneralTransform> vtkGeneralTransformPtr;
typedef vtkSmartPointer<class vtkGLSLShaderDeviceAdapter2 > vtkGLSLShaderDeviceAdapter2Ptr;
typedef vtkSmartPointer<class vtkGlyph3DMapper> vtkGlyph3DMapperPtr;
typedef vtkSmartPointer<class vtkIdList> vtkIdListPtr;
typedef vtkSmartPointer<class vtkImageAccumulate> vtkImageAccumulatePtr;
typedef vtkSmartPointer<class vtkImageActor> vtkImageActorPtr;
typedef vtkSmartPointer<class vtkImageAlgorithm> vtkImageAlgorithmPtr;
typedef vtkSmartPointer<class vtkImageAppendComponents > vtkImageAppendComponentsPtr;
typedef vtkSmartPointer<class vtkImageBlend> vtkImageBlendPtr;
typedef vtkSmartPointer<class vtkImageCast> vtkImageCastPtr;
typedef vtkSmartPointer<class vtkImageShiftScale> vtkImageShiftScalePtr;
typedef vtkSmartPointer<class vtkImageChangeInformation> vtkImageChangeInformationPtr;
typedef vtkSmartPointer<class vtkImageClip> vtkImageClipPtr;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageExport> vtkImageExportPtr;
typedef vtkSmartPointer<class vtkImageExtractComponents> vtkImageExtractComponentsPtr;
typedef vtkSmartPointer<class vtkImageMapToColors> vtkImageMapToColorsPtr;
typedef vtkSmartPointer<class vtkImageMapToWindowLevelColors> vtkImageMapToWindowLevelColorsPtr;
typedef vtkSmartPointer<class vtkImagePlaneWidget> vtkImagePlaneWidgetPtr;
typedef vtkSmartPointer<class vtkImageResample> vtkImageResamplePtr;
typedef vtkSmartPointer<class vtkImageReslice> vtkImageReslicePtr;
typedef vtkSmartPointer<class vtkImageShrink3D> vtkImageShrink3DPtr;
typedef vtkSmartPointer<class vtkImageThreshold> vtkImageThresholdPtr;
typedef vtkSmartPointer<class vtkInteractorStyleFlight> vtkInteractorStyleFlightPtr;
typedef vtkSmartPointer<class vtkInteractorStyleTrackballCamera> vtkInteractorStyleTrackballCameraPtr;
typedef vtkSmartPointer<class vtkInteractorStyleUnicam> vtkInteractorStyleUnicamPtr;
typedef vtkSmartPointer<class vtkLandmarkTransform> vtkLandmarkTransformPtr;
typedef vtkSmartPointer<class vtkLeaderActor2D> vtkLeaderActor2DPtr;
typedef vtkSmartPointer<class vtkLineSource> vtkLineSourcePtr;
typedef vtkSmartPointer<class vtkLookupTable> vtkLookupTablePtr;
typedef vtkSmartPointer<class vtkMapper> vtkMapperPtr;
typedef vtkSmartPointer<class vtkMarchingCubes> vtkMarchingCubesPtr;
typedef vtkSmartPointer<class vtkMaskPoints> vtkMaskPointsPtr;
typedef vtkSmartPointer<class vtkMath> vtkMathPtr;
typedef vtkSmartPointer<class vtkMatrix4x4> vtkMatrix4x4Ptr;
typedef vtkSmartPointer<class vtkMatrixToLinearTransform> vtkMatrixToLinearTransformPtr;
typedef vtkSmartPointer<class vtkMetaImageReader> vtkMetaImageReaderPtr;
typedef vtkSmartPointer<class vtkMetaImageWriter> vtkMetaImageWriterPtr;
typedef vtkSmartPointer<class vtkMINCImageReader> vtkMINCImageReaderPtr;
typedef vtkSmartPointer<class vtkOpenGLHardwareSupport > vtkOpenGLHardwareSupportPtr;
typedef vtkSmartPointer<class vtkPainterPolyDataMapper> vtkPainterPolyDataMapperPtr;
typedef vtkSmartPointer<class vtkPiecewiseFunction> vtkPiecewiseFunctionPtr;
typedef vtkSmartPointer<class vtkPlaneSource> vtkPlaneSourcePtr;
typedef vtkSmartPointer<class vtkPlanes> vtkPlanesPtr;
typedef vtkSmartPointer<class vtkPlane> vtkPlanePtr;
typedef vtkSmartPointer<class vtkPoints> vtkPointsPtr;
typedef vtkSmartPointer<class vtkPolyDataMapper2D> vtkPolyDataMapper2DPtr;
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;
typedef vtkSmartPointer<class vtkPolyDataNormals> vtkPolyDataNormalsPtr;
typedef vtkSmartPointer<class vtkPolyDataReader> vtkPolyDataReaderPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
typedef vtkSmartPointer<class vtkPolyDataWriter> vtkPolyDataWriterPtr;
typedef vtkSmartPointer<class vtkProbeFilter> vtkProbeFilterPtr;
typedef vtkSmartPointer<class vtkProp3D> vtkProp3DPtr;
typedef vtkSmartPointer<class vtkProperty> vtkPropertyPtr;
typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkRenderWindowInteractor> vtkRenderWindowInteractorPtr;
typedef vtkSmartPointer<class vtkRenderWindow> vtkRenderWindowPtr;
typedef vtkSmartPointer<class vtkScalarsToColors> vtkScalarsToColorsPtr;
typedef vtkSmartPointer<class vtkSectorSource> vtkSectorSourcePtr;
typedef vtkSmartPointer<class vtkShader2 > vtkShader2Ptr;
typedef vtkSmartPointer<class vtkShaderProgram2 > vtkShaderProgram2Ptr;
typedef vtkSmartPointer<class vtkSortDataArray> vtkSortDataArrayPtr;
typedef vtkSmartPointer<class vtkSphereSource> vtkSphereSourcePtr;
typedef vtkSmartPointer<class vtkSTLReader> vtkSTLReaderPtr;
typedef vtkSmartPointer<class vtkStripper> vtkStripperPtr;
typedef vtkSmartPointer<class vtkTextActor3D> vtkTextActor3DPtr;
typedef vtkSmartPointer<class vtkTextMapper> vtkTextMapperPtr;
typedef vtkSmartPointer<class vtkTextureMapToCylinder> vtkTextureMapToCylinderPtr;
typedef vtkSmartPointer<class vtkTextureMapToPlane> vtkTextureMapToPlanePtr;
typedef vtkSmartPointer<class vtkTextureMapToSphere> vtkTextureMapToSpherePtr;
typedef vtkSmartPointer<class vtkTexture> vtkTexturePtr;
typedef vtkSmartPointer<class vtkThinPlateSplineTransform> vtkThinPlateSplineTransformPtr;
typedef vtkSmartPointer<class vtkTransform> vtkTransformPtr;
typedef vtkSmartPointer<class vtkTransformPolyDataFilter> vtkTransformPolyDataFilterPtr;
typedef vtkSmartPointer<class vtkTransformTextureCoords> vtkTransformTextureCoordsPtr;
typedef vtkSmartPointer<class vtkTriangleFilter> vtkTriangleFilterPtr;
typedef vtkSmartPointer<class vtkTriangleFilter> vtkTriangleFilterPtr;
typedef vtkSmartPointer<class vtkUnsignedCharArray> vtkUnsignedCharArrayPtr;
typedef vtkSmartPointer<class vtkUnsignedShortArray> vtkUnsignedShortArrayPtr;
typedef vtkSmartPointer<class vtkVectorText> vtkVectorTextPtr;
typedef vtkSmartPointer<class vtkVolumeMapper> vtkVolumeMapperPtr;
typedef vtkSmartPointer<class vtkVolumeProperty> vtkVolumePropertyPtr;
typedef vtkSmartPointer<class vtkVolume> vtkVolumePtr;
typedef vtkSmartPointer<class vtkWindowedSincPolyDataFilter> vtkWindowedSincPolyDataFilterPtr;
typedef vtkSmartPointer<class vtkWindowLevelLookupTable> vtkWindowLevelLookupTablePtr;
#ifndef CX_VTK_OPENGL2
typedef vtkSmartPointer<class vtkVolumeTextureMapper3D> vtkVolumeTextureMapper3DPtr;
#else
typedef vtkSmartPointer<class vtkSmartVolumeMapper> vtkVolumeTextureMapper3DPtr;
#endif

typedef std::pair<vtkVectorTextPtr,vtkFollowerPtr> vtkVectorTextFollowerPair;

class vtkObject;
class vtkOpenGLExtensionManager;
class vtkRenderWindowInteractor;
class vtkTextProperty;
class vtkViewport;


#endif /* VTKFORWARDDECLARATIONS_H_ */
