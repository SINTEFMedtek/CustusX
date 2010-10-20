#include "sscSliceProxy.h"

#include <math.h>
#include "sscTypeConversions.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscSliceComputer.h"

namespace ssc
{

SliceProxyPtr SliceProxy::New(const QString& name)
{
  SliceProxyPtr retval(new SliceProxy);
  retval->mName = name;
  return retval;
}

QString SliceProxy::getName() const
{
  return mName;
}

SliceProxy::SliceProxy() :
    mCutplane(new SliceComputer())
{
	connect(ssc::DataManager::getInstance(), SIGNAL(centerChanged()),this, SLOT(centerChangedSlot()) ) ;
	connect(dataManager(), SIGNAL(medicalDomainChanged()), this, SLOT(medicalDomainChangedSlot()));
	//TODO connect to toolmanager rMpr changed
	mDefaultCenter = ssc::DataManager::getInstance()->getCenter();
	centerChangedSlot();
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
		disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SLOT(tooltipOffsetSlot(double)));		
		disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(changed())); 		
	}	
	
	mTool = tool;
	
	if (mTool)
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));		
		connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SLOT(tooltipOffsetSlot(double)));		
		connect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(changed()));/// not used here, but forwarded to users		

		emit toolVisible(mTool->getVisible());
		toolTransformAndTimestampSlot(mTool->get_prMt(), 0); // initial values
		tooltipOffsetSlot(mTool->getTooltipOffset());
	}	
	
	centerChangedSlot(); // force center update for tool==0
	changed();	
}

void SliceProxy::toolTransformAndTimestampSlot(Transform3D prMt, double timestamp)
{
	//std::cout << "proxy get transform" << std::endl;
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
	Transform3D rMt = rMpr*prMt;
//	if (similar(rMt, mCutplane->getToolPosition()))
//	{
//    return;
//	}
	mCutplane->setToolPosition(rMt);
	changed();
}

void SliceProxy::tooltipOffsetSlot(double val)
{
	mCutplane->setToolOffset(val);
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

void SliceProxy::setDefaultCenter(const Vector3D& c)
{
	mDefaultCenter = c;	
	centerChangedSlot();
}

void SliceProxy::centerChangedSlot()
{
	if (mTool)
	{
		Vector3D c = ssc::DataManager::getInstance()->getCenter();
		mCutplane->setFixedCenter(c);
	  //std::cout << "center changed: " + string_cast(c) << std::endl;
	}
	else
	{
		// If no tool is available, ensure only dummy values are used.
		// It is very important that this volume is completely frozen in order
		// to avoid any confusion - the user must know it is nonnavigable.
		mCutplane->setFixedCenter(mDefaultCenter);
		mCutplane->setToolPosition(getSyntheticToolPos(mDefaultCenter));
	  //std::cout << "center changed: " + string_cast(mDefaultCenter) << std::endl;
	}	
	
	changed();
}

void SliceProxy::medicalDomainChangedSlot()
{
  mCutplane->setMedicalDomain(dataManager()->getMedicalDomain());
  changed();
}

/**Group the typical plane definition uses together.
 */
void SliceProxy::initializeFromPlane(PLANE_TYPE plane, bool useGravity, const Vector3D& gravityDir, bool useViewOffset, double viewportHeight, double toolViewOffset)
{
  mCutplane->initializeFromPlane(plane, useGravity, gravityDir, useViewOffset, viewportHeight, toolViewOffset, dataManager()->getMedicalDomain());
  changed();
//	setPlane(plane);
//	//Logger::log("vm.log"," set plane to proxy ");
//	if (plane == ptSAGITTAL || plane == ptCORONAL || plane == ptAXIAL )
//	{
//		setOrientation(ssc::otORTHOGONAL);
//		setFollowType(ssc::ftFIXED_CENTER);
//	}
//	else if (plane == ptANYPLANE || plane==ptRADIALPLANE || plane==ptSIDEPLANE)
//	{
//		setOrientation(ssc::otOBLIQUE);
//		setFollowType(ssc::ftFOLLOW_TOOL);
//
//		setGravity(useGravity, gravityDir);
//		setToolViewOffset(useViewOffset, viewportHeight, toolViewOffset); // TODO finish this one
//	}
}

SliceComputer SliceProxy::getComputer() const
{
  return *mCutplane;
}

void SliceProxy::setComputer(const SliceComputer& val)
{
  mCutplane.reset(new SliceComputer(val));
  changed();
}

void SliceProxy::setOrientation(ORIENTATION_TYPE orientation)
{
	mCutplane->setOrientationType(orientation);
	changed();
}

void SliceProxy::setPlane(PLANE_TYPE plane)
{
	mCutplane->setPlaneType(plane);
	changed();
}

void SliceProxy::setFollowType(FOLLOW_TYPE followType)
{
	mCutplane->setFollowType(followType);
	changed();
}

void SliceProxy::setGravity(bool use, const Vector3D& dir)
{
	mCutplane->setGravity(use, dir);
}
void SliceProxy::setToolViewOffset(bool use, double viewportHeight, double toolViewOffset)
{
	mCutplane->setToolViewOffset(use, viewportHeight, toolViewOffset);
}
 
void SliceProxy::setToolViewportHeight(double viewportHeight)
{
	mCutplane->setToolViewportHeight(viewportHeight);
}

ToolPtr SliceProxy::getTool()
{
	return mTool;
}

Transform3D SliceProxy::get_sMr()
{
	SlicePlane plane = mCutplane->getPlane();
	//std::cout << "---" << " proxy get transform.c : " << plane.c << std::endl;
	//std::cout << "proxy get transform -" << getName() <<":\n" << plane << std::endl;
	return createTransformIJC(plane.i, plane.j, plane.c).inv();	
}

void SliceProxy::changed()
{
	emit transformChanged(get_sMr());
}

void SliceProxy::printSelf(std::ostream & os, Indent indent)
{
	os << indent << "sliceproxy" << std::endl;
	os << indent << "sMr: " << std::endl;
	get_sMr().put(os, indent.getIndent()+3);
	os << std::endl;
}

} // namespace ssc
