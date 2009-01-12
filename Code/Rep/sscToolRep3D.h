#ifndef SSCTOOL3DREP_H_
#define SSCTOOL3DREP_H_

#include <boost/shared_ptr.hpp>
#include "vtkSmartPointer.h"
#include "sscRepImpl.h"
#include "sscTransform3D.h"

typedef vtkSmartPointer<class vtkActor> vtkActorPtr;
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;
typedef vtkSmartPointer<class vtkSTLReader> vtkSTLReaderPtr;

namespace ssc
{
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class Tool> ToolPtr;

class ToolRep3D : public RepImpl
{
	Q_OBJECT
public:
	static ToolRep3DPtr New(const std::string& uid, const std::string& name="");
	virtual ~ToolRep3D();
	virtual std::string getType() const;

	virtual void setTool(ToolPtr tool);
	virtual bool hasTool(ToolPtr tool) const;
	
protected:
	ToolRep3D(const std::string& uid, const std::string& name="");
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

private slots:
	/**
	 * Receive transform and timestamp from a tool
	 *\param matrix		The tools transform
	 *\param timestamp	The time at which the transform was recorded
	 */
	void receiveTransforms(Transform3D matrix, double timestamp);
	/**
	 * Receive whether or not the tool is visible to the tracking system
	 *\param visible Whether or not the tool is visible to the tracking system.
	 */
	void receiveVisible(bool visible);
private:
	ToolPtr mTool;
	vtkActorPtr mToolActor;
	vtkPolyDataMapperPtr mPolyDataMapper;
	vtkSTLReaderPtr mSTLReader;
};
} // namespace ssc

#endif /*SSCTOOL3DREP_H_*/
