#include "sscCoordinateSystemHelpers.h"

#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscData.h"

namespace ssc
{
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
  Transform3D rMfrom;

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
  default:

    break;
  };

  return rMfrom;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getS(ssc::ToolPtr tool)
{
  ssc::CoordinateSystem retval;
  if(!tool)
    return retval;

  retval.mId = ssc::csSENSOR;
  retval.mRefObject = tool->getUid();

  return retval;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getT(ssc::ToolPtr tool)
{
  ssc::CoordinateSystem retval;
  if(!tool)
    return retval;

  ssc::ToolPtr refTool = ssc::toolManager()->getReferenceTool();
  if(refTool && (tool == refTool))
  {
    retval.mId = ssc::csPATIENTREF;
  }
  else
    retval.mId = ssc::csTOOL;

  retval.mRefObject = tool->getUid();

  return retval;
}

ssc::CoordinateSystem CoordinateSystemHelpers::getD(ssc::DataPtr data)
{
  ssc::CoordinateSystem retval;
  if(!data)
    return retval;

  retval.mId = ssc::csDATA;
  retval.mRefObject = data->getUid();

  return retval;
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
  return Transform3D(); // ref_M_ref
}

Transform3D CoordinateSystemHelpers::get_rMd(QString uid) const
{
  DataPtr data = dataManager()->getData(uid);

  if(!data)
  {
   messageManager()->sendWarning("Could not find data with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
    return Transform3D();
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

  if(!tool)
  {
   messageManager()->sendWarning("Could not find tool with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
   return Transform3D();
  }
  return this->get_rMpr() * tool->get_prMt(); // ref_M_t
}

Transform3D CoordinateSystemHelpers::get_rMs(QString uid) const
{
  ToolPtr tool = toolManager()->getTool(uid);

  if(!tool)
  {
   messageManager()->sendWarning("Could not find tool with uid: "+uid+". Can not find transform to unknown coordinate system, returning identity!");
   return Transform3D();
  }

  Transform3D tMs = tool->getCalibration_sMt().inv();

  Transform3D rMpr = this->get_rMpr();
  Transform3D prMt = tool->get_prMt();

  Transform3D rMs = rMpr * prMt * tMs;

  return rMs; //ref_M_s
}

} //namespace ssc
