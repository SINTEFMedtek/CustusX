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
#include "cxOpenIGTLinkDataTransferWidget.h"

#include <QPushButton>
#include <QGroupBox>
#include "cxOpenIGTLinkClient.h"
#include "cxOpenIGTLinkConnectionWidget.h"
#include "cxBoolProperty.h"
#include "cxHelperWidgets.h"
#include "cxSelectDataStringProperty.h"
#include "cxStringProperty.h"
#include "cxOpenIGTLinkDataTransfer.h"

namespace cx {

OpenIGTLinkDataTransferWidget::OpenIGTLinkDataTransferWidget(ctkPluginContext *context, QWidget *parent) :
	BaseWidget(parent, "OpenIGTLinkDataTransferWidget", "OpenIGTLink Data Transfer")
{
	mDataTransfer.reset(new OpenIGTLinkDataTransfer(context));

	mConnectionWidget = new OpenIGTLinkConnectionWidget(mDataTransfer->getOpenIGTLink());

	// handled by dialect - remove (kept cause might be useful in export/import dialogs)
//	mCoordinateSystem = StringProperty::initialize("igltcoords", "Coordinate System",
//												   "Select which space to interpret/represent the iglink data in\n"
//												   "RAS is used by Slicer and others,\n"
//												   "LPS by CustusX and DICOM.",
//												   "RAS",
//												   QStringList() << "RAS" << "LPS",
//												   mOptions.getElement());
//	std::map<QString,QString> names;
//	names["LPS"] = "LPS (DICOM)";
//	names["RAS"] = "RAS (Slicer)";
//	mCoordinateSystem->setDisplayNames(names);

	QPushButton* sendButton = new QPushButton("Send", this);
	connect(sendButton, &QPushButton::clicked, mDataTransfer.get(), &OpenIGTLinkDataTransfer::onSend);

	QVBoxLayout* layout = new QVBoxLayout(this);
//	layout->setMargin(0);

	QVBoxLayout* connectionLayout = this->createVBoxInGroupBox(layout, "Connection");
	connectionLayout->addWidget(mConnectionWidget);

	QVBoxLayout* receiveLayout = this->createVBoxInGroupBox(layout, "Receive");
	receiveLayout->addWidget(createDataWidget(mDataTransfer->getViewService(),
											  mDataTransfer->getPatientModelService(),
											  this, mDataTransfer->getAcceptIncomingData()));

	QVBoxLayout* sendLayout = this->createVBoxInGroupBox(layout, "Send");
	sendLayout->addWidget(createDataWidget(mDataTransfer->getViewService(),
										   mDataTransfer->getPatientModelService(),
										   this, mDataTransfer->getDataToSend()));
	sendLayout->addWidget(sendButton);
}

OpenIGTLinkDataTransferWidget::~OpenIGTLinkDataTransferWidget()
{
	mDataTransfer.reset();
}

QVBoxLayout* OpenIGTLinkDataTransferWidget::createVBoxInGroupBox(QVBoxLayout* parent, QString header)
{
	QWidget* widget = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setMargin(0);

	QGroupBox* groupBox = this->wrapInGroupBox(widget, header);
	parent->addWidget(groupBox);

	return layout;
}

} // namespace cx
