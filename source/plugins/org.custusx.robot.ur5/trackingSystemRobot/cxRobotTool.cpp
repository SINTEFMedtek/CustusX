#include "cxRobotTool.h"
#include "cxProbeImpl.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include <QDir>
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

#include "cxSpaceProvider.h"
#include "cxLandmark.h"

namespace cx
{

RobotTool::RobotTool(QString uid, Ur5RobotPtr robot, VisServicesPtr services):
    ToolImpl(uid,uid),
    mPolyData(NULL),
    mUr5Robot(robot),
    mServices(services),
    mTimestamp(0),
    eMt(Transform3D::Identity()),
    prMb(Transform3D::Identity()),
    isRobotLinksVisualized(false)
{
    mTypes = this->determineTypesBasedOnUid(Tool::mUid);

    this->createPolyData();
    this->toolVisibleSlot(true);

    this->set_prMb_calibration();
    this->set_eMt_calibration();
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
}


void RobotTool::toolTransformAndTimestampSlot(Transform3D bMe, double timestamp)
{
    mTimestamp = timestamp;// /1000000;

    m_prMt = this->prMb*bMe;

    (*mPositionHistory)[mTimestamp] = bMe; // store original in history

    emit toolTransformAndTimestamp(m_prMt, mTimestamp);

    if(this->isRobotLinksVisualized)
        this->updateActors();
}

void RobotTool::calculateTpsSlot()
{
}

void RobotTool::createPolyData()
{
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    mGraphicsFolderName = dir.path()+mGraphicsFolderName;

    if (!this->mGraphicsFolderName.isEmpty() && dir.exists(this->mGraphicsFolderName))
    {
        this->initiateActors();

        vtkSTLReaderPtr eeSTL = vtkSTLReaderPtr::New();

        eeSTL->SetFileName(cstring_cast(QString(mGraphicsFolderName + "ee.stl")));
        eeSTL->Update();

        mPolyData = eeSTL->GetOutput();
    }
    else
    {
        mPolyData = Tool::createDefaultPolyDataCone();
    }
}

void RobotTool::toolVisibleSlot(bool on)
{
}

void RobotTool::initiateActors()
{
    baseActor = this->vtkSourceToActor("base.stl");
    link1Actor = this->vtkSourceToActor("link1.stl");
    link2Actor = this->vtkSourceToActor("link2.stl");
    link3Actor = this->vtkSourceToActor("link3.stl");
    link4Actor = this->vtkSourceToActor("link4.stl");
    link5Actor = this->vtkSourceToActor("link5.stl");
    eeActor = this->vtkSourceToActor("ee.stl");
}

void RobotTool::addRobotActors()
{
    ViewPtr view = mServices->view()->get3DView();

    Transform3D rMb = this->get_rMb();
    Transform3D rMl1 = this->get_rMb()*Ur5Kinematics::T01(mUr5Robot->getCurrentState().jointConfiguration);
    Transform3D rMl2 = this->get_rMb()*Ur5Kinematics::T01(mUr5Robot->getCurrentState().jointConfiguration);
    Transform3D rMl3 = this->get_rMb()*Ur5Kinematics::T02(mUr5Robot->getCurrentState().jointConfiguration);
    Transform3D rMl4 = this->get_rMb()*Ur5Kinematics::T04(mUr5Robot->getCurrentState().jointConfiguration);
    Transform3D rMl5 = this->get_rMb()*Ur5Kinematics::T05(mUr5Robot->getCurrentState().jointConfiguration);

    baseActor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMb)));
    link1Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMl1)));
    link2Actor->SetPosition(0,0,121);
    link2Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMl2)));
    link3Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMl3)));
    link4Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMl4)));
    link5Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMl5)));

    view->getRenderer()->AddActor(link1Actor);
    view->getRenderer()->AddActor(link2Actor);
    view->getRenderer()->AddActor(link3Actor);
    view->getRenderer()->AddActor(link4Actor);
    view->getRenderer()->AddActor(link5Actor);
    view->getRenderer()->AddActor(baseActor);

    this->isRobotLinksVisualized = true;
}

void RobotTool::updateActors()
{
    Transform3D rMb = this->get_rMb();
    Transform3D rMl1 = this->get_rMb()*Ur5Kinematics::T01(mUr5Robot->getCurrentState().jointConfiguration);
    Transform3D rMl2 = this->get_rMb()*Ur5Kinematics::T01(mUr5Robot->getCurrentState().jointConfiguration);
    Transform3D rMl3 = this->get_rMb()*Ur5Kinematics::T02(mUr5Robot->getCurrentState().jointConfiguration);
    Transform3D rMl4 = this->get_rMb()*Ur5Kinematics::T04(mUr5Robot->getCurrentState().jointConfiguration);
    Transform3D rMl5 = this->get_rMb()*Ur5Kinematics::T05(mUr5Robot->getCurrentState().jointConfiguration);

    baseActor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMb)));
    link1Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMl1)));
    link2Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMl2)));
    link2Actor->SetOrientation(0,0,mUr5Robot->getCurrentState().jointConfiguration(1)*180/M_PI+90);
    link3Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMl3)));
    link3Actor->SetOrientation(0,0,mUr5Robot->getCurrentState().jointConfiguration(2)*180/M_PI);
    link4Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMl4)));
    link5Actor->SetUserTransform(cx_transform3D_internal::getVtkTransform(&(rMl5)));
}

void RobotTool::set_prMb_calibration()
{
    prMb = createTransformRotateZ(M_PI/2);
    prMb(0,3) = 35;
    prMb(1,3) = 283;
    prMb(2,3) = -23;
}

void RobotTool::set_eMt_calibration()
{
    eMt(0,3) = 133;
    eMt(1,3) = 24;
    eMt(2,3) = 102;
}

void RobotTool::removeActors()
{
    ViewPtr view = mServices->view()->get3DView();

    view->getRenderer()->RemoveActor(link1Actor);
    view->getRenderer()->RemoveActor(link2Actor);
    view->getRenderer()->RemoveActor(link3Actor);
    view->getRenderer()->RemoveActor(link4Actor);
    view->getRenderer()->RemoveActor(link5Actor);
    view->getRenderer()->RemoveActor(baseActor);

    this->isRobotLinksVisualized = false;
}

Transform3D RobotTool::get_rMb()
{
    return (mServices->patient()->get_rMpr()*this->prMb);
}

vtkActorPtr RobotTool::vtkSourceToActor(QString filename)
{
    vtkSTLReaderPtr source = vtkSTLReaderPtr::New();
    source->SetFileName(cstring_cast(QString(mGraphicsFolderName + filename)));

    vtkPolyDataMapperPtr mapper = vtkPolyDataMapperPtr::New();
    mapper->SetInputConnection(source->GetOutputPort());

    vtkActorPtr actor = vtkActorPtr::New();
    actor->SetMapper(mapper);

    return actor;
}

Transform3D RobotTool::get_prMb()
{
    return (this->prMb);
}

Transform3D RobotTool::get_eMt()
{
    return (this->eMt);
}


} // namespace cx
