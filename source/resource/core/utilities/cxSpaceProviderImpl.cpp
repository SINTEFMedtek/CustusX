/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxSpaceProviderImpl.h"

#include "cxPatientModelService.h"
#include "cxTrackingService.h"
#include "cxData.h"
#include "cxImage.h"
#include "vtkImageData.h"
#include "cxLogger.h"
#include "cxSpaceListenerImpl.h"
#include "cxTool.h"
#include "cxActiveData.h"


namespace cx
{

SpaceProviderImpl::SpaceProviderImpl(TrackingServicePtr trackingService, PatientModelServicePtr dataManager) :
	mTrackingService(trackingService),
	mDataManager(dataManager)
{
//	connect(mTrackingService.get(), SIGNAL(stateChanged()), this, SIGNAL(spaceAddedOrRemoved()));
	connect(mTrackingService.get(), &TrackingService::stateChanged, this, &SpaceProvider::spaceAddedOrRemoved);
	connect(mDataManager.get(), &PatientModelService::dataAddedOrRemoved, this, &SpaceProvider::spaceAddedOrRemoved);
}

SpaceListenerPtr SpaceProviderImpl::createListener()
{
	return SpaceListenerPtr(new SpaceListenerImpl(mTrackingService, mDataManager));
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

	std::map<QString, DataPtr> data = mDataManager->getDatas();
	for (std::map<QString, DataPtr>::iterator i=data.begin(); i!=data.end(); ++i)
	{
		retval.push_back(CoordinateSystem(csDATA, i->second->getSpace()));
		retval.push_back(CoordinateSystem(csDATA_VOXEL, i->second->getSpace()));
	}

	std::map<QString, ToolPtr> tools = mTrackingService->getTools();
	for (std::map<QString, ToolPtr>::iterator i=tools.begin(); i!=tools.end(); ++i)
	{
		retval.push_back(CoordinateSystem(csTOOL, i->first));
		retval.push_back(CoordinateSystem(csSENSOR, i->first));
		retval.push_back(CoordinateSystem(csTOOL_OFFSET, i->first));
	}

	return retval;
}

std::map<QString, QString> SpaceProviderImpl::getDisplayNamesForCoordRefObjects()
{
	std::map<QString, QString> retval;

	retval["active"] = "active";

	std::map<QString, DataPtr> data = mDataManager->getDatas();
	for (std::map<QString, DataPtr>::iterator i=data.begin(); i!=data.end(); ++i)
	{
		retval[i->second->getSpace()] = i->second->getName();
	}

	std::map<QString, ToolPtr> tools = mTrackingService->getTools();
	for (std::map<QString, ToolPtr>::iterator i=tools.begin(); i!=tools.end(); ++i)
	{
		retval[i->first] = i->second->getName();
	}

	return retval;
}

Vector3D SpaceProviderImpl::getActiveToolTipPoint(CoordinateSystem to, bool useOffset)
{
	Transform3D toMfrom = this->getActiveToolTipTransform(to, useOffset);
	return toMfrom.coord(Vector3D(0,0,0));
}

CoordinateSystem SpaceProviderImpl::getToolCoordinateSystem(ToolPtr tool)
{
	QString uid = tool->getUid();

	CoordinateSystem retval(csTOOL, uid);
	return retval;
}

/** return toMfrom = qMt or qMto
  */
Transform3D SpaceProviderImpl::getActiveToolTipTransform(CoordinateSystem to, bool useOffset)
{
	ToolPtr tool = mTrackingService->getActiveTool();
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

	ToolPtr refTool = mTrackingService->getReferenceTool();
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

	ToolPtr refTool = mTrackingService->getReferenceTool();
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

CoordinateSystem SpaceProviderImpl::convertToSpecific(CoordinateSystem space)
{
	if (space.mRefObject!="active")
		return space;

	if (( space.mId==csDATA )||( space.mId==csDATA_VOXEL ))
	{
		DataPtr data = mDataManager->getActiveData()->getActiveUsingRegexp("image|trackedStream|mesh");
//		DataPtr data = mDataManager->getActiveData()->getActive();
		space.mRefObject = (data!=0) ? data->getUid() : "";
	}
	else if (( space.mId==csTOOL )||( space.mId==csSENSOR )||( space.mId==csTOOL_OFFSET))
	{
		ToolPtr tool = mTrackingService->getActiveTool();
		space.mRefObject = (tool!=0) ? tool->getUid() : "";
	}

	return space;
}

Transform3D SpaceProviderImpl::get_rMr()
{
	return Transform3D::Identity(); // ref_M_ref
}

Transform3D SpaceProviderImpl::get_rMd(QString uid)
{
	if (!mDataManager->isPatientValid())
		return Transform3D::Identity();

	DataPtr data = mDataManager->getData(uid);

	if (!data && uid=="active")
	{
		ActiveDataPtr activeData = mDataManager->getActiveData();
		data = activeData->getActive<Image>();
	}

	if(!data)
	{
		reportWarning("Could not find data with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}
	return data->get_rMd(); // ref_M_d
}

Transform3D SpaceProviderImpl::get_rMdv(QString uid)
{
	if (!mDataManager->isPatientValid())
		return Transform3D::Identity();

	DataPtr data = mDataManager->getData(uid);

	if (!data && uid=="active")
	{
		ActiveDataPtr activeData = mDataManager->getActiveData();
		data = activeData->getActive<Image>();
	}

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
	ToolPtr tool = mTrackingService->getTool(uid);

	if (!tool && uid=="active")
		tool = mTrackingService->getActiveTool();

	if(!tool)
	{
		reportWarning("Could not find tool with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}
	return get_rMpr() * tool->get_prMt(); // ref_M_t
}

Transform3D SpaceProviderImpl::get_rMto(QString uid)
{
	ToolPtr tool = mTrackingService->getTool(uid);

	if (!tool && uid=="active")
		tool = mTrackingService->getActiveTool();

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
	ToolPtr tool = mTrackingService->getTool(uid);

	if (!tool && uid=="active")
		tool = mTrackingService->getActiveTool();

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


