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
#include "cxNetworkConnectionsWidget.h"

#include "cxNetworkConnectionManager.h"
#include "cxStringProperty.h"
#include "cxOpenIGTLinkConnectionWidget.h"
#include <QGroupBox>
#include "cxDetailedLabeledComboBoxWidget.h"
#include "cxOpenIGTLinkClient.h"
#include "cxLogger.h"
#include "cxConnectionHandle.h"

namespace cx
{

struct WidgetInGroupBox
{
	WidgetInGroupBox(QVBoxLayout* parentLayout, QString header)
	{
		groupBox = new QGroupBox(header);
		parentLayout->addWidget(groupBox);
		layout = new QVBoxLayout(groupBox);
		layout->setMargin(2);
	}

	QGroupBox* groupBox;
	QVBoxLayout* layout;

	void replaceWidget(QWidget* newWidget)
	{
		this->clearLayout();
		layout->addWidget(newWidget);
	}
	void clearLayout()
	{
		QLayoutItem *child;
		while ((child = layout->takeAt(0)) != 0)
		{
			QWidget* widget = child->widget();
			delete child;
			delete widget;
		}
	}
};





NetworkConnectionsWidget::NetworkConnectionsWidget(NetworkConnectionManagerPtr connections, QWidget* parent) :
	BaseWidget(parent, "NetworkConnectionsWidget", "Network Connections"),
	mConnections(connections)
{
	mConnectionSelector = StringProperty::initialize("Connection", "",
													 "Selected Network Connection",
													 "", QStringList(),
													 mOptions.getElement("video"));
	connect(mConnectionSelector.get(), &StringProperty::changed,
			this, &NetworkConnectionsWidget::onConnectionSelected);

	connect(mConnections.get(), &NetworkConnectionManager::connectionsChanged,
			this, &NetworkConnectionsWidget::onNetworkManagerChanged);

	mConnectionSelectionWidget = new DetailedLabeledComboBoxWidget(this, mConnectionSelector);
	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(mConnectionSelectionWidget);
	mOptionsWidget = new WidgetInGroupBox(topLayout, "connection");
	topLayout->addStretch();

	this->onNetworkManagerChanged();
}

void NetworkConnectionsWidget::onNetworkManagerChanged()
{
	QString value = mConnectionSelector->getValue();
	QStringList connectionUids = mConnections->getConnectionUids();

	if (!connectionUids.count(value))
		value = connectionUids.empty() ? "" : connectionUids.front();

	mConnectionSelector->setValueRange(connectionUids);
	mConnectionSelector->setValue(value);

	this->updateConnectionWidget();
}

void NetworkConnectionsWidget::onConnectionSelected()
{
	this->updateConnectionWidget();
}

void NetworkConnectionsWidget::updateConnectionWidget()
{
	NetworkConnectionHandlePtr connection = mConnections->getConnection(mConnectionSelector->getUid());
	OpenIGTLinkConnectionWidget* widget = new OpenIGTLinkConnectionWidget(connection);
	mOptionsWidget->replaceWidget(widget);
	if (connection)
	{
		CX_LOG_CHANNEL_DEBUG("CA") << "update " <<  (connection->client()->getUid());
		mOptionsWidget->groupBox->setTitle(connection->client()->getUid());
	}
}


} // namespace cx
