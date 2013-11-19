#include "sscCoordinateSystemHelpers.h"

#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscData.h"
#include "sscDefinitionStrings.h"
#include "sscImage.h"
#include "vtkImageData.h"
#include "sscProbe.h"

namespace cx
{

QString CoordinateSystem::toString() const
{
	return enum2string(mId) + (mRefObject.isEmpty() ? "" : ("/"+mRefObject));
}

CoordinateSystem CoordinateSystem::fromString(QString text)
{
	QStringList raw = text.split("/");
	CoordinateSystem retval(csCOUNT);
	if (raw.size()<1)
		return retval;
	retval.mId = string2enum<COORDINATE_SYSTEM>(raw[0]);
	if (raw.size()<2)
		return retval;
	retval.mRefObject = raw[1];
	return retval;
}

bool operator==(const CoordinateSystem& lhs, const CoordinateSystem& rhs)
{
	return ( lhs.mId==rhs.mId )&&( lhs.mRefObject==rhs.mRefObject );
}

// --------------------------------------------------------

std::vector<CoordinateSystem> CoordinateSystemHelpers::getSpacesToPresentInGUI()
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

	// active: alias for the currently active image
	retval.push_back(CoordinateSystem(csIMAGE_V, "active"));
	retval.push_back(CoordinateSystem(csIMAGE_U, "active"));
	retval.push_back(CoordinateSystem(csIMAGE_PIXEL, "active"));

	return retval;
}

Vector3D CoordinateSystemHelpers::getDominantToolTipPoint(CoordinateSystem to, bool useOffset)
{
    Transform3D toMfrom = CoordinateSystemHelpers::getDominantToolTipTransform(to, useOffset);
    return toMfrom.coord(Vector3D(0,0,0));
}

CoordinateSystem CoordinateSystemHelpers::getToolCoordinateSystem(ToolPtr tool)
{
	QString dominantToolUid = tool->getUid();

	CoordinateSystem retval(csTOOL, dominantToolUid);
	return retval;
}

/** return toMfrom = qMt or qMto
  */
Transform3D CoordinateSystemHelpers::getDominantToolTipTransform(CoordinateSystem to, bool useOffset)
{
	ToolPtr tool = toolManager()->getDominantTool();
	if (!tool)
		return Transform3D::Identity();

    COORDINATE_SYSTEM target;
    if (useOffset)
        target = csTOOL_OFFSET;
    else
        target = csTOOL;
    CoordinateSystem from = CoordinateSystem(target, tool->getUid());

//	CoordinateSystem from = getToolCoordinateSystem(tool);
	Transform3D retval = CoordinateSystemHelpers::get_toMfrom(from, to);
	return retval;
}

Transform3D CoordinateSystemHelpers::get_toMfrom(CoordinateSystem from, CoordinateSystem to)
{
	Transform3D to_M_from = get_rMfrom(to).inv() * get_rMfrom(from);
	return to_M_from;
}

Transform3D CoordinateSystemHelpers::get_rMfrom(CoordinateSystem from)
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
	case csIMAGE_PIXEL:
		rMfrom = get_rMp(from.mRefObject);
		break;
	case csIMAGE_V:
		rMfrom = get_rMv(from.mRefObject);
		break;
	case csIMAGE_U:
		rMfrom = get_rMu(from.mRefObject);
		break;
	default:

		break;
	};

	return rMfrom;
}

CoordinateSystem CoordinateSystemHelpers::getS(ToolPtr tool)
{
	CoordinateSystem retval(csCOUNT);
	if (!tool)
		return retval;

	retval.mId = csSENSOR;
	retval.mRefObject = tool->getUid();

	return retval;
}

CoordinateSystem CoordinateSystemHelpers::getT(ToolPtr tool)
{
	CoordinateSystem retval(csCOUNT);
	if (!tool)
		return retval;

	ToolPtr refTool = toolManager()->getReferenceTool();
	if (refTool && (tool == refTool))
	{
		retval.mId = csPATIENTREF;
	}
	else
		retval.mId = csTOOL;

	retval.mRefObject = tool->getUid();

	return retval;
}

CoordinateSystem CoordinateSystemHelpers::getTO(ToolPtr tool)
{
	CoordinateSystem retval(csCOUNT);
	if (!tool)
		return retval;

	ToolPtr refTool = toolManager()->getReferenceTool();
	if (refTool && (tool == refTool))
	{
		retval.mId = csPATIENTREF;
	}
	else
		retval.mId = csTOOL_OFFSET;

	retval.mRefObject = tool->getUid();

	return retval;
}

CoordinateSystem CoordinateSystemHelpers::getD(DataPtr data)
{
	CoordinateSystem retval(csCOUNT);
	if (!data)
		return retval;

	retval.mId = csDATA;
	retval.mRefObject = data->getUid();

	return retval;
}

