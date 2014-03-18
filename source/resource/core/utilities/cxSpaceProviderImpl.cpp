// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#include "cxSpaceProviderImpl.h"

#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxData.h"
#include "cxImage.h"
#include "vtkImageData.h"
#include "cxReporter.h"
#include "cxSpaceListenerImpl.h"

namespace cx
{

SpaceProviderImpl::SpaceProviderImpl(TrackingServicePtr toolManager, DataServicePtr dataManager)
{
	mToolManager = toolManager;
	mDataManager = dataManager;
}

SpaceListenerPtr SpaceProviderImpl::createListener()
{
	return SpaceListenerPtr(new SpaceListenerImpl(mToolManager, mDataManager));
}

std::vector<CoordinateSystem> SpaceProviderImpl::getSpacesToPresentInGUI()
{
	std::vector<CoordinateSystem> retval;
	retval.push_back(CoordinateSystem(csREF));
	retval.push_back(CoordinateSystem(csPATIENTREF));

	// alias for the currently active tool:
	retval.push_back(CoordinateSystem(csDATA, "active"));
	retval.push_back(CoordinateSystem(csDATA_VOXEL, "active"));

	// alias for the currently active tool:
	retval.push_back(CoordinateSystem(csTOOL, "active"));
	retval.push_back(CoordinateSystem(csSENSOR, "active"));
	retval.push_back(CoordinateSystem(csTOOL_OFFSET, "active"));

	return retval;
}

Vector3D SpaceProviderImpl::getDominantToolTipPoint(CoordinateSystem to, bool useOffset)
{
	Transform3D toMfrom = this->getDominantToolTipTransform(to, useOffset);
	return toMfrom.coord(Vector3D(0,0,0));
}

CoordinateSystem SpaceProviderImpl::getToolCoordinateSystem(ToolPtr tool)
{
	QString dominantToolUid = tool->getUid();

	CoordinateSystem retval(csTOOL, dominantToolUid);
	return retval;
}

/** return toMfrom = qMt or qMto
  */
Transform3D SpaceProviderImpl::getDominantToolTipTransform(CoordinateSystem to, bool useOffset)
{
	ToolPtr tool = mToolManager->getDominantTool();
	if (!tool)
		return Transform3D::Identity();

	COORDINATE_SYSTEM target;
	if (useOffset)
		target = csTOOL_OFFSET;
	else
		target = csTOOL;
	CoordinateSystem from = CoordinateSystem(target, tool->getUid());

//	CoordinateSystem from = getToolCoordinateSystem(tool);
	Transform3D retval = this->get_toMfrom(from, to);
	return retval;
}

Transform3D SpaceProviderImpl::get_toMfrom(CoordinateSystem from, CoordinateSystem to)
{
	Transform3D to_M_from = get_rMfrom(to).inv() * get_rMfrom(from);
	return to_M_from;
}

Transform3D SpaceProviderImpl::get_rMfrom(CoordinateSystem from)
{
	Transform3D rMfrom = Transform3D::Identity();

	switch(from.mId)
	{
	case csREF:
		rMfrom = get_rMr();
		break;
	case csDATA:
		rMfrom = get_rMd(from.mRefObject);
		break;
	case csPATIENTREF:
		rMfrom = get_rMpr();
		break;
	case csTOOL:
		rMfrom = get_rMt(from.mRefObject);
		break;
	case csSENSOR:
		rMfrom = get_rMs(from.mRefObject);
		break;
	case csTOOL_OFFSET:
		rMfrom = get_rMto(from.mRefObject);
		break;
	case csDATA_VOXEL:
		rMfrom = get_rMdv(from.mRefObject);
		break;
	default:

		break;
	};

	return rMfrom;
}

CoordinateSystem SpaceProviderImpl::getS(ToolPtr tool)
{
	CoordinateSystem retval(csCOUNT);
	if (!tool)
		return retval;

	retval.mId = csSENSOR;
	retval.mRefObject = tool->getUid();

	return retval;
}

CoordinateSystem SpaceProviderImpl::getT(ToolPtr tool)
{
	CoordinateSystem retval(csCOUNT);
	if (!tool)
		return retval;

	ToolPtr refTool = mToolManager->getReferenceTool();
	if (refTool && (tool == refTool))
	{
		retval.mId = csPATIENTREF;
	}
	else
		retval.mId = csTOOL;

	retval.mRefObject = tool->getUid();

	return retval;
}

CoordinateSystem SpaceProviderImpl::getTO(ToolPtr tool)
{
	CoordinateSystem retval(csCOUNT);
	if (!tool)
		return retval;

	ToolPtr refTool = mToolManager->getReferenceTool();
	if (refTool && (tool == refTool))
	{
		retval.mId = csPATIENTREF;
	}
	else
		retval.mId = csTOOL_OFFSET;

	retval.mRefObject = tool->getUid();

	return retval;
}

CoordinateSystem SpaceProviderImpl::getD(DataPtr data)
{
	CoordinateSystem retval(csCOUNT);
	if (!data)
		return retval;

	retval.mId = csDATA;
	retval.mRefObject = data->getUid();

	return retval;
}

CoordinateSystem SpaceProviderImpl::getPr()
{
	CoordinateSystem pr(csPATIENTREF);
	return pr;
}

CoordinateSystem SpaceProviderImpl::getR()
{
	CoordinateSystem r(csREF);
	return r;
}

Transform3D SpaceProviderImpl::get_rMr()
{
	return Transform3D::Identity(); // ref_M_ref
}

Transform3D SpaceProviderImpl::get_rMd(QString uid)
{
	DataPtr data = mDataManager->getData(uid);

	if (!data && uid=="active")
		data = mDataManager->getActiveImage();

	if(!data)
	{
		reportWarning("Could not find data with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}
	return data->get_rMd(); // ref_M_d
}

Transform3D SpaceProviderImpl::get_rMdv(QString uid)
{
	DataPtr data = mDataManager->getData(uid);

	if (!data && uid=="active")
		data = mDataManager->getActiveImage();

	if(!data)
	{
		reportWarning("Could not find data with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}

	ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
	if (!image)
		return data->get_rMd();
	return data->get_rMd()*createTransformScale(Vector3D(image->getBaseVtkImageData()->GetSpacing())); // ref_M_d
}

Transform3D SpaceProviderImpl::get_rMpr()
{
	Transform3D rMpr = mDataManager->get_rMpr();
	return rMpr; //ref_M_pr
}

Transform3D SpaceProviderImpl::get_rMt(QString uid)
{
	ToolPtr tool = mToolManager->getTool(uid);

	if (!tool && uid=="active")
		tool = mToolManager->getDominantTool();

	if(!tool)
	{
		reportWarning("Could not find tool with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}
	return get_rMpr() * tool->get_prMt(); // ref_M_t
}

Transform3D SpaceProviderImpl::get_rMto(QString uid)
{
	ToolPtr tool = mToolManager->getTool(uid);

	if (!tool && uid=="active")
		tool = mToolManager->getDominantTool();

	if(!tool)
	{
		reportWarning("Could not find tool with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}

	double offset = tool->getTooltipOffset();
	Transform3D tMto = createTransformTranslate(Vector3D(0,0,offset));
	return get_rMpr() * tool->get_prMt() * tMto; // ref_M_to
}

Transform3D SpaceProviderImpl::get_rMs(QString uid)
{
	ToolPtr tool = mToolManager->getTool(uid);

	if (!tool && uid=="active")
		tool = mToolManager->getDominantTool();

	if(!tool)
	{
		reportWarning("Could not find tool with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}

	Transform3D tMs = tool->getCalibration_sMt().inv();

	Transform3D rMpr = get_rMpr();
	Transform3D prMt = tool->get_prMt();

	Transform3D rMs = rMpr * prMt * tMs;

	return rMs; //ref_M_s
}


} // namespace cx


