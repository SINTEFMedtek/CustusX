/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKINGSHAPEWIDGET_H_
#define CXTRACKINGSHAPEWIDGET_H_

#include "org_custusx_tracking_shape_Export.h"
#include "cxBaseWidget.h"
#include "cxSocketConnection.h"
#include "cxForwardDeclarations.h"
#include "cxReadFbgsMessage.h"
class QVBoxLayout;
class QPushButton;

namespace cx
{

/**
 * Widget for use in the plugin tracking_shape
 *
 * \ingroup org_custusx_tracking_shape
 *
 * \date 2022-11-08
 * \author Ole Vegard Solberg
 */
class org_custusx_tracking_shape_EXPORT ShapeSensorWidget : public BaseWidget
{
	Q_OBJECT
public:
	ShapeSensorWidget(VisServicesPtr services, QWidget* parent = 0);
	virtual ~ShapeSensorWidget();

private slots:
	void dataAvailableSlot();
	void connectStateChangedSlot(CX_SOCKETCONNECTION_STATE status);
	void connectClickedSlot();
	void showClickedSlot();
	void testShapeClickedSlot();
	void activeToolChangedSlot();
	void receiveTransforms(Transform3D prMt, double timestamp);
private:
	VisServicesPtr mServices;
	QVBoxLayout*  mVerticalLayout;
	SocketConnectionPtr mSocketConnection;
	StringPropertyBasePtr mIpAddress;
	DoublePropertyBasePtr mIpPort;
	QPushButton *mConnectButton;
	QPushButton *mShowShapeButton;
	QPushButton *mTestShapeButton;
	ReadFbgsMessage mReadFbgsMessage;
	bool mShowShape = false;
	StringPropertyBasePtr mSelector;
	ToolPtr mTool;

	QString defaultWhatsThis() const;
	StringPropertyBasePtr getIPAddress(QDomElement root);
	DoublePropertyBasePtr getIPPort(QDomElement root);
	void readBuffer(QString buffer);
	bool readShape(QString axisString, QString buffer);
	bool readPositions(QString buffer, int bufferPos);
};

} /* namespace cx */

#endif /* CXTRACKINGSHAPEWIDGET_H_ */
