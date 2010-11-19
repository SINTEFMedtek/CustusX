#include "sscToolRep3D.h"

#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkSTLReader.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>

#include "sscGraphicalPrimitives.h"
#include "sscToolManager.h"
#include "sscTool.h"
#include "sscView.h"
#include "sscUSProbeSector.h"

#include "sscTypeConversions.h"

namespace ssc
{

ToolRep3D::ToolRep3D(const QString& uid, const QString& name) :
	RepImpl(uid, name)
{
  mStayHiddenAfterVisible = false;
	mStayVisibleAfterHide = false;
	mOffsetPointVisibleAtZeroOffset = false;
	mToolActor = vtkActorPtr::New();
	mPolyDataMapper = vtkPolyDataMapperPtr::New();
	mSTLReader = vtkSTLReaderPtr::New();

	mOffsetPoint.reset(new GraphicalPoint3D());
	mOffsetLine.reset(new GraphicalLine3D());
	mTooltipPoint.reset(new GraphicalPoint3D());

	mProbeSector.reset(new ProbeData());
	mProbeSectorPolyDataMapper = vtkPolyDataMapperPtr::New();
	mProbeSectorActor = vtkActorPtr::New();

  //std::cout << "Tool3DRep: construct " << std::endl;

//	if (pd::Settings::instance()->useDebugAxis())
//	 	{
//		 	mTool->AddPart( Axes3D().getProp() );
//	 	}
}

ToolRep3D::~ToolRep3D()
{
}

ToolRep3DPtr ToolRep3D::New(const QString& uid, const QString& name)
{
	ToolRep3DPtr retval(new ToolRep3D(uid, name));
	retval->mSelf = retval;
	return retval;
}

QString ToolRep3D::getType() const
{
	return "ssc::ToolRep3D";
}

ToolPtr ToolRep3D::getTool()
{
	return mTool;
}

void ToolRep3D::setTool(ToolPtr tool)
{
	if (tool==mTool)
		return;
	
	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
		disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SLOT(tooltipOffsetSlot(double)));
    disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));

		mToolActor->SetMapper(NULL);
	}

	mTool = tool;

	// setup new
	if (mTool)
	{
		QString filename = mTool->getGraphicsFileName();
		if (!filename.isEmpty() && filename.endsWith("STL"))
		{
			mSTLReader->SetFileName( cstring_cast(filename) );
			mPolyDataMapper->SetInputConnection( mSTLReader->GetOutputPort() );	 //read a 3D model file of the tool
		}
		else
		{
			mPolyDataMapper->SetInput( mTool->getGraphicsPolyData() ); // creates a cone, default
		}

		if (mPolyDataMapper->GetInput())
		{
			mToolActor->SetMapper(mPolyDataMapper);
		}

		//some color to 3D cursor
		mToolActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
		if (mTool->getType() == Tool::TOOL_MANUAL)
		{
			mToolActor->GetProperty()->SetColor(1.0, 0.8, 0.0);
		}

		receiveTransforms(mTool->get_prMt(), 0);
		//std::cout << "Tool3DRep: set tool" << std::endl;
		mToolActor->SetVisibility(mTool->getVisible());

		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)),	this, SLOT(receiveVisible(bool)));
		connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SLOT(tooltipOffsetSlot(double)));
    connect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));

    this->probeSectorChanged();
	}
}

bool ToolRep3D::hasTool(ToolPtr tool) const
{
	return (mTool == tool);
}

void ToolRep3D::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddActor(mToolActor);
  view->getRenderer()->AddActor(mProbeSectorActor);

	mOffsetPoint.reset(new GraphicalPoint3D(view->getRenderer()));
	mOffsetPoint->setRadius(2);
	mOffsetPoint->setColor(Vector3D(1,0.8,0));

	mTooltipPoint.reset(new GraphicalPoint3D(view->getRenderer()));
	mTooltipPoint->setRadius(2);
	mTooltipPoint->setColor(Vector3D(1,0.8,0)); //light green
	//mTooltipPoint->setColor(Vector3D(0.25,0.87,0.16)); //light green

	mOffsetLine.reset(new GraphicalLine3D(view->getRenderer()));
	mOffsetLine->setColor(Vector3D(1,0.8,0));

	mTooltipPoint->getActor()->SetVisibility(false);
	mOffsetPoint->getActor()->SetVisibility(false);
	mOffsetLine->getActor()->SetVisibility(false);
}

void ToolRep3D::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mToolActor);
  view->getRenderer()->RemoveActor(mProbeSectorActor);

	mTooltipPoint.reset(new GraphicalPoint3D());
	mOffsetPoint.reset(new GraphicalPoint3D());
	mOffsetLine.reset(new GraphicalLine3D());
}

