/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTTESTVIDEOCONNECTIONWIDGET_H_
#define CXTESTTESTVIDEOCONNECTIONWIDGET_H_

#include "cxVideoConnectionWidget.h"

namespace cxtest
{

/*
 * TestVideoConnectionWidget
 *
 * \date May 27, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class TestVideoConnectionWidget : public cx::VideoConnectionWidget
{
	Q_OBJECT
public:
	TestVideoConnectionWidget(cx::VisServicesPtr services);
	virtual ~TestVideoConnectionWidget(){}

public:
	bool canStream(QString filename);

private:
	void setupWidgetToRunStreamer(QString filename);
	cx::PropertyPtr getOption(QString uid, QString method);
};

} /* namespace cxtest */
#endif /* CXTESTTESTVIDEOCONNECTIONWIDGET_H_ */
