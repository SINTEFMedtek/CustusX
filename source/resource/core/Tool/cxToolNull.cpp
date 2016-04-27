#include "cxToolNull.h"

namespace cx
{

std::set<Tool::Type> ToolNull::getTypes() const
{
	return std::set<Type>();
}

vtkPolyDataPtr ToolNull::getGraphicsPolyData() const
{
	return vtkPolyDataPtr();
}

TimedTransformMapPtr ToolNull::getPositionHistory()
{
	return TimedTransformMapPtr();
}

ToolPositionMetadata ToolNull::getMetadata() const
{
	return ToolPositionMetadata();
}

const std::map<double, ToolPositionMetadata>& ToolNull::getMetadataHistory()
{
	return mMetadata;
}

bool ToolNull::getVisible() const
{
	return false;
}

bool ToolNull::isInitialized() const
{
	return false;
}

QString ToolNull::getUid() const
{
	return "";
}


QString ToolNull::getName() const
{
	return "";
}

bool ToolNull::isCalibrated() const
{
	return false;
}
Transform3D ToolNull::getCalibration_sMt() const
{
	return Transform3D::Identity();
}

void ToolNull::setCalibration_sMt(Transform3D calibration)
{
	Q_UNUSED(calibration);
}

ProbePtr ToolNull::getProbe() const
{
	return ProbePtr();
}

double ToolNull::getTimestamp() const
{
	return 0;
}

void ToolNull::printSelf(std::ostream &os, Indent indent)
{
	os << indent << "null tool" << std::endl;
}

double ToolNull::getTooltipOffset() const
{
	return 0;
}

void ToolNull::setTooltipOffset(double val)
{
	Q_UNUSED(val);
}

std::map<int, Vector3D> ToolNull::getReferencePoints() const
{
	return std::map<int, Vector3D>();
}

bool ToolNull::hasReferencePointWithId(int id)
{
	Q_UNUSED(id);
	return false;
}

TimedTransformMap ToolNull::getSessionHistory(double startTime, double stopTime)
{
	return TimedTransformMap();
}

Transform3D ToolNull::get_prMt() const
{
	return Transform3D::Identity();
}

void ToolNull::resetTrackingPositionFilter(TrackingPositionFilterPtr filter)
{
	Q_UNUSED(filter);
}

void ToolNull::set_prMt(const Transform3D& prMt, double timestamp)
{
	Q_UNUSED(prMt);
	Q_UNUSED(timestamp);
}

void ToolNull::setVisible(bool vis)
{
	Q_UNUSED(vis);
}

bool ToolNull::isNull()
{
	return true;
}


}
