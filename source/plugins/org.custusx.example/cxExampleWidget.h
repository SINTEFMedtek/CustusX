/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXEXAMPLEWIDGET_H_
#define CXEXAMPLEWIDGET_H_

#include <QWidget>
class QVBoxLayout;

namespace cx
{

/**
 * Widget for use in the plugin example
 *
 * \ingroup org_custusx_example
 *
 * \date 2014-05-02
 * \author Christian Askeland
 */
class ExampleWidget : public QWidget
{
	Q_OBJECT
public:
	ExampleWidget(QWidget* parent = 0);
	virtual ~ExampleWidget();

private:
	QString defaultWhatsThis() const;
	QVBoxLayout*  mVerticalLayout;
};

} /* namespace cx */

#endif /* CXEXAMPLEWIDGET_H_ */
