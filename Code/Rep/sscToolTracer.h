#ifndef SSCTOOLTRACKER_H_
#define SSCTOOLTRACKER_H_

#include <QObject>
#include <QSize>
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
#include "sscTransform3D.h"

#include <boost/shared_ptr.hpp>
//#include "sscProbeSector.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

class QColor;

namespace ssc
{

typedef boost::shared_ptr<class ToolTracer> ToolTracerPtr;

/**Graphics class for displaying the trace path traversed by a tool.
 */
class ToolTracer : QObject
{
  Q_OBJECT
public: 
public:
  ToolTracer();
  void setTool(ToolPtr tool);
  vtkPolyDataPtr getPolyData();
  vtkActorPtr getActor();

  void setColor(QColor color);

//  void start();
//  void stop();
//  void clear();

private slots:
  void receiveTransforms(Transform3D prMt, double timestamp);
private:
  vtkPolyDataPtr mPolyData; ///< polydata representation of the probe, in space u
  vtkActorPtr mActor;
  ToolPtr mTool;
  vtkPolyDataMapperPtr mPolyDataMapper;
  vtkPropertyPtr mProperty;

  vtkPointsPtr mPoints;
  vtkCellArrayPtr mLines;
};

} // namespace ssc

#endif /*SSCTOOLTRACKER_H_*/
