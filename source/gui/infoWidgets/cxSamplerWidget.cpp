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
#include "cxSamplerWidget.h"

#include "cxCoordinateSystemHelpers.h"
#include <vtkImageData.h>
#include "cxToolManager.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxTypeConversions.h"
#include "cxSettings.h"
#include "cxDataManager.h"
#include "cxImage.h"
#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"
#include "cxSpaceListener.h"

namespace cx
{

SamplerWidget::SamplerWidget(QWidget* parent) :
  BaseWidget(parent, "SamplerWidget", "Point Sampler")
{
	mListener = spaceProvider()->createListener();
	mListener->setSpace(Space::reference());
//	mListener.reset(new CoordinateSystemListener(Space(csREF)));
	connect(mListener.get(), SIGNAL(changed()), this, SLOT(setModified()));

	mActiveTool = DominantToolProxy::New(trackingService());
	connect(mActiveTool.get(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(setModified()));
	connect(mActiveTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), SLOT(setModified()));
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(spacesChangedSlot()));
	connect(toolManager(), SIGNAL(configured()), this, SLOT(spacesChangedSlot()));

	mLayout = new QHBoxLayout(this);
	mLayout->setMargin(4);
	mLayout->setSpacing(4);

	mAdvancedAction = this->createAction(this,
	                                     QIcon(":/icons/open_icon_library/system-run-5.png"),
	                                     "Details",
	                                     "Show Advanced options",
	                                     SLOT(toggleAdvancedSlot()),
	                                     mLayout,
	                                     new CXSmallToolButton());

	mAdvancedWidget = new QWidget(this);
	mAdvancedLayout = new QHBoxLayout(mAdvancedWidget);
	mAdvancedLayout->setMargin(0);
	mLayout->addWidget(mAdvancedWidget);

    mSpaceSelector = StringDataAdapterXml::initialize("selectSpace",
        "Space",
        "Select coordinate system to store position in.",
	    "",
		QStringList(),
        QDomNode());
	connect(mSpaceSelector.get(), SIGNAL(valueWasSet(int)), this, SLOT(spacesChangedSlot()));
	connect(mSpaceSelector.get(), SIGNAL(valueWasSet(int)), this, SLOT(setModified()));
	QString space = settings()->value("sampler/Space", Space(csREF).toString()).toString();
	mSpaceSelector->setValue(space);
	LabeledComboBoxWidget* spaceSelectorWidget = new LabeledComboBoxWidget(this, mSpaceSelector);
	spaceSelectorWidget->showLabel(false);
    mAdvancedLayout->addWidget(spaceSelectorWidget);
	this->spacesChangedSlot();

	mCoordLineEdit = new QLineEdit(this);
	mCoordLineEdit->setStyleSheet("QLineEdit { width : 30ex; }"); // enough for "-xxx.x, -xxx.x, -xxx.x - xxxx" plus some slack
	mCoordLineEdit->setSizePolicy(QSizePolicy::Fixed,
                                  mCoordLineEdit->sizePolicy().verticalPolicy());
	mCoordLineEdit->setReadOnly(true);
    mLayout->addWidget(mCoordLineEdit);

	this->showAdvanced();
	this->setModified();
}

SamplerWidget::~SamplerWidget()
{}

QString SamplerWidget::defaultWhatsThis() const
{
  return "<html>"
	  "<h3>Utility for sampling the current tool point</h3>"
	  "<p>Displays the current tool tip position in a selected coordinate system.</p>"
	  "<p><i></i></p>"
	  "</html>";
}

void SamplerWidget::toggleAdvancedSlot()
{
	settings()->setValue("sampler/ShowDetails", !settings()->value("sampler/ShowDetails", "true").toBool());
	mAdvancedWidget->setVisible(!mAdvancedWidget->isVisible());
	this->showAdvanced();
}

void SamplerWidget::showAdvanced()
{
	bool on = settings()->value("sampler/ShowDetails").toBool();
	mAdvancedWidget->setVisible(on);
}

void SamplerWidget::spacesChangedSlot()
{
	CoordinateSystem space = CoordinateSystem::fromString(mSpaceSelector->getValue());
	settings()->setValue("sampler/Space", space.toString());

	std::vector<CoordinateSystem> spaces = spaceProvider()->getSpacesToPresentInGUI();
	QStringList range;
	for (unsigned i=0; i<spaces.size(); ++i)
	  range << spaces[i].toString();

	mSpaceSelector->setValueRange(range);
	mSpaceSelector->setValue(space.toString());
	mListener->setSpace(space);
}

void SamplerWidget::prePaintEvent()
{
	CoordinateSystem space = CoordinateSystem::fromString(mSpaceSelector->getValue());
	Vector3D p = spaceProvider()->getDominantToolTipPoint(space, true);
	int w=1;
	QString coord = QString("%1, %2, %3").arg(p[0], w, 'f', 1).arg(p[1], w, 'f', 1).arg(p[2], w, 'f', 1);

	ImagePtr image = dataManager()->getActiveImage();
	if (image)
	{
		Vector3D p = spaceProvider()->getDominantToolTipPoint(Space(csDATA_VOXEL,"active"), true);
		IntBoundingBox3D bb(Eigen::Vector3i(0,0,0),
		                         Eigen::Vector3i(image->getBaseVtkImageData()->GetDimensions())-Eigen::Vector3i(1,1,1));
		if (bb.contains(p.cast<int>()))
		{
			double val = image->getBaseVtkImageData()->GetScalarComponentAsFloat(p[0], p[1], p[2], 0);
			int intVal = val;
			coord += QString(" I=%1").arg(intVal);
		}
	}

	mCoordLineEdit->setText(coord);
	mCoordLineEdit->setStatusTip(QString("Position of active tool tip in %1 space\n"
	                                     "and the intensity of the active volume in that position").arg(space.toString()));
	mCoordLineEdit->setToolTip(mCoordLineEdit->statusTip());
}


} // namespace cx

