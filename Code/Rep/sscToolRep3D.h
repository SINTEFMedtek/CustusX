#ifndef SSCTOOL3DREP_H_
#define SSCTOOL3DREP_H_

#include <boost/shared_ptr.hpp>
#include "vtkSmartPointer.h"
#include "sscRepImpl.h"
#include "sscTransform3D.h"
#include "sscGraphicalPrimitives.h"

typedef vtkSmartPointer<class vtkActor> vtkActorPtr;
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;
typedef vtkSmartPointer<class vtkSTLReader> vtkSTLReaderPtr;

namespace ssc
{
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class Tool> ToolPtr;

/**Display a 3D representation of a ssc::Tool.
 * The representation to use is extracted from the tool itself.		
 *
 * Used by Sonowand.
 * Used by CustusX.
 */
class ToolRep3D : public RepImpl
{
	Q_OBJECT
public:
	static ToolRep3DPtr New(const std::string& uid, const std::string& name="");
	virtual ~ToolRep3D();
	virtual std::string getType() const;

	virtual void setTool(ToolPtr tool);
	virtual bool hasTool(ToolPtr tool) const;
	virtual ToolPtr getTool();

  void setStayHiddenAfterVisible(bool val);
	void setStayVisibleAfterHide(bool val); ///< if true, tool is still rendered as visible after visibility status is hidden.
	void setOffsetPointVisibleAtZeroOffset(bool val); ///< if true the sphere is visible even when the offset is zero

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
	void tooltipOffsetSlot(double val); ///< receive the virtual tool tip extension.
private:
	void updateOffsetGraphics();

	ToolPtr mTool;
	vtkActorPtr mToolActor;
	vtkPolyDataMapperPtr mPolyDataMapper;
	vtkSTLReaderPtr mSTLReader;
	GraphicalPoint3DPtr mTooltipPoint;
	GraphicalPoint3DPtr mOffsetPoint;
	GraphicalLine3DPtr mOffsetLine;
  bool mStayHiddenAfterVisible;
	bool mStayVisibleAfterHide;
	bool mOffsetPointVisibleAtZeroOffset;
};
} // namespace ssc

#endif /*SSCTOOL3DREP_H_*/
