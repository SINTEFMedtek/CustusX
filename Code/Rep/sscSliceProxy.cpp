#include "sscSliceProxy.h"

#include <boost/lexical_cast.hpp>
#include "sscDataManager.h"
#include "sscToolManager.h"

namespace ssc

{

SliceProxy::SliceProxy()
{
	connect(ssc::DataManager::getInstance(), SIGNAL(centerChanged()),this, SLOT(centerChangedSlot()) ) ;
	//TODO connect to toolmanager rMpr changed
	centerChangedSlot();
	//std::cout<<"Got center"<< ssc::DataManager::getInstance()->getCenter() <<std::endl;
}

SliceProxy::~SliceProxy()
{
}

void SliceProxy::setTool(ToolPtr tool)
{
	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));		

		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(toolTransformAndTimestamp(Transform3D,double)));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));		
	}	
	
	mTool = tool;
	
	if (mTool)
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));		

		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(toolTransformAndTimestamp(Transform3D,double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));		

		toolTransformAndTimestampSlot(mTool->get_prMt(), 0); // initial values
	}	
	
	centerChangedSlot(); // force center update for tool==0
	changed();	
}

void SliceProxy::toolTransformAndTimestampSlot(Transform3D prMt, double timestamp)
{
	//std::cout << "proxy get transform" << std::endl;
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
	mCutplane.setToolPosition(rMpr*prMt);	
	changed();
}

void SliceProxy::toolVisibleSlot(bool visible)
{
	
}

/**Provide a nice default transform for displays without a tool.
 */
Transform3D SliceProxy::getSyntheticToolPos(const Vector3D& center) const
{
	Transform3D R_tq = createTransformRotateY(M_PI) * createTransformRotateZ(M_PI_2);
	Transform3D T_c = createTransformTranslate(center);
	return T_c * R_tq;
}

void SliceProxy::centerChangedSlot()
{
	Vector3D c = ssc::DataManager::getInstance()->getCenter();
	mCutplane.setFixedCenter(c);
	
	if (!mTool)
	{
		mCutplane.setToolPosition(getSyntheticToolPos(c));			
	}	
	
	//std::cout << "center changed: " + boost::lexical_cast<std::string>(ssc::DataManager::instance()->getCenter());
	changed();
}

void SliceProxy::setOrientation(ORIENTATION_TYPE orientation)
{
	mCutplane.setOrientationType(orientation);	
	changed();
}

void SliceProxy::setPlane(PLANE_TYPE plane)
{
	mCutplane.setPlaneType(plane);	
	changed();
}

void SliceProxy::setFollowType(FOLLOW_TYPE followType)
{
	mCutplane.setFollowType(followType);	
	changed();
}

void SliceProxy::setGravity(bool use, const Vector3D& dir)
{
	mCutplane.setGravity(use, dir);
}
void SliceProxy::setToolViewOffset(bool use, double viewportHeight, double toolViewOffset)
{
	mCutplane.setToolViewOffset(use, viewportHeight, toolViewOffset);
}
 
void SliceProxy::setToolViewportHeight(double viewportHeight)
{
	mCutplane.setToolViewportHeight(viewportHeight);
}

ToolPtr SliceProxy::getTool()
{
	return mTool;
}

Transform3D SliceProxy::get_sMr()
{
	SlicePlane plane = mCutplane.getPlane();
	//std::cout << mDebugId << " proxy get transform.c : " << plane.c << std::endl;
	//std::cout << "proxy get transform :\n" << plane << std::endl;
	return createTransformIJC(plane.i, plane.j, plane.c).inv();	
}

void SliceProxy::changed()
{
	emit transformChanged(get_sMr());
}


} // namespace ssc
