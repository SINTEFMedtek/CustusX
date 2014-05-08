// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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
