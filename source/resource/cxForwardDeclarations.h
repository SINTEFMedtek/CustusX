#ifndef CX_FORWARDDECLARARATIONS_H_
#define CX_FORWARDDECLARARATIONS_H_

#include <boost/shared_ptr.hpp>
#include <QPointer>
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

/**\file cxForwardDeclarations.h
 *
 * Include this file when the types and not the
 * full definitions of the ssc is needed.
 */

typedef boost::shared_ptr<class DataAdapter> DataAdapterPtr;

namespace ssc
{
typedef boost::shared_ptr<class ImageImportVideoSource> ImageImportVideoSourcePtr;

typedef boost::shared_ptr<class ProcessedUSInputData> ProcessedUSInputDataPtr;
typedef boost::shared_ptr<class USFrameData> USFrameDataPtr;

typedef boost::shared_ptr<class StringDataAdapter> StringDataAdapterPtr;
typedef boost::shared_ptr<class DoubleDataAdapter> DoubleDataAdapterPtr;
typedef boost::shared_ptr<class BoolDataAdapter> BoolDataAdapterPtr;
typedef boost::shared_ptr<class ColorDataAdapter> ColorDataAdapterPtr;

typedef boost::shared_ptr<class StringDataAdapterXml> StringDataAdapterXmlPtr;
typedef boost::shared_ptr<class DoubleDataAdapterXml> DoubleDataAdapterXmlPtr;
typedef boost::shared_ptr<class BoolDataAdapterXml> BoolDataAdapterXmlPtr;
typedef boost::shared_ptr<class ColorDataAdapterXml> ColorDataAdapterXmlPtr;

typedef boost::shared_ptr<class ViewportListener> ViewportListenerPtr;
}

namespace cx
{

typedef boost::shared_ptr<class LandmarkRep> LandmarkRepPtr;
typedef boost::shared_ptr<class ViewGroupData> ViewGroupDataPtr;

class View2D;
class View3D;
typedef boost::shared_ptr<class ViewGroup> ViewGroupPtr;
typedef boost::shared_ptr<class ViewGroup2D> ViewGroup2DPtr;
typedef boost::shared_ptr<class ViewGroup3D> ViewGroup3DPtr;
typedef boost::shared_ptr<class ViewWrapper> ViewWrapperPtr;
typedef boost::shared_ptr<class PatientData> PatientDataPtr;
typedef boost::shared_ptr<class Tool> ToolPtr;
typedef boost::shared_ptr<class VideoConnection> VideoConnectionPtr;
typedef QPointer<View3D> View3DQPtr;
typedef QPointer<ssc::ViewWidget> ViewWidgetQPtr;

typedef boost::shared_ptr<class UsReconstructionFileReader> UsReconstructionFileReaderPtr;
typedef boost::shared_ptr<class ImageLandmarksSource> ImageLandmarksSourcePtr;


} // namespace cx

#endif /*CX_FORWARDDECLARARATIONS_H_*/
