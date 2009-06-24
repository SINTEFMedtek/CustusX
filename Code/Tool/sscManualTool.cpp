#include "sscManualTool.h"
#include <QTime>
#include <QTimer>
#include <vtkConeSource.h>

typedef vtkSmartPointer <class vtkConeSource> vtkConeSourcePtr;

namespace ssc
{

ManualTool::ManualTool(const std::string& uid) : mUid(uid), mName(uid), mMutex(QMutex::Recursive)
{
	mVisible = false; 	
	createPolyData();
}

ManualTool::~ManualTool()
{
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

void ManualTool::createPolyData()
{
	vtkConeSourcePtr coneSource = vtkConeSource::New();
	coneSource->SetResolution(25);
	coneSource->SetRadius(10);
	coneSource->SetHeight(500);
	coneSource->SetDirection(0, 0, 1);	
	double newCenter[3];
	coneSource->GetCenter(newCenter);
	newCenter[2] = newCenter[2] - coneSource->GetHeight()/2;
	coneSource->SetCenter( newCenter );
	mPolyData = coneSource->GetOutput();
}

ssc::Tool::Type ManualTool::getType() const
{
	return TOOL_MANUAL;
}

vtkPolyDataPtr ManualTool::getGraphicsPolyData() const
{
	return mPolyData;
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

}//end namespace
