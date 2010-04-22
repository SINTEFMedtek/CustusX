#include "sscManualTool.h"
#include <QTime>
#include <QTimer>
#include <vtkSTLReader.h>
#include <vtkCursor3D.h>
#include "sscDummyTool.h"


//typedef vtkSmartPointer <class vtkConeSource> vtkConeSourcePtr;

namespace ssc
{

ManualTool::ManualTool(const std::string& uid, const std::string& name) :
    Tool(uid,name), mMutex(QMutex::Recursive)
{
	mOffset = 0;
	mType = TOOL_MANUAL;
	mVisible = false;
	//mPolyData = ssc::DummyTool::createPolyData(140,10,10,3);
	read3DCrossHair();
}

ManualTool::~ManualTool()
{
}

void ManualTool::read3DCrossHair()
{
	if(mCrossHair)
	{
		return;
	}
	mCrossHair = vtkCursor3DPtr::New();
	mCrossHair->SetModelBounds(-120,140,-120,140,-120,140);
	mCrossHair->SetFocalPoint(0,0,0);
	mCrossHair->AllOff();
	mCrossHair->AxesOn();

	//mCrossHair->OutlineOn();
//	mCrossHair->XShadowsOn();
//	mCrossHair->YShadowsOn();
//	mCrossHair->ZShadowsOn();

	//std::string filename( "/Data/Models/3DCrosshair.STL");
//	mSTLReader = vtkSTLReaderPtr::New();
//	mSTLReader->SetFileName( filename.c_str() );
//	mSTLReader->Update();
//	if (mSTLReader->GetOutput())
//	{
//		std::cout << " we got polydata " << std::endl;
//		vtkPolyDataPtr mPolyData = mSTLReader->GetOutput();
//	}

	//mPolyDataMapper->SetInputConnection( mSTLReader->GetOutputPort() );


}
//only used now by mouse or touch tool
void ManualTool::set_prMt(const Transform3D& prMt)
{
	QDateTime time;
	double timestamp = (double) time.time().msec();

	QMutexLocker locker(&mMutex);
	m_prMt = prMt;
	locker.unlock();

	emit toolTransformAndTimestamp( prMt, timestamp );
}

std::string ManualTool::getGraphicsFileName() const
{
	return "";
}

ssc::Tool::Type ManualTool::getType() const
{
	return mType;
}

vtkPolyDataPtr ManualTool::getGraphicsPolyData() const
{
	//return mPolyData;
	return mCrossHair->GetOutput();
}

Transform3D ManualTool::get_prMt() const
{
	QMutexLocker locker(&mMutex);
	return m_prMt;
}

bool ManualTool::getVisible() const
{
	QMutexLocker locker(&mMutex);
	return mVisible;
}

int ManualTool::getIndex() const
{
	return -1;
}

std::string ManualTool::getUid() const
{
	return mUid;
}

std::string ManualTool::getName() const
{
	return mName;
}

void ManualTool::setVisible(bool vis)
{
	QMutexLocker locker(&mMutex);
	mVisible = vis;
}

void ManualTool::setType(const Type& type)
{
	QMutexLocker locker(&mMutex);
	mType = type;
}

bool ManualTool::isCalibrated() const
{
	return false;
}

ssc::ProbeSector ManualTool::getProbeSector() const
{
	return ssc::ProbeSector();
}

double ManualTool::getTimestamp() const
{
	return 0;
}

double ManualTool::getTooltipOffset() const
{
	return mOffset;
}

void ManualTool::setTooltipOffset(double val)
{
	if (similar(val,mOffset))
		return;
	mOffset = val;
	mCrossHair->SetFocalPoint(0,0,mOffset);
	emit tooltipOffset(mOffset);
}

}//end namespace
