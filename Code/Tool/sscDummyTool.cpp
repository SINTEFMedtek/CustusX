#include <boost/cstdint.hpp>
#include "sscDummyTool.h"

#include <vtkPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <QTimer>
#include <QTime>
#include <vtkPlane.h>
#include <vtkClipPolyData.h>
#include "sscToolManager.h"
#include "sscMessageManager.h"
#include "sscTimeKeeper.h"

namespace ssc
{

int DummyTool::mTransformCount = 0;




DummyTool::DummyTool(ToolManager *manager, const QString& uid) :
	mVisible(false),
	m_prMt(Transform3D::Identity()),
	mTransformSaveFileName("DummyToolsAreToDumbToSaveThemselves"),
	mTimer(new QTimer())
{
	mUid = uid;
	mName = uid;

	DoubleBoundingBox3D bb(Vector3D(0,0,0), Vector3D(512,512,256));
	setToolPositionMovementBB(bb);
//	std::cout << "dummytool" << std::endl;

//	this->createPolyData();
	mPolyData = this->createPolyData(150, 15, 4, 2);

	connect(mTimer.get(), SIGNAL(timeout()),this, SLOT(sendTransform()));
	connect(manager, SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
}

DummyTool::~DummyTool()
{
}

std::set<Tool::Type> DummyTool::getTypes() const
{
	return mTypes;
}

void DummyTool::setToolPositionMovementBB(const DoubleBoundingBox3D& bb)
{
	setToolPositionMovement(createToolPositionMovement(bb));
}

std::vector<Transform3D> DummyTool::getToolPositionMovement()
{
	return mTransforms;
}

/**Use this to override the default movement pattern in the tool.
 * The pattern
 *
 */
void DummyTool::setToolPositionMovement(const std::vector<Transform3D>& positions)
{
	mTransforms = positions;
}

void DummyTool::setType(Tool::Type type)
{
	mTypes.clear();
	mTypes.insert(type);
}

QString DummyTool::getGraphicsFileName() const
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
void DummyTool::setTransformSaveFile(const QString& filename)
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
QString DummyTool::getUid() const
{
	return mUid;
}
QString DummyTool::getName() const
{
	return mName;
}
void DummyTool::startTracking(int interval)
{
	mThread = new DummyToolThread(interval);
	connect(mThread, SIGNAL(ping()), this, SLOT(sendTransform()));
	mThread->start();


//	mTimer->start(interval);
//std::cout << "start tracking" << std::endl;
	mVisible = true;

	emit toolVisible(mVisible);
}
bool DummyTool::isCalibrated() const
{
	return true;
}
void DummyTool::stopTracking()
{
	disconnect(mThread, SIGNAL(ping()), this, SLOT(sendTransform()));
	mThread->quit();
//	mTimer->stop();
	std::cout << "stop tracking" << std::endl;

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

/** Create a dummy 3D representation with a butt tip
 * Input is the height of the two segments, h1 and h2,
 * and the radius of the two segmens, r1 and r2.
 * Typical values is given in the class constructor (150,10,10,3).
 */
vtkPolyDataPtr DummyTool::createPolyData(double h1, double h2, double r1, double r2)
{

//	double r1 = 10;
//	double r2 = 3;
//	double h1 = 140;
//	double h2 =  10;

	vtkAppendPolyDataPtr assembly = vtkAppendPolyDataPtr::New(); 

	vtkPlanePtr plane = vtkPlanePtr::New();
	plane->SetNormal(0,0,-1);
	plane->SetOrigin(0,0,-h2);

	vtkConeSourcePtr cone1 = vtkConeSourcePtr::New();
	double h1_extension = h1*r2 / (r1-r2); 
	double h1_mod = h1+h1_extension;
	cone1->SetResolution(50);
	cone1->SetRadius(r1);
	cone1->SetHeight(h1_mod);
	cone1->SetDirection(0,0,1);
	double center1 = -h1/2-h2+h1_extension/2;
	cone1->SetCenter(Vector3D(0,0,center1).begin());

	vtkClipPolyDataPtr clipper1 = vtkClipPolyDataPtr::New();
	clipper1->SetInput(cone1->GetOutput());
	clipper1->SetClipFunction(plane);    

	vtkConeSourcePtr cone2 = vtkConeSourcePtr::New();
	cone2->SetResolution(25);
	cone2->SetRadius(r2);
	cone2->SetHeight(h2);
	cone2->SetDirection(0,0,1);
	double center2 = -h2/2;
	cone2->SetCenter(Vector3D(0,0,center2).begin());

	assembly->AddInput(clipper1->GetOutput());
	assembly->AddInput(cone2->GetOutput());
//    mPolyData = assembly->GetOutput();
	return assembly->GetOutput();
}

void DummyTool::createLinearMovement(std::vector<Transform3D>* retval, Transform3D* T_in, const Transform3D& R, const Vector3D& a, const Vector3D& b, double step) const
{
	Vector3D u = (b-a).normal();
	//No round in Windows
	//unsigned N = (unsigned)round((b-a).length()/step);
	unsigned N = (unsigned)floor((b-a).length()/step + 0.5);
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

/** Create a test trajectory that moves within the given bb.
 */
std::vector<Transform3D> DummyTool::createToolPositionMovementTranslationOnly(const DoubleBoundingBox3D& bb) const
{
	std::vector<Transform3D> retval;

	Vector3D range = bb.range();
	// define four points. Traverse them and then back to the starting point.
//    Vector3D a = bb.center() + Vector3D(range[0]/2, range[0]/10, range[0]/10);
//    Vector3D b = bb.center();
//    Vector3D c = b + Vector3D(-range[0]*0.1, -range[0]*0.1, -range[0]*0.1);
//    Vector3D d = c + Vector3D(range[0]*0.1, range[0]*0.1, range[2]/3);
	Vector3D a = bb.center() + Vector3D( range[0]*0.4,  range[1]*0.4,  range[2]*0.4);
	Vector3D b = bb.center();
	Vector3D c = bb.center() + Vector3D(-range[0]*0.4, -range[1]*0.1, -range[2]*0.1);
	Vector3D d = bb.center() + Vector3D( range[0]*0.0,  range[1]*0.1,  range[2]*0.3);

	int steps = 200;
	double step = *std::max_element(range.begin(), range.end()) / steps;

	Transform3D r0 = createTransformRotateX(M_PI)*createTransformRotateZ(-M_PI*0.5);
	Transform3D R = createTransformRotateZ(-M_PI*0.25)*createTransformRotateX(-M_PI*0.25) * r0;
	Transform3D T = createTransformTranslate(a);

	createLinearMovement(&retval, &T, R, a, b, step);
	createLinearMovement(&retval, &T, R, b, c, step);
	createLinearMovement(&retval, &T, R, c, d, step);
	createLinearMovement(&retval, &T, R, d, a, step);

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
//	TimeKeeper timer;
//	static int count=0;

	m_prMt = prMt;

	// use ms since Epoch (AD1970)
	QDateTime time = QDateTime::currentDateTime();
	boost::uint64_t ts = (boost::uint64_t)(time.toTime_t())*1000 + time.time().msec();
	double timestamp = static_cast<double>(ts);

	//check:
//	std::cout << "check: " << time.toString("yyyyMMdd'T'hhmmss:zzz").toStdString() << std::endl;
//	std::cout << "DummyTool::set_prMt " << QDateTime::currentDateTime().toString("mm:ss:zzz").toStdString() << std::endl;
//	ssc::messageManager()->sendDebug("DummyTool:: emit toolTransformAndTimestamp()");

//	std::cout << "start emit tool pos" << std::endl;

	emit toolTransformAndTimestamp(m_prMt, timestamp);

//	if (++count%300==0)
//		timer.printElapsedms("emit tool pos");
}

double DummyTool::getTooltipOffset() const 
{
	return toolManager()->getTooltipOffset();
}

void DummyTool::setTooltipOffset(double val) 
{ 
	toolManager()->setTooltipOffset(val);
}

ssc::Transform3D DummyTool::getCalibration_sMt() const
{
	return createTransformTranslate(Vector3D(5,5,20));
}


}//namespace ssc
