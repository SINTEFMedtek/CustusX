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

namespace ssc
{
typedef boost::shared_ptr<class ImageImportVideoSource> ImageImportVideoSourcePtr;

typedef boost::shared_ptr<class ProcessedUSInputData> ProcessedUSInputDataPtr;
typedef boost::shared_ptr<class USFrameData> USFrameDataPtr;
}

namespace cx
{

typedef boost::shared_ptr<class LandmarkRep> LandmarkRepPtr;

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


} // namespace cx

#endif /*CX_FORWARDDECLARARATIONS_H_*/
