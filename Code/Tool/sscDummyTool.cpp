#include "sscDummyTool.h"

#include <vtkPolyData.h>
#include <vtkConeSource.h>
#include <QTimer>
#include <QTime>

/**
 * sscTool.cpp
 *
 * \brief
 *
 * \date Oct 30, 2008
 * \author: jbake
 */

namespace ssc
{

int DummyTool::mTransformCount = 0;

DummyTool::DummyTool(const std::string& uid) :
	mVisible(false),
	mTransformSaveFileName("DummyToolsAreToDumbToSaveThemselves"),
	mTimer(new QTimer())
{
	mUid = uid;
	mName = uid;

	mTransforms = createToolPositionMovement();

	this->createPolyData();

	connect(mTimer.get(), SIGNAL(timeout()),this, SLOT(sendTransform()));
}

DummyTool::~DummyTool()
{
}

/**Use this to override the default movement pattern in the tool.
 * The pattern
 *
 */
void DummyTool::setToolPositionMovement(const std::vector<Transform3D>& positions)
{
	mTransforms = positions;
}

Tool::Type DummyTool::getType() const
{
	return TOOL_NONE;
}

std::string DummyTool::getGraphicsFileName() const
{
	//return "DummyToolsDoNotHaveAGraphicsFile";

	return "DummyToolsDoNotHaveAGraphicsFile";
}
vtkPolyDataPtr DummyTool::getGraphicsPolyData() const
{
	return mPolyData;
}
void DummyTool::saveTransformsAndTimestamps()
{
	std::cout << "DummyTools are to dumb to save themselves..." << std::endl;
}
void DummyTool::setTransformSaveFile(const std::string& filename)
{
	mTransformSaveFileName = filename;
}
Transform3D DummyTool::get_prMt() const
{
	return m_prMt;
}
bool DummyTool::getVisible() const
{
	return mVisible;
}
std::string DummyTool::getUid() const
{
	return mUid;
}
std::string DummyTool::getName() const
{
	return mName;
}
void DummyTool::startTracking()
{
	mTimer->start(33);

	mVisible = true;

	emit toolVisible(mVisible);
}
bool DummyTool::isCalibrated() const
{
	return true;
}
void DummyTool::stopTracking()
{
	mTimer->stop();

	mVisible = false;
	emit toolVisible(mVisible);
}
void DummyTool::setVisible(bool val)
{
	mVisible = val;
	emit toolVisible(mVisible);	
}
void DummyTool::sendTransform()
{
	set_prMt(*getNextTransform());
}
void DummyTool::createPolyData()
{
	vtkConeSourcePtr coneSource = vtkConeSource::New();

    coneSource->SetResolution(25);
    coneSource->SetRadius(10);
    coneSource->SetHeight(100);

    coneSource->SetDirection(0,0,1);
    double newCenter[3];
    coneSource->GetCenter(newCenter);
    newCenter[2] = newCenter[2] - coneSource->GetHeight()/2;
    coneSource->SetCenter(newCenter);

    mPolyData = coneSource->GetOutput();
}
std::vector<Transform3D> DummyTool::createToolPositionMovement() const
{
	//std::cout<<"createToolPositionMovement"<<std::endl;
    std::vector<Transform3D> retval;

    Transform3D r0 = createTransformRotateX(M_PI)*createTransformRotateZ(-M_PI*0.5);
    Transform3D r = createTransformRotateZ(-M_PI*0.25)*createTransformRotateX(-M_PI*0.25) * r0;
    Transform3D t = createTransformTranslate(Vector3D(-10,-11,0));

    for (unsigned i=0; i<40; ++i)
    {
        Transform3D t_delta = createTransformTranslate(Vector3D(2,0,0));
        t = t_delta * t;
        retval.push_back(t * r);
    }
    for (unsigned i=0; i<100; ++i)
    {
        Transform3D r_delta = createTransformRotateZ(-M_PI*0.01);
        r = r_delta * r;
        retval.push_back(t * r);
    }
    for (unsigned i=0; i<50; ++i)
    {
        Transform3D t_delta = createTransformTranslate(Vector3D(0,0,2));
        t = t_delta * t;
        retval.push_back(t * r);
    }
    for (unsigned i=0; i<100; ++i)
    {
        Transform3D r_delta = createTransformRotateZ(-M_PI*0.01);
        r = r_delta * r;
        retval.push_back(t * r);
    }
    return retval;
}
Transform3D* DummyTool::getNextTransform()
{
	if(mTransformCount >= int(mTransforms.size()))
		mTransformCount = 0;

	return &mTransforms.at(mTransformCount++);
}

void DummyTool::set_prMt(const Transform3D& prMt)
{
	QDateTime time;
	double timestamp = (double) time.time().msec();
	m_prMt = prMt;
	emit toolTransformAndTimestamp(m_prMt, timestamp);
}


}//namespace ssc
