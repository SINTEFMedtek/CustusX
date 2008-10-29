#include "toolToolImpl.h"
#include "SonoWand.h"

// --------------------------------------------------------
namespace tool
{
// --------------------------------------------------------

ToolImpl::ToolImpl(std::string uid, ThreadSafeDataPtr data) :
	mUid(uid), mData(data)
{

}

ToolImpl::~ToolImpl()
{

}

ssc::Tool::Type ToolImpl::getType() const
{
	int id = mData->id(mUid);
	switch (id)
	{
	case 0:
		return TOOL_REFERENCE;
	case 3:
	case 6:
		return TOOL_POINTER;
	case 1:
	case 2:
		return TOOL_US_PROBE;
	default:
		return TOOL_NONE;
	}
}

//enum TOOL_TYPE
//{
//	ttNO_TOOL = -1,
//	ttUS_5S_PROBE = 1,
//	ttUS_LINEAR_PROBE = 2,
//	ttPLANNING_INTRA = 3,
//	ttPLANNING_NAV = 6,
//	ttCONE
//};

std::string ToolImpl::getGraphicsFileName() const
{
	int id = mData->id(mUid);
	std::string base = SW_Path(kApplicationData);
	switch (id)
	{
	case 3:
		return base+"/Models/Intra_operativ_navigator.STL";
	case 6:
		return base+"/Models/Planning_navigator.STL";
	case 1:
	case 2:
		return base+"/Models/5S_probeMSector.STL";
	default:
		return "";
	}
}

vtkPolyDataPtr ToolImpl::getGraphicsPolyData() const
{
	return vtkPolyDataPtr();
}

void ToolImpl::saveTransformsAndTimestamps()
{

}

void ToolImpl::setTransformSaveFile(const std::string& filename)
{

}

Transform3D ToolImpl::getTransform() const
{
	return mData->position(mUid);
}

bool ToolImpl::getVisible() const
{
	return mData->visible(mUid);
}

std::string ToolImpl::getUid() const
{
	return mUid;
}

std::string ToolImpl::getName() const
{
	return mUid;
}

void ToolImpl::invokeTransformEvent()
{
	TransformAndTimestampEventArgument arg;
	arg.matrix = getTransform();
	arg.timestamp = 0;
	this->InvokeEvent(TOOL_TRANSFORM_AND_TIMESTAMP_EVENT, &arg);
}

void ToolImpl::invokeVisibleEvent()
{
	VisibleEventArgument arg;
	arg.visible = getVisible();
	this->InvokeEvent(TOOL_VISIBLE_EVENT, &arg);
}

// --------------------------------------------------------
} // namespace tool
// --------------------------------------------------------