void ToolRep3D::receiveTransforms(Transform3D prMt, double timestamp)
{
	Transform3DPtr rMprPtr = ssc::ToolManager::getInstance()->get_rMpr();
	Transform3D rMt = (*rMprPtr)*prMt;
//	std::cout << "tool " << mTool->getUid() << std::endl;
//	if (mToolActor->GetUserMatrix() && similar(rMt, ssc::Transform3D(mToolActor->GetUserMatrix())))
//	  return;
//	std::cout << "passed" << std::endl;
	mToolActor->SetUserMatrix( rMt.matrix());
	//updateOffsetGraphics();
	this->update();
}

void ToolRep3D::update()
{
  Transform3D prMt;
  if (mTool)
  {
    prMt = mTool->get_prMt();
  }
  Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
//  mProbeSector->setPosition(rMpr*prMt);

  if (this->showProbe())
  {
//    mProbeSector->setSector(mTool->getProbeSector());
    Transform3D tMu = mProbeSector->get_tMu();

//    mProbeSectorPolyDataMapper->SetInput(mProbeSector->getSectorLinesOnly());
//    if (mProbeSectorPolyDataMapper->GetInput())
//    {
//      mProbeSectorActor->SetMapper(mProbeSectorPolyDataMapper);
//    }
    mProbeSectorActor->SetUserMatrix((rMpr*prMt*tMu).matrix());
    mProbeSectorActor->SetVisibility(mTool->getVisible());
  }
  else
    mProbeSectorActor->SetVisibility(false);

  this->updateOffsetGraphics();
}


void ToolRep3D::probeSectorChanged()
{
  if (!mTool)
    return;

  Transform3D prMt = mTool->get_prMt();
  Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();

  if (this->showProbe())
  {
    mProbeSector->setSector(mTool->getProbeSector());
    Transform3D tMu = mProbeSector->get_tMu();

    mProbeSectorPolyDataMapper->SetInput(mProbeSector->getSectorLinesOnly());
//    mProbeSectorPolyDataMapper->SetInput(mProbeSector->getSector());
    if (mProbeSectorPolyDataMapper->GetInput())
    {
      mProbeSectorActor->SetMapper(mProbeSectorPolyDataMapper);
    }
    mProbeSectorActor->SetUserMatrix((rMpr*prMt*tMu).matrix());
    mProbeSectorActor->SetVisibility(mTool->getVisible());
  }
  else
    mProbeSectorActor->SetVisibility(false);

}


void ToolRep3D::updateOffsetGraphics()
{

	bool visible = mTool && mTool->getVisible() && mTool->getType()!=Tool::TOOL_US_PROBE; // no offset for probes

	if (!mStayVisibleAfterHide || (mOffsetPoint->getActor()->GetVisibility()==false))
	{
		mOffsetPoint->getActor()->SetVisibility(visible);
		mTooltipPoint->getActor()->SetVisibility(visible);
		mOffsetLine->getActor()->SetVisibility(visible);
	}

	if (similar(0.0, mTool->getTooltipOffset()))
	{
	    mTooltipPoint->getActor()->SetVisibility(visible && mOffsetPointVisibleAtZeroOffset);
		mOffsetPoint->getActor()->SetVisibility(false);
		mOffsetLine->getActor()->SetVisibility(false);
	}

	if (!mTool)
		return;
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
	Transform3D rMt = rMpr * mTool->get_prMt();

	Vector3D p0 = rMt.coord(Vector3D(0,0,0));
	Vector3D p1 = rMt.coord(Vector3D(0,0,mTool->getTooltipOffset()));
	mOffsetPoint->setValue(p1);
	mOffsetLine->setValue(p0,p1);
	mTooltipPoint->setValue(Vector3D(p0));
}

void ToolRep3D::receiveVisible(bool visible)
{
//  std::cout << "ToolRep3D::receiveVisible " << mTool->getName() << std::endl;

	if (!visible && mStayVisibleAfterHide)
	{
		return; // don't hide
	}
	mToolActor->SetVisibility(visible);

  if(mStayHiddenAfterVisible)
  {
    mToolActor->SetVisibility(false);
  }else
  {
    mToolActor->SetVisibility(mTool->getVisible());
  }

//  std::cout << "Tool3DRep: receiveVisible "<< mTool->getName() << visible << " - " << mStayHiddenAfterVisible << std::endl;

	//updateOffsetGraphics();
  this->update();
}

void ToolRep3D::setStayHiddenAfterVisible(bool val)
{
  mStayHiddenAfterVisible = val;
  if (mTool)
    receiveVisible(mTool->getVisible());
}

/**
 * If true, tool is still rendered as visible after visibility status is hidden.
 * Nice for viewing the last known position of a tool.
 */
void ToolRep3D::setStayVisibleAfterHide(bool val)
{
	mStayVisibleAfterHide = val;
}
void ToolRep3D::setOffsetPointVisibleAtZeroOffset(bool val)
{
  mOffsetPointVisibleAtZeroOffset = val;
}
void ToolRep3D::tooltipOffsetSlot(double val)
{
	updateOffsetGraphics();
}

bool ToolRep3D::showProbe()
{
//  return mTool && mTool->getType()==ssc::Tool::TOOL_US_PROBE;
  return mTool;;
}
} // namespace ssc
