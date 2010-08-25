#include "sscManualTool.h"
#include <QTime>
#include <QTimer>
#include <vtkSTLReader.h>
#include <vtkCursor3D.h>
#include "sscDummyTool.h"
#include "sscMessageManager.h"


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
	if(!mCrossHair)
	{
	  mCrossHair = vtkCursor3DPtr::New();
	  mCrossHair->AllOff();
	  mCrossHair->AxesOn();
	}
	int s = 60;
	//mCrossHair->SetModelBounds(-120,140,-120,140,-120,140);
	mCrossHair->SetModelBounds(-s,s,-s,s,-s,s+mOffset);
  mCrossHair->SetFocalPoint(0,0,mOffset);
}

//only used now by mouse or touch tool
//copied into cx::Tool, move to ssc::Tool?
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
	//return ssc::ProbeSector();
  
  //Test code 
  double depthStart = 10;
  double depthEnd = 100;
  double width = 50;
  //ssc::messageManager()->sendDebug("Use ManualTool::getProbeSector() test data");
  return ssc::ProbeSector(ssc::ProbeSector::tLINEAR, depthStart, depthEnd, width);
  
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
	read3DCrossHair();
//	mCrossHair->SetFocalPoint(0,0,mOffset);
	emit tooltipOffset(mOffset);
}

}//end namespace
