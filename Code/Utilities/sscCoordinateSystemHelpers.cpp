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
  Transform3D to_M_from = helper.get_rMto(to).inv() * helper.get_rMto(from);
  return to_M_from;
}

Transform3D CoordinateSystemHelpers::get_rMto(CoordinateSystem to) const
{
  Transform3D rMto;

  switch(to.mId)
  {
  case csREF:
    rMto = this->get_rMr();
    break;
  case csDATA:
    rMto = this->get_rMd(to.mRefObject);
    break;
  case csPATIENTREF:
    rMto = this->get_rMpr();
    break;
  case csTOOL:
    rMto = this->get_rMt(to.mRefObject);
    break;
  case csSENSOR:
    rMto = this->get_rMs(to.mRefObject);
    break;
  default:

    break;
  };

  return rMto;
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
