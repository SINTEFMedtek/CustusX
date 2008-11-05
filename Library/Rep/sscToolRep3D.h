#ifndef SSCTOOL3DREP_H_
#define SSCTOOL3DREP_H_

#include <boost/shared_ptr.hpp>
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkActor> vtkActorPtr;
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;
#include "sscRepImpl.h"
#include "sscTool.h"

namespace ssc
{
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;

class ToolRep3D : public RepImpl
{
	Q_OBJECT
public:
	ToolRep3D(const std::string& uid, const std::string& name, ToolPtr tool);
	virtual ~ToolRep3D();
	virtual std::string getType() const;

protected:
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

private slots:
	void receiveTransforms(Tool::TransformAndTimestampEventArgumentPtr payload);
	void receiveVisible(Tool::VisibleEventArgumentPtr visible);

private:
	ToolPtr mTool;
	vtkActorPtr mToolActor;
	vtkPolyDataMapperPtr mPolyDataMapper;
};

} // namespace ssc

#endif /*SSCTOOL3DREP_H_*/
