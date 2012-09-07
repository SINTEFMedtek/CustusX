#include "sscCoordinateSystemHelpers.h"

#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscData.h"
#include "sscDefinitionStrings.h"

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

std::vector<CoordinateSystem> CoordinateSystemHelpers::getAvailableSpaces()
{
	std::vector<CoordinateSystem> retval;
	retval.push_back(CoordinateSystem(csREF));
	retval.push_back(CoordinateSystem(csPATIENTREF));

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

	// alias for the currently active tool:
  retval.push_back(CoordinateSystem(csTOOL, "active"));
  retval.push_back(CoordinateSystem(csSENSOR, "active"));
  retval.push_back(CoordinateSystem(csTOOL_OFFSET, "active"));

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

	return retval;
}

Vector3D CoordinateSystemHelpers::getDominantToolTipPoint(CoordinateSystem to, bool useOffset)
{
  ToolPtr tool = ssc::toolManager()->getDominantTool();
  if (!tool)
    return ssc::Vector3D(0,0,0);

  QString dominantToolUid = tool->getUid();

  CoordinateSystem from;
  from.mId = csTOOL;
  from.mRefObject = dominantToolUid;

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
  CoordinateSystemHelpers helper;
  Transform3D to_M_from = helper.get_rMfrom(to).inv() * helper.get_rMfrom(from);
  return to_M_from;
}

Transform3D CoordinateSystemHelpers::get_rMfrom(CoordinateSystem from) const
{
  Transform3D rMfrom = Transform3D::Identity();

  switch(from.mId)
  {
  case csREF:
    rMfrom = this->get_rMr();
    break;
  case csDATA:
    rMfrom = this->get_rMd(from.mRefObject);
    break;
  case csPATIENTREF:
    rMfrom = this->get_rMpr();
    break;
  case csTOOL:
    rMfrom = this->get_rMt(from.mRefObject);
    break;
  case csSENSOR:
    rMfrom = this->get_rMs(from.mRefObject);
    break;
  case csTOOL_OFFSET:
    rMfrom = this->get_rMto(from.mRefObject);
    break;
  default:

    break;
  };

  return rMfrom;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getPr()
{
  CoordinateSystem pr;
  pr.mId = ssc::csPATIENTREF;
  pr.mRefObject = "";
  return pr;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getR()
{
  CoordinateSystem pr;
  pr.mId = ssc::csREF;
  pr.mRefObject = "";
  return pr;
}

Transform3D CoordinateSystemHelpers::get_rMr() const
{
  return Transform3D::Identity(); // ref_M_ref
}

Transform3D CoordinateSystemHelpers::get_rMd(QString uid) const
{
  DataPtr data = dataManager()->getData(uid);

  if(!data)
  {
   messageManager()->sendWarning("Could not find data with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
    return Transform3D::Identity();
  }
  return data->get_rMd(); // ref_M_d
}

Transform3D CoordinateSystemHelpers::get_rMpr() const
{
  Transform3D rMpr = *(toolManager()->get_rMpr());
  return rMpr; //ref_M_pr
}

Transform3D CoordinateSystemHelpers::get_rMt(QString uid) const
{
  ToolPtr tool = toolManager()->getTool(uid);

  if (!tool && uid=="active")
    tool = toolManager()->getDominantTool();

  if(!tool)
  {
   messageManager()->sendWarning("Could not find tool with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
   return Transform3D::Identity();
  }
  return this->get_rMpr() * tool->get_prMt(); // ref_M_t
}

Transform3D CoordinateSystemHelpers::get_rMto(QString uid) const
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
  return this->get_rMpr() * tool->get_prMt() * tMto; // ref_M_to
}

Transform3D CoordinateSystemHelpers::get_rMs(QString uid) const
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

  Transform3D rMpr = this->get_rMpr();
  Transform3D prMt = tool->get_prMt();

  Transform3D rMs = rMpr * prMt * tMs;

  return rMs; //ref_M_s
}

} //namespace ssc
