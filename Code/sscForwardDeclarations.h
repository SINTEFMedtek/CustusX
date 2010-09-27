#ifndef SSC_FORWARDDECLARARATIONS_H_
#define SSC_FORWARDDECLARARATIONS_H_

#include <boost/shared_ptr.hpp>

/**\file sscForwardDeclarations.h
 *
 * Include this file when the types and not the
 * full definitions of the ssc is needed.
 */

namespace ssc {

class View;

// data
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef boost::shared_ptr<class ManualTool> ManualToolPtr;
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class Data> DataPtr;
typedef boost::shared_ptr<class Mesh> MeshPtr;
typedef boost::shared_ptr<class ImageTF3D> ImageTF3DPtr;
typedef boost::shared_ptr<class ImageLUT2D> ImageLUT2DPtr;
typedef boost::shared_ptr<class GPUImageDataBuffer> GPUImageDataBufferPtr;
typedef boost::weak_ptr<class GPUImageDataBuffer> GPUImageDataBufferWeakPtr;
typedef boost::shared_ptr<class GPUImageLutBuffer> GPUImageLutBufferPtr;
typedef boost::weak_ptr<class GPUImageLutBuffer> GPUImageLutBufferWeakPtr;

// reps
typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;
typedef boost::shared_ptr<class GeometricRep> GeometricRepPtr;
typedef boost::shared_ptr<class ProgressiveLODVolumetricRep> ProgressiveLODVolumetricRepPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class ToolRep2D> ToolRep2DPtr;
typedef boost::shared_ptr<class SliceRepSW> SliceRepSWPtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef boost::shared_ptr<class SlicePlaneRep> SlicePlaneRepPtr;
typedef boost::shared_ptr<class ProbeRep> ProbeRepPtr;
typedef boost::shared_ptr<class OrientationAnnotationRep> OrientationAnnotationRepPtr;
typedef boost::shared_ptr<class DisplayTextRep> DisplayTextRepPtr;
typedef boost::shared_ptr<class AxesRep> AxesRepPtr;
typedef boost::shared_ptr<class SlicePlanes3DMarkerIn2DRep> SlicePlanes3DMarkerIn2DRepPtr;
typedef boost::shared_ptr<class SlicePlanesProxy> SlicePlanesProxyPtr;
typedef boost::shared_ptr<class SlicePlanes3DRep> SlicePlanes3DRepPtr;
typedef boost::shared_ptr<class SlicePlaneClipper> SlicePlaneClipperPtr;

typedef boost::shared_ptr<class ImageMapperMonitor> ImageMapperMonitorPtr;

} // namespace ssc

#endif /*SSC_FORWARDDECLARARATIONS_H_*/
