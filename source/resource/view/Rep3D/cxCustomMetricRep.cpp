/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxCustomMetricRep.h"

#include "cxView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <QFileInfo>
#include "cxTypeConversions.h"
#include "vtkTextActor.h"
#include "cxGraphicalPrimitives.h"
#include "cxCustomMetric.h"
#include "cxGraphicalPrimitives.h"
#include "vtkMatrix4x4.h"
#include "vtkSTLReader.h"
#include <vtkPolyDataNormals.h>


namespace cx
{

CustomMetricRepPtr CustomMetricRep::New(const QString& uid)
{
    return wrap_new(new CustomMetricRep(), uid);
}

CustomMetricRep::CustomMetricRep()
{
}

void CustomMetricRep::clear()
{
	DataMetricRep::clear();
    mSTLModel.reset();
}

CustomMetricPtr CustomMetricRep::getCustomMetric()
{
    return boost::dynamic_pointer_cast<CustomMetric>(mMetric);
}

void CustomMetricRep::onModifiedStartRender()
{
	if (!mMetric)
		return;

    this->updateSTLModel();
	this->drawText();
}

void CustomMetricRep::updateSTLModel()
{
    if (!mMetric)
        return;

    CustomMetricPtr custom = this->getCustomMetric();

    if (!mSTLModel && this->getView() && mMetric)
    {
        mSTLModel.reset(new GraphicalObjectWithDirection());
        mSTLModel->setRenderer(this->getRenderer());
    }

    if(!mSTLModel)
        return;

    QString filename = custom.get()->getSTLFile();

    if (filename.isEmpty() || !QFileInfo(filename).exists() || QFileInfo(filename).isDir())
    {
        //std::out << "STL file not found in: " << filename << "\n";
        //return this->createCube();
        return;
    }

    vtkSTLReaderPtr STLReader = vtkSTLReaderPtr::New();
    STLReader->SetFileName(cstring_cast(filename));

    vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
    normals->SetInputConnection(STLReader->GetOutputPort());
    normals->Update();

    vtkMapperPtr polyDataMapper = mSTLModel->getMapper();
    polyDataMapper->SetInputConnection(normals->GetOutputPort()); //read a 3D model file of the tool
    polyDataMapper->Update();

    vtkActorPtr actor = mSTLModel->getActor();
    actor->SetMapper(polyDataMapper);
    actor->GetProperty()->SetColor(0.5, 1, 1);
    actor->GetProperty()->SetSpecularPower(15);
    actor->GetProperty()->SetSpecular(0.3);

    mSTLModel->setPosition(custom->getPosition());
    mSTLModel->setDirection(custom->getDirection());
    mSTLModel->setVectorUp(custom->getVectorUp());
}

}