CoordinateSystem CoordinateSystemHelpers::getPr()
{
	CoordinateSystem pr(csPATIENTREF);
	return pr;
}

CoordinateSystem CoordinateSystemHelpers::getR()
{
	CoordinateSystem r(csREF);
	return r;
}

CoordinateSystem CoordinateSystemHelpers::getP()
{
	CoordinateSystem p(csIMAGE_PIXEL);
	return p;
}

Transform3D CoordinateSystemHelpers::get_rMr()
{
	return Transform3D::Identity(); // ref_M_ref
}

Transform3D CoordinateSystemHelpers::get_rMd(QString uid)
{
	DataPtr data = dataManager()->getData(uid);

	if (!data && uid=="active")
		data = dataManager()->getActiveImage();

	if(!data)
	{
		messageManager()->sendWarning("Could not find data with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}
	return data->get_rMd(); // ref_M_d
}

Transform3D CoordinateSystemHelpers::get_rMdv(QString uid)
{
	DataPtr data = dataManager()->getData(uid);

	if (!data && uid=="active")
		data = dataManager()->getActiveImage();

	if(!data)
	{
		messageManager()->sendWarning("Could not find data with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}

	ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
	if (!image)
		return data->get_rMd();
	return data->get_rMd()*createTransformScale(Vector3D(image->getBaseVtkImageData()->GetSpacing())); // ref_M_d
}

Transform3D CoordinateSystemHelpers::get_rMpr()
{
	Transform3D rMpr = *(toolManager()->get_rMpr());
	return rMpr; //ref_M_pr
}

Transform3D CoordinateSystemHelpers::get_rMt(QString uid)
{
	ToolPtr tool = toolManager()->getTool(uid);

	if (!tool && uid=="active")
		tool = toolManager()->getDominantTool();

	if(!tool)
	{
		messageManager()->sendWarning("Could not find tool with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}
	return get_rMpr() * tool->get_prMt(); // ref_M_t
}

Transform3D CoordinateSystemHelpers::get_rMto(QString uid)
{
	ToolPtr tool = toolManager()->getTool(uid);

	if (!tool && uid=="active")
		tool = toolManager()->getDominantTool();

	if(!tool)
	{
		messageManager()->sendWarning("Could not find tool with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}

	double offset = tool->getTooltipOffset();
	Transform3D tMto = createTransformTranslate(Vector3D(0,0,offset));
	return get_rMpr() * tool->get_prMt() * tMto; // ref_M_to
}

Transform3D CoordinateSystemHelpers::get_rMs(QString uid)
{
	ToolPtr tool = toolManager()->getTool(uid);

	if (!tool && uid=="active")
		tool = toolManager()->getDominantTool();

	if(!tool)
	{
		messageManager()->sendWarning("Could not find tool with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
		return Transform3D::Identity();
	}

	Transform3D tMs = tool->getCalibration_sMt().inv();

	Transform3D rMpr = get_rMpr();
	Transform3D prMt = tool->get_prMt();

	Transform3D rMs = rMpr * prMt * tMs;

	return rMs; //ref_M_s
}

Transform3D CoordinateSystemHelpers::get_rMv(QString toolUid)
{
	ProbePtr probe = getProbe(toolUid);
	if(!probe)
		return Transform3D::Identity();

	Transform3D rMu = get_rMu(toolUid);
	Transform3D uMv = probe->get_uMv();
	Transform3D rMv = rMu * uMv;

	return rMv;
}

Transform3D CoordinateSystemHelpers::get_rMu(QString toolUid)
{
	ProbePtr probe = getProbe(toolUid);
	Transform3D rMt;
	if(probe)
		rMt = get_rMt(toolUid);
	else
		rMt = Transform3D::Identity();

	Transform3D tMu = probe->get_tMu();
	Transform3D rMu = rMt * tMu;
	return rMu;
}

Transform3D CoordinateSystemHelpers::get_rMp(QString toolUid)
{
	ProbePtr probe = getProbe(toolUid);
	if(!probe)
		return Transform3D::Identity();

	Transform3D rMv = get_rMv(toolUid);
	Transform3D vMp = probe->get_vMp();
	Transform3D rMp = rMv * vMp;

	return rMp;
}


ProbePtr CoordinateSystemHelpers::getProbe(QString toolUid)
{
	ProbePtr probe;

	ToolPtr tool = toolManager()->getTool(toolUid);
	if(!tool)
		messageManager()->sendWarning("Could not find tool with uid: "+toolUid+". Can not find transform to unknown coordinate system, returning identity!");

	probe = tool->getProbe();
	if(!probe)
		messageManager()->sendWarning("Tool with uid: "+toolUid+" is not a probe. Can not find transform to unknown coordinate system, returning identity!");

	return probe;
}



} //namespace cx
