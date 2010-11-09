#include "sscManualTool.h"
#include <QTime>
#include <QTimer>
#include <vtkSTLReader.h>
#include <vtkCursor3D.h>
#include "sscDummyTool.h"
#include "sscMessageManager.h"

namespace ssc
{

ManualTool::ManualTool(const QString& uid, const QString& name) :
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
//  std::cout << "manual tool set " << prMt << std::endl;

	QMutexLocker locker(&mMutex);
	m_prMt = prMt;
	locker.unlock();

	emit toolTransformAndTimestamp( prMt, timestamp );
}

QString ManualTool::getGraphicsFileName() const
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

QString ManualTool::getUid() const
{
	return mUid;
}

QString ManualTool::getName() const
{
	return mName;
}

void ManualTool::setVisible(bool vis)
{
	QMutexLocker locker(&mMutex);
	if (mVisible==vis)
	  return;
	mVisible = vis;
	emit toolVisible(mVisible);
	//std::cout << "ManualTool::setVisible( " << vis << " )" << std::endl;
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
	read3DCrossHair();
//	mCrossHair->SetFocalPoint(0,0,mOffset);
	emit tooltipOffset(mOffset);
}

ssc::Transform3D ManualTool::getCalibration_sMt() const
{
//	Transform3D Rz30 = createTransformRotateZ(M_PI/6);
//	Transform3D Rz60 = createTransformRotateZ(2.0*M_PI/6);
//	Transform3D Rx90 = createTransformRotateX(M_PI/2);
//	Transform3D Ry90 = createTransformRotateY(M_PI/2);
//	Transform3D Rz90 = createTransformRotateZ(M_PI/2);
//	Transform3D T = createTransformTranslate(Vector3D(10,10,10));
//	Transform3D sMt = (Rz30*Rx90*Ry90);
//	std::cout << "sMt\n" << sMt << std::endl;
//	return sMt*T;

	//return createTransformTranslate(Vector3D(5,5,5));
	return Transform3D();
}


}//end namespace
