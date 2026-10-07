/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CX_FORWARDDECLARARATIONS_H_
#define CX_FORWARDDECLARARATIONS_H_

#include "cxPrecompiledHeader.h"

#include <vector>
#include <map>

#include <memory>
#include <QPointer>
#include "vtkForwardDeclarations.h"

/**\file cxForwardDeclarations.h
 *
 * Include this file when the types and not the
 * full definitions of the ssc is needed.
 */

namespace cx
{

typedef std::shared_ptr<class Property> PropertyPtr;

class View;
class ViewItem;

// KEEP SORTED AND UNIQUE!

// data
typedef std::shared_ptr<class ActiveData> ActiveDataPtr;
typedef std::shared_ptr<class Tool> ToolPtr;
typedef std::shared_ptr<class ManualTool> ManualToolPtr;
typedef std::shared_ptr<class DummyTool> DummyToolPtr;
typedef std::shared_ptr<class Image> ImagePtr;
typedef std::shared_ptr<class NavigatedVideoImage> NavigatedVideoImagePtr;
typedef std::shared_ptr<class Data> DataPtr;
typedef std::shared_ptr<class Mesh> MeshPtr;
typedef std::shared_ptr<class TrackedStream> TrackedStreamPtr;
typedef std::shared_ptr<class ImageTF3D> ImageTF3DPtr;
typedef std::shared_ptr<class ImageLUT2D> ImageLUT2DPtr;
typedef std::shared_ptr<class ImageTFData> ImageTFDataPtr;
typedef std::shared_ptr<class GPUImageDataBuffer> GPUImageDataBufferPtr;
typedef std::weak_ptr<class GPUImageDataBuffer> GPUImageDataBufferWeakPtr;
typedef std::shared_ptr<class GPUImageLutBuffer> GPUImageLutBufferPtr;
typedef std::weak_ptr<class GPUImageLutBuffer> GPUImageLutBufferWeakPtr;
typedef std::shared_ptr<class ProbeSector> ProbeSectorPtr;
typedef std::shared_ptr<class FiberBundle> FiberBundlePtr;
typedef std::shared_ptr<class USFrameData> USFrameDataPtr;

// reps
typedef std::shared_ptr<class Axes3D> Axes3DPtr;
typedef std::shared_ptr<class AxesRep> AxesRepPtr;
typedef std::shared_ptr<class CrossHair2D> CrossHair2DPtr;
typedef std::shared_ptr<class CrossHairRep2D> CrossHairRep2DPtr;
typedef std::shared_ptr<class DisplayTextRep> DisplayTextRepPtr;
typedef std::shared_ptr<class DistanceMetric> DistanceMetricPtr;
typedef std::shared_ptr<class FiberBundleRep> FiberBundleRepPtr;
typedef std::shared_ptr<class GeometricRep2D> GeometricRep2DPtr;
typedef std::shared_ptr<class GeometricRep> GeometricRepPtr;
typedef std::shared_ptr<class GPUImageDataBuffer> GPUImageDataBufferPtr;
typedef std::shared_ptr<class GPUImageLutBuffer> GPUImageLutBufferPtr;
typedef std::shared_ptr<class GraphicalLine3D> GraphicalLine3DPtr;
typedef std::shared_ptr<class GraphicalPoint3D> GraphicalPoint3DPtr;
typedef std::shared_ptr<class GuideRep2D> GuideRep2DPtr;
typedef std::shared_ptr<class ImageLUT2D> ImageLUT2DPtr;
typedef std::shared_ptr<class ImageTF3D> ImageTF3DPtr;
typedef std::shared_ptr<class LandmarkRep> LandmarkRepPtr;
typedef std::shared_ptr<class LineSegment> LineSegmentPtr;
typedef std::shared_ptr<class OffsetPoint> OffsetPointPtr;
typedef std::shared_ptr<class OrientationAnnotation3DRep> OrientationAnnotation3DRepPtr;
typedef std::shared_ptr<class OrientationAnnotationRep> OrientationAnnotationRepPtr;
typedef std::shared_ptr<class PickerRep> PickerRepPtr;
typedef std::shared_ptr<class PointMetric> PointMetricPtr;
typedef std::shared_ptr<class PointMetricRep> PointMetricRepPtr;
typedef std::shared_ptr<class PointMetricRep2D> PointMetricRep2DPtr;
typedef std::shared_ptr<class Rect3D> Rect3DPtr;
typedef std::shared_ptr<class Rep> RepPtr;
typedef std::shared_ptr<class SlicedImageProxy> SlicedImageProxyPtr;
typedef std::shared_ptr<class SlicePlaneClipper> SlicePlaneClipperPtr;
typedef std::shared_ptr<class SlicePlaneRep> SlicePlaneRepPtr;
typedef std::shared_ptr<class SlicePlaneRep> SlicePlaneRepPtr;
typedef std::shared_ptr<class SlicePlanes3DMarkerIn2DRep> SlicePlanes3DMarkerIn2DRepPtr;
typedef std::shared_ptr<class SlicePlanes3DRep> SlicePlanes3DRepPtr;
typedef std::shared_ptr<class SlicePlanesProxy> SlicePlanesProxyPtr;
typedef std::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef std::shared_ptr<class SliceRepSW> SliceRepSWPtr;
typedef std::shared_ptr<class Stream2DRep3D> Stream2DRep3DPtr;
typedef std::shared_ptr<class StreamRep3D> StreamRep3DPtr;
typedef std::shared_ptr<class SurfaceRep> SurfaceRepPtr;
typedef std::shared_ptr<class TestVideoSource> TestVideoSourcePtr;
typedef std::shared_ptr<class TextDisplay> TextDisplayPtr;
typedef std::shared_ptr<class Texture3DSlicerRep> Texture3DSlicerRepPtr;
typedef std::shared_ptr<class ToolRep2D> ToolRep2DPtr;
typedef std::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef std::shared_ptr<class ToolTracer> ToolTracerPtr;
typedef std::shared_ptr<class Tool> ToolPtr;
typedef std::shared_ptr<class VideoFixedPlaneRep> VideoFixedPlaneRepPtr;
typedef std::shared_ptr<class VideoSource> VideoSourcePtr;
typedef std::shared_ptr<class View> ViewPtr;
typedef std::shared_ptr<class VolumetricBaseRep> VolumetricBaseRepPtr;
typedef std::shared_ptr<class VolumetricRep> VolumetricRepPtr;

// Services
typedef std::shared_ptr<class CoreServices> CoreServicesPtr;
typedef std::shared_ptr<class RegServices> RegServicesPtr;
typedef std::shared_ptr<class VisServices> VisServicesPtr;
typedef std::shared_ptr<class FileReaderWriterService> FileReaderWriterServicePtr;
typedef std::shared_ptr<class FileManagerService> FileManagerServicePtr;
typedef std::shared_ptr<class AcquisitionService> AcquisitionServicePtr;
typedef std::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef std::shared_ptr<class RegistrationService> RegistrationServicePtr;
typedef std::shared_ptr<class SessionStorageService> SessionStorageServicePtr;
typedef std::shared_ptr<class SpaceProvider> SpaceProviderPtr;
typedef std::shared_ptr<class StateService> StateServicePtr;
typedef std::shared_ptr<class TrackingService> TrackingServicePtr;
typedef std::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;
typedef std::shared_ptr<class VideoService> VideoServicePtr;
typedef std::shared_ptr<class ViewService> ViewServicePtr;

typedef std::weak_ptr<class SpaceProvider> SpaceProviderWeakPtr;
typedef std::weak_ptr<class StateService> StateServiceWeakPtr;
typedef std::weak_ptr<class ViewManager> ViewServiceWeakPtr;

// data adapters
typedef std::shared_ptr<class StringPropertyBase> StringPropertyBasePtr;
typedef std::shared_ptr<class DoublePropertyBase> DoublePropertyBasePtr;
typedef std::shared_ptr<class BoolPropertyBase> BoolPropertyBasePtr;
typedef std::shared_ptr<class ColorPropertyBase> ColorPropertyBasePtr;

typedef std::shared_ptr<class StringProperty> StringPropertyPtr;
typedef std::shared_ptr<class DoubleProperty> DoublePropertyPtr;
typedef std::shared_ptr<class BoolProperty> BoolPropertyPtr;
typedef std::shared_ptr<class ColorProperty> ColorPropertyPtr;
typedef std::shared_ptr<class DoublePairProperty> DoublePairPropertyPtr;
typedef std::shared_ptr<class FilePathProperty> FilePathPropertyPtr;
typedef std::shared_ptr<class FilePreviewProperty> FilePreviewPropertyPtr;
typedef std::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;

// other stuff
typedef std::shared_ptr<class Branch> BranchPtr;
typedef std::shared_ptr<class BranchList> BranchListPtr;
typedef std::shared_ptr<class CameraControl> CameraControlPtr;
typedef std::shared_ptr<class Clippers> ClippersPtr;
typedef std::shared_ptr<class CyclicActionLogger> CyclicActionLoggerPtr;
typedef std::shared_ptr<class Filter> FilterPtr;
typedef std::shared_ptr<class ImageLandmarksSource> ImageLandmarksSourcePtr;
typedef std::shared_ptr<class InteractiveClipper> InteractiveClipperPtr;
typedef std::shared_ptr<class InteractiveCropper> InteractiveCropperPtr;
typedef std::shared_ptr<class LayoutRepository> LayoutRepositoryPtr;
typedef std::shared_ptr<class Navigation> NavigationPtr;
typedef std::shared_ptr<class Presets> PresetsPtr;
typedef std::shared_ptr<class ProcessedUSInputData> ProcessedUSInputDataPtr;
typedef std::shared_ptr<class RepContainer> RepContainerPtr;
typedef std::shared_ptr<class UsReconstructionFileReader> UsReconstructionFileReaderPtr;
typedef std::shared_ptr<class ViewGroupData> ViewGroupDataPtr;
typedef std::shared_ptr<class ViewGroup> ViewGroupPtr;
typedef std::shared_ptr<class ViewGroup2D> ViewGroup2DPtr;
typedef std::shared_ptr<class ViewGroup3D> ViewGroup3DPtr;
typedef std::shared_ptr<class ViewportListener> ViewportListenerPtr;
typedef std::shared_ptr<class ViewWrapper> ViewWrapperPtr;
typedef std::shared_ptr<class VideoConnectionManager> VideoConnectionManagerPtr;
typedef std::shared_ptr<class WorkflowStateMachine> WorkflowStateMachinePtr;

} // namespace cx

#endif /*CX_FORWARDDECLARARATIONS_H_*/
