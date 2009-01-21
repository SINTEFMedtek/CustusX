#include "sscToolRep3D.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>

#include "sscToolManager.h"
#include "sscTool.h"
#include "sscView.h"

namespace ssc
{

ToolRep3D::ToolRep3D(const std::string& uid, const std::string& name) :
	RepImpl(uid, name)
{
	mToolActor = vtkActorPtr::New();
	mPolyDataMapper = vtkPolyDataMapper::New();
	mSTLReader = vtkSTLReader::New();
	
//	if (pd::Settings::instance()->useDebugAxis())
//	 	{
//		 	mTool->AddPart( Axes3D().getProp() );
//	 	}
}
	
ToolRep3D::~ToolRep3D()
{}
ToolRep3DPtr ToolRep3D::New(const std::string& uid, const std::string& name)
{
	ToolRep3DPtr retval(new ToolRep3D(uid, name));
	retval->mSelf = retval;
	return retval;
}

std::string ToolRep3D::getType() const
{
	return "ssc::ToolRep3D";
}

ToolPtr ToolRep3D::getTool()
{
	return mTool;
}

void ToolRep3D::setTool(ToolPtr tool)
{
	if(!tool)
	{
		std::cout<<"this tool is no longer vaild!"<<std::endl;
	}
	mTool = tool;
	std::string filename = mTool->getGraphicsFileName();
	
	if (!filename.empty() && filename.compare(filename.size()-3,3,"STL") == 0 )
	{
		std::cout<<"reading filename :" << filename <<	std::endl;
		mSTLReader->SetFileName( filename.c_str() ); 
		mPolyDataMapper->SetInputConnection( mSTLReader->GetOutputPort() );	 //read a 3D model file of the tool
	}
	else 
	{
		mPolyDataMapper->SetInput( mTool->getGraphicsPolyData() ); // creates a cone, default
		std::cout<<"filename empty, making a cone :" <<	std::endl;	
	}
	
	
	std::cout<<"setMapper"<<std::endl;
	mToolActor->SetMapper(mPolyDataMapper);
	
	connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
			this, SLOT(receiveTransforms(Transform3D, double)));

	connect(mTool.get(), SIGNAL(toolVisible(bool)),
			this, SLOT(receiveVisible(bool)));
	
}
bool ToolRep3D::hasTool(ToolPtr tool) const
{
	return (mTool != NULL);
}
void ToolRep3D::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddActor(mToolActor);
}
void ToolRep3D::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mToolActor);
}
void ToolRep3D::receiveTransforms(Transform3D prMt, double timestamp)
{
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
	
//	std::cout << "----------" << std::endl;
//	std::cout << "prMt\n" << prMt << std::endl;
//	std::cout << "rMpr\n" << rMpr << std::endl;
//	std::cout << "----------" << std::endl;
	
	Transform3D rMt = rMpr*prMt;	
	mToolActor->SetUserMatrix( rMt.matrix());
}
void ToolRep3D::receiveVisible(bool visible)
{
	mToolActor->SetVisibility(visible);
}

} // namespace ssc
