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

	DoubleBoundingBox3D bb(Vector3D(0,0,0), Vector3D(512,512,256));
	setToolPositionMovementBB(bb);

	this->createPolyData();

	connect(mTimer.get(), SIGNAL(timeout()),this, SLOT(sendTransform()));
}

DummyTool::~DummyTool()
{
}

void DummyTool::setToolPositionMovementBB(const DoubleBoundingBox3D& bb)
{
	setToolPositionMovement(createToolPositionMovement(bb));
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
void DummyTool::startTracking(int interval)
{
	mTimer->start(interval);

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
    coneSource->SetHeight(300);

    coneSource->SetDirection(0,0,1);
    double newCenter[3];
    coneSource->GetCenter(newCenter);
    newCenter[2] = newCenter[2] - coneSource->GetHeight()/2;
    coneSource->SetCenter(newCenter);

    mPolyData = coneSource->GetOutput();
}

void DummyTool::createLinearMovement(std::vector<Transform3D>* retval, Transform3D* T_in, const Transform3D& R, const Vector3D& a, const Vector3D& b, double step) const
{
	Vector3D u = (b-a).normal();
	unsigned N = (unsigned)round((b-a).length()/step);
	Transform3D& T = *T_in;
	
	for (unsigned i=0; i<N; ++i)
	{
        Transform3D T_delta = createTransformTranslate(u*step);
        T = T_delta * T;
        retval->push_back(T * R);		
	}
}

/** Create a test trajectory that moves within the given bb.
 */
std::vector<Transform3D> DummyTool::createToolPositionMovement(const DoubleBoundingBox3D& bb) const
{	
//	std::cout<<"createToolPositionMovement:"<<bb<<std::endl;
    std::vector<Transform3D> retval;
       
    Vector3D range = bb.range();
    // define four points. Traverse them and then back to the starting point.
    Vector3D a = bb.center() + Vector3D(range[0]/2, 0, 0);
    Vector3D b = bb.center();
    Vector3D c = b + Vector3D(0, -range[0]*0.1, 0);
    Vector3D d = c + Vector3D(0, 0, range[2]/2);

//    Vector3D a = bb.corner(0,0,0);
//    Vector3D b = bb.corner(1,0,0);
//    Vector3D c = bb.corner(1,1,0);
//    Vector3D d = bb.corner(1,1,1);
       
//    std::cout << "a" << a << std::endl;
//    std::cout << "b" << b << std::endl;
//    std::cout << "c" << c << std::endl;
//    std::cout << "d" << d << std::endl;

    int steps = 200;
    double step = *std::max_element(range.begin(), range.end()) / steps;

    Transform3D r0 = createTransformRotateX(M_PI)*createTransformRotateZ(-M_PI*0.5);
    Transform3D R = createTransformRotateZ(-M_PI*0.25)*createTransformRotateX(-M_PI*0.25) * r0;
    Transform3D T = createTransformTranslate(a);  
    
    createLinearMovement(&retval, &T, R, a, b, step);    

    for (unsigned i=0; i<50; ++i)
    {
        Transform3D r_delta = createTransformRotateZ(-M_PI*0.01);
        R = r_delta * R;
        retval.push_back(T * R);
    }
    
    createLinearMovement(&retval, &T, R, b, c, step);    

    for (unsigned i=0; i<50; ++i)
    {
        Transform3D r_delta = createTransformRotateZ(-M_PI*0.01);
        R = r_delta * R;
        retval.push_back(T * R);
    }

    createLinearMovement(&retval, &T, R, c, d, step);    
    createLinearMovement(&retval, &T, R, d, a, step);    

    for (unsigned i=0; i<20; ++i)
    {
        Transform3D r_delta = createTransformRotateZ(-M_PI/20);
        R = r_delta * R;
        retval.push_back(T * R);
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
