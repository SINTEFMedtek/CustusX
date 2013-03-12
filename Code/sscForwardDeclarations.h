#ifndef SSC_FORWARDDECLARARATIONS_H_
#define SSC_FORWARDDECLARARATIONS_H_

#include <boost/shared_ptr.hpp>
#include <map>


/**\file sscForwardDeclarations.h
 *
 * Include this file when the types and not the
 * full definitions of the ssc is needed.
 */

namespace ssc {

class View;
class ViewContainer;
class ViewItem;
class ViewWidget;

// KEEP SORTED AND UNIQUE!

// data
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef boost::shared_ptr<class ManualTool> ManualToolPtr;
typedef boost::shared_ptr<class DummyTool> DummyToolPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class NavigatedVideoImage> NavigatedVideoImagePtr;
typedef boost::shared_ptr<class Data> DataPtr;
typedef boost::shared_ptr<class Mesh> MeshPtr;
typedef boost::shared_ptr<class ImageTF3D> ImageTF3DPtr;
typedef boost::shared_ptr<class ImageLUT2D> ImageLUT2DPtr;
typedef boost::shared_ptr<class ImageTFData> ImageTFDataPtr;
typedef boost::shared_ptr<class GPUImageDataBuffer> GPUImageDataBufferPtr;
typedef boost::weak_ptr<class GPUImageDataBuffer> GPUImageDataBufferWeakPtr;
typedef boost::shared_ptr<class GPUImageLutBuffer> GPUImageLutBufferPtr;
typedef boost::weak_ptr<class GPUImageLutBuffer> GPUImageLutBufferWeakPtr;
typedef boost::shared_ptr<class ProbeSector> ProbeSectorPtr;
typedef boost::shared_ptr<class FiberBundle> FiberBundlePtr;

// reps
typedef boost::shared_ptr<class Axes3D> Axes3DPtr;
typedef boost::shared_ptr<class AxesRep> AxesRepPtr;
typedef boost::shared_ptr<class CrossHair2D> CrossHair2DPtr;
typedef boost::shared_ptr<class CrossHairRep2D> CrossHairRep2DPtr;
typedef boost::shared_ptr<class DisplayTextRep> DisplayTextRepPtr;
typedef boost::shared_ptr<class FiberBundleRep> FiberBundleRepPtr;
typedef boost::shared_ptr<class GeometricRep2D> GeometricRep2DPtr;
typedef boost::shared_ptr<class GeometricRep> GeometricRepPtr;
typedef boost::shared_ptr<class GPUImageDataBuffer> GPUImageDataBufferPtr;
typedef boost::shared_ptr<class GPUImageLutBuffer> GPUImageLutBufferPtr;
typedef boost::shared_ptr<class GPURayCastVolumeRep> GPURayCastVolumeRepPtr;
typedef boost::shared_ptr<class GraphicalLine3D> GraphicalLine3DPtr;
typedef boost::shared_ptr<class GraphicalPoint3D> GraphicalPoint3DPtr;
typedef boost::shared_ptr<class GuideRep2D> GuideRep2DPtr;
typedef boost::shared_ptr<class ImageLUT2D> ImageLUT2DPtr;
typedef boost::shared_ptr<class ImageMapperMonitor> ImageMapperMonitorPtr;
typedef boost::shared_ptr<class ImageTF3D> ImageTF3DPtr;
typedef boost::shared_ptr<class LineSegment> LineSegmentPtr;
typedef boost::shared_ptr<class Mesh> MeshPtr;
typedef boost::shared_ptr<class OffsetPoint> OffsetPointPtr;
typedef boost::shared_ptr<class OrientationAnnotation3DRep> OrientationAnnotation3DRepPtr;
typedef boost::shared_ptr<class OrientationAnnotationRep> OrientationAnnotationRepPtr;
typedef boost::shared_ptr<class PickerRep> PickerRepPtr;
typedef boost::shared_ptr<class PointMetric> PointMetricPtr;
typedef boost::shared_ptr<class PointMetricRep> PointMetricRepPtr;
typedef boost::shared_ptr<class PointMetricRep2D> PointMetricRep2DPtr;
typedef boost::shared_ptr<class ProgressiveLODVolumetricRep> ProgressiveLODVolumetricRepPtr;
typedef boost::shared_ptr<class Rect3D> Rect3DPtr;
typedef boost::shared_ptr<class Rep> RepPtr;
typedef boost::shared_ptr<class SlicedImageProxy> SlicedImageProxyPtr;
typedef boost::shared_ptr<class SlicePlaneClipper> SlicePlaneClipperPtr;
typedef boost::shared_ptr<class SlicePlaneRep> SlicePlaneRepPtr;
typedef boost::shared_ptr<class SlicePlaneRep> SlicePlaneRepPtr;
typedef boost::shared_ptr<class SlicePlanes3DMarkerIn2DRep> SlicePlanes3DMarkerIn2DRepPtr;
typedef boost::shared_ptr<class SlicePlanes3DRep> SlicePlanes3DRepPtr;
typedef boost::shared_ptr<class SlicePlanesProxy> SlicePlanesProxyPtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef boost::shared_ptr<class SliceRepSW> SliceRepSWPtr;
typedef boost::shared_ptr<class SurfaceRep> SurfaceRepPtr;
typedef boost::shared_ptr<class TestVideoSource> TestVideoSourcePtr;
typedef boost::shared_ptr<class TextDisplay> TextDisplayPtr;
typedef boost::shared_ptr<class Texture3DSlicerRep> Texture3DSlicerRepPtr;
typedef boost::shared_ptr<class ToolRep2D> ToolRep2DPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class ToolTracer> ToolTracerPtr;
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef boost::shared_ptr<class VideoFixedPlaneRep> VideoFixedPlaneRepPtr;
typedef boost::shared_ptr<class VideoSource> VideoSourcePtr;
typedef boost::shared_ptr<class View> ViewPtr;
typedef boost::shared_ptr<class VolumetricBaseRep> VolumetricBaseRepPtr;
typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;
typedef boost::shared_ptr<class VtkFileMesh> VtkFileMeshPtr;

class LabeledComboBoxWidget;

} // namespace ssc

#endif /*SSC_FORWARDDECLARARATIONS_H_*/
