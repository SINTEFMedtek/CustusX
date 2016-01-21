#include "cxRobotTool.h"
#include "cxProbeImpl.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include <Qdir>
#include "cxUr5State.h"

#include <vtkActor.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSet.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkAppendPolyData.h>
#include <vtkMatrix4x4.h>
#include "cxTransform3D.h"
#include <vtkAssembly.h>
#include "cxVisServices.h"
#include "cxViewService.h"
#include "cxView.h"
#include <vtkRenderer.h>
#include "cxUr5Kinematics.h"

namespace cx
{

RobotTool::RobotTool(QString uid, Ur5RobotPtr robot):
    ToolImpl(uid,uid),
    mPolyData(NULL),
    mUr5Robot(robot),
    mTimestamp(0)
{
    mTypes = this->determineTypesBasedOnUid(Tool::mUid);

    this->createPolyData();
    this->toolVisibleSlot(true);
}

RobotTool::RobotTool(QString uid, Ur5RobotPtr robot, VisServicesPtr services):
    ToolImpl(uid,uid),
    mPolyData(NULL),
    mUr5Robot(robot),
    mServices(services),
    mTimestamp(0)
{
    mTypes = this->determineTypesBasedOnUid(Tool::mUid);

    this->createPolyData();
    this->toolVisibleSlot(true);

    this->set_prMb_calibration();
}

RobotTool::~RobotTool()
{

}

std::set<Tool::Type> RobotTool::determineTypesBasedOnUid(const QString uid) const
{
    std::set<Type> retval;
    retval.insert(TOOL_POINTER);
    return retval;
}



std::set<Tool::Type> RobotTool::getTypes() const
{
    return mTypes;
}

vtkPolyDataPtr RobotTool::getGraphicsPolyData() const
{
    return mPolyData;
}

Transform3D RobotTool::get_prMt() const
{
    return m_prMt;
}

bool RobotTool::getVisible() const
{
    return true;
}

QString RobotTool::getUid() const
{
    return Tool::mUid;
}

QString RobotTool::getName() const
{
    return Tool::mName;
}

bool RobotTool::isCalibrated() const
{
    return true;
}

double RobotTool::getTimestamp() const
{
    return mTimestamp;
}

// Just use the tool tip offset from the tool manager
double RobotTool::getTooltipOffset() const
{
    return ToolImpl::getTooltipOffset();
}

// Just use the tool tip offset from the tool manager
void RobotTool::setTooltipOffset(double val)
{
    ToolImpl::setTooltipOffset(val);
}

Transform3D RobotTool::getCalibration_sMt() const
{
    return m_sMt_calibration;
}

bool RobotTool::isInitialized() const
{
    return true;
}

void RobotTool::setVisible(bool vis)
{
    CX_LOG_WARNING() << "Cannot set visible on a openigtlink tool.";
}


void RobotTool::toolTransformAndTimestampSlot(Transform3D prMs, double timestamp)
{
    mTimestamp = timestamp;// /1000000;
    Transform3D prMt = prMs;
    Transform3D prMt_filtered = prMt;

    (*mPositionHistory)[mTimestamp] = prMt; // store original in history
    m_prMt = prMt_filtered;
    emit toolTransformAndTimestamp(m_prMt, mTimestamp);
}

void RobotTool::calculateTpsSlot()
{
}

void RobotTool::createPolyData()
{
    mPolyData = Tool::createDefaultPolyDataCone();
}

void RobotTool::toolVisibleSlot(bool on)
{
//    if (on)
//        mTpsTimer.start(1000); //calculate tps every 1 seconds
//    else
//        mTpsTimer.stop();
}

void RobotTool::initiateActors()
{
    vtkSTLReaderPtr baseSTL = vtkSTLReaderPtr::New();
    baseSTL->SetFileName(cstring_cast(QString(mGraphicsFolderName + "base.stl")));
    vtkPolyDataMapperPtr baseMapper = vtkPolyDataMapperPtr::New();
    baseMapper->SetInputConnection(baseSTL->GetOutputPort());

    baseActor = vtkActorPtr::New();
    baseActor->SetMapper(baseMapper);

    vtkSTLReaderPtr link1STL = vtkSTLReaderPtr::New();
    link1STL->SetFileName(cstring_cast(QString(mGraphicsFolderName + "link1.stl")));
    vtkPolyDataMapperPtr link1Mapper = vtkPolyDataMapperPtr::New();
    link1Mapper->SetInputConnection(link1STL->GetOutputPort());

    link1Actor = vtkActorPtr::New();
    link1Actor->SetMapper(link1Mapper);

    vtkSTLReaderPtr link2STL = vtkSTLReaderPtr::New();
    link2STL->SetFileName(cstring_cast(QString(mGraphicsFolderName + "link2.stl")));
    vtkPolyDataMapperPtr link2Mapper = vtkPolyDataMapperPtr::New();
    link2Mapper->SetInputConnection(link2STL->GetOutputPort());

    link2Actor = vtkActorPtr::New();
    link2Actor->SetMapper(link2Mapper);

    vtkSTLReaderPtr link3STL = vtkSTLReaderPtr::New();
    link3STL->SetFileName(cstring_cast(QString(mGraphicsFolderName + "link3.stl")));
    vtkPolyDataMapperPtr link3Mapper = vtkPolyDataMapperPtr::New();
    link3Mapper->SetInputConnection(link3STL->GetOutputPort());

    link3Actor = vtkActorPtr::New();
    link3Actor->SetMapper(link3Mapper);

    vtkSTLReaderPtr link4STL = vtkSTLReaderPtr::New();
    link4STL->SetFileName(cstring_cast(QString(mGraphicsFolderName + "link4.stl")));
    vtkPolyDataMapperPtr link4Mapper = vtkPolyDataMapperPtr::New();
    link4Mapper->SetInputConnection(link4STL->GetOutputPort());

    link4Actor = vtkActorPtr::New();
    link4Actor->SetMapper(link4Mapper);

    vtkSTLReaderPtr link5STL = vtkSTLReaderPtr::New();
    link5STL->SetFileName(cstring_cast(QString(mGraphicsFolderName + "link5.stl")));
    vtkPolyDataMapperPtr link5Mapper = vtkPolyDataMapperPtr::New();
    link5Mapper->SetInputConnection(link5STL->GetOutputPort());

    link5Actor = vtkActorPtr::New();
    link5Actor->SetMapper(link5Mapper);

    vtkSTLReaderPtr eeSTL = vtkSTLReaderPtr::New();
    eeSTL->SetFileName(cstring_cast(QString(mGraphicsFolderName + "ee.stl")));
    vtkPolyDataMapperPtr eeMapper = vtkPolyDataMapperPtr::New();
    eeMapper->SetInputConnection(eeSTL->GetOutputPort());

    eeActor = vtkActorPtr::New();
    eeActor->SetMapper(eeMapper);
}

void RobotTool::addRobotActors()
{
    Ur5Kinematics kinematic;

    ViewPtr view = mServices->view()->get3DView();


    baseActor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&this->prMb));
    link1Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(this->prMb*kinematic.T01(mUr5Robot->getCurrentState().jointPosition))));
    link2Actor->SetPosition(0,0,130);
    link2Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(this->prMb*kinematic.T01(mUr5Robot->getCurrentState().jointPosition))));
    link3Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(this->prMb*kinematic.T02(mUr5Robot->getCurrentState().jointPosition))));
    link4Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(this->prMb*kinematic.T04(mUr5Robot->getCurrentState().jointPosition))));
    link5Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(this->prMb*kinematic.T05(mUr5Robot->getCurrentState().jointPosition))));

    //std::cout << baseActor->GetCenter()[0] << " " <<  baseActor->GetCenter()[1] << " " <<  baseActor->GetCenter()[2] << std::endl;
    //std::cout << baseActor->GetPosition()[0] << " " <<  baseActor->GetPosition()[1] << " " <<  baseActor->GetPosition()[2] << std::endl;
    //std::cout << baseActor->GetOrigin()[0] << " " <<  baseActor->GetOrigin()[1] << " " <<  baseActor->GetOrigin()[2] << std::endl;

    view->getRenderer()->AddActor(link1Actor);
    view->getRenderer()->AddActor(link2Actor);
    view->getRenderer()->AddActor(link3Actor);
    view->getRenderer()->AddActor(link4Actor);
    view->getRenderer()->AddActor(link5Actor);
    view->getRenderer()->AddActor(baseActor);
}
void RobotTool::set_prMb_calibration()
{
    prMb = Transform3D::Identity();
    //prMb = createTransformRotateZ(1.57);
    //prMb(0,3) = 35;
    //prMb(1,3) = 300;
    //prMb(2,3) = 0;
}



} // namespace cx
