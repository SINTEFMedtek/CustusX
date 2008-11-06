#include "sscToolRep3D.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>

#include "sscTool.h"
#include "sscView.h"

namespace ssc
{

ToolRep3D::ToolRep3D(const std::string& uid, const std::string& name, ToolPtr tool) :
	RepImpl(uid, name)
{
	mTool = tool;
	mToolActor = vtkActorPtr::New();
	mPolyDataMapper = vtkPolyDataMapper::New();

	mPolyDataMapper->SetInput(mTool->getGraphicsPolyData());
	mToolActor->SetMapper(mPolyDataMapper);

	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Tool::TransformAndTimestampEventArgumentPtr)),
			this, SLOT(receiveTransforms(Tool::TransformAndTimestampEventArgumentPtr)));

	connect(mTool.get(), SIGNAL(toolVisible(Tool::VisibleEventArgumentPtr)),
			this, SLOT(receiveVisible(Tool::VisibleEventArgumentPtr)));
}

ToolRep3D::~ToolRep3D()
{}

std::string ToolRep3D::getType() const
{
	return "ssc::ToolRep3D";
}
void ToolRep3D::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddActor(mToolActor.GetPointer());
}
void ToolRep3D::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mToolActor.GetPointer());
}
void ToolRep3D::receiveTransforms(Transform3D matrix, double timestamp)
{
	mToolActor->SetUserMatrix(matrix.matrix().GetPointer());
}
void ToolRep3D::receiveVisible(bool visible)
{
	mToolActor->SetVisibility(visible);
}
} // namespace ssc
