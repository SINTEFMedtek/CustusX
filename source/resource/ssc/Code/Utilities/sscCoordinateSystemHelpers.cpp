#include "sscCoordinateSystemHelpers.h"

#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscData.h"
#include "sscDefinitionStrings.h"
#include "sscImage.h"
#include "vtkImageData.h"

namespace ssc
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

std::vector<CoordinateSystem> CoordinateSystemHelpers::getAvailableSpaces(bool compact)
{
	std::vector<CoordinateSystem> retval;
	retval.push_back(CoordinateSystem(csREF));
	retval.push_back(CoordinateSystem(csPATIENTREF));

	// alias for the currently active tool:
	retval.push_back(CoordinateSystem(csDATA, "active"));
	retval.push_back(CoordinateSystem(csDATA_VOXEL, "active"));

	if (!compact)
	{
		std::set<QString> dataSpaces;
		std::map<QString, DataPtr> data = dataManager()->getData();
		for (std::map<QString, DataPtr>::iterator iter=data.begin(); iter!=data.end(); ++iter)
		{
			dataSpaces.insert(iter->second->getSpace());
	//		dataSpaces.insert(iter->second->getParentSpace()); // system only handle spaces identical to data.
		}
		dataSpaces.erase("");
		for (std::set<QString>::iterator iter=dataSpaces.begin(); iter!=dataSpaces.end(); ++iter)
		{
			retval.push_back(CoordinateSystem(csDATA, *iter));
		}
		for (std::set<QString>::iterator iter=dataSpaces.begin(); iter!=dataSpaces.end(); ++iter)
		{
			retval.push_back(CoordinateSystem(csDATA_VOXEL, *iter));
		}
	}

	// alias for the currently active tool:
	retval.push_back(CoordinateSystem(csTOOL, "active"));
	retval.push_back(CoordinateSystem(csSENSOR, "active"));
	retval.push_back(CoordinateSystem(csTOOL_OFFSET, "active"));

	if (!compact)
	{
		std::map<QString, ToolPtr> tools = *toolManager()->getTools();
		for (std::map<QString, ToolPtr>::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
		{
			retval.push_back(CoordinateSystem(csTOOL, iter->first));
		}
		for (std::map<QString, ToolPtr>::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
		{
			retval.push_back(CoordinateSystem(csSENSOR, iter->first));
		}
		for (std::map<QString, ToolPtr>::iterator iter=tools.begin(); iter!=tools.end(); ++iter)
		{
			retval.push_back(CoordinateSystem(csTOOL_OFFSET, iter->first));
		}
	}

	return retval;
}

Vector3D CoordinateSystemHelpers::getDominantToolTipPoint(CoordinateSystem to, bool useOffset)
{
	ToolPtr tool = ssc::toolManager()->getDominantTool();
	if (!tool)
		return ssc::Vector3D(0,0,0);

	QString dominantToolUid = tool->getUid();

	CoordinateSystem from(csTOOL, dominantToolUid);

	Vector3D point_t;
	if(useOffset)
		point_t = Vector3D(0,0,tool->getTooltipOffset());
	else
		point_t = Vector3D(0,0,0);

	Vector3D P_to = CoordinateSystemHelpers::get_toMfrom(from, to).coord(point_t);

	return P_to;
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
	default:

		break;
	};

	return rMfrom;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getS(ssc::ToolPtr tool)
{
	ssc::CoordinateSystem retval(csCOUNT);
	if (!tool)
		return retval;

	retval.mId = ssc::csSENSOR;
	retval.mRefObject = tool->getUid();

	return retval;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getT(ssc::ToolPtr tool)
{
	ssc::CoordinateSystem retval(csCOUNT);
	if (!tool)
		return retval;

	ssc::ToolPtr refTool = ssc::toolManager()->getReferenceTool();
	if (refTool && (tool == refTool))
	{
		retval.mId = ssc::csPATIENTREF;
	}
	else
		retval.mId = ssc::csTOOL;

	retval.mRefObject = tool->getUid();

	return retval;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getTO(ssc::ToolPtr tool)
{
	ssc::CoordinateSystem retval(csCOUNT);
	if (!tool)
		return retval;

	ssc::ToolPtr refTool = ssc::toolManager()->getReferenceTool();
	if (refTool && (tool == refTool))
	{
		retval.mId = ssc::csPATIENTREF;
	}
	else
		retval.mId = ssc::csTOOL_OFFSET;

	retval.mRefObject = tool->getUid();

	return retval;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getD(ssc::DataPtr data)
{
	ssc::CoordinateSystem retval(csCOUNT);
	if (!data)
		return retval;

	retval.mId = ssc::csDATA;
	retval.mRefObject = data->getUid();

	return retval;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getPr()
{
	CoordinateSystem pr(ssc::csPATIENTREF);
	return pr;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getR()
{
	CoordinateSystem pr(ssc::csREF);
	return pr;
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

	ssc::ImagePtr image = boost::dynamic_pointer_cast<ssc::Image>(data);
	if (!image)
		return data->get_rMd();
	return data->get_rMd()*ssc::createTransformScale(ssc::Vector3D(image->getBaseVtkImageData()->GetSpacing())); // ref_M_d
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

} //namespace ssc
