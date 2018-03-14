/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXHELPINDEXWIDGET_H
#define CXHELPINDEXWIDGET_H

#include <QWidget>
#include "boost/shared_ptr.hpp"
class QLineEdit;

namespace cx
{
typedef boost::shared_ptr<class HelpEngine> HelpEnginePtr;

/**
 *
 * \ingroup org_custusx_help
 *
 * \date 2014-09-30
 * \author Christian Askeland
 */
class HelpIndexWidget : public QWidget
{
	Q_OBJECT

public:
	explicit HelpIndexWidget(HelpEnginePtr engine, QWidget* parent = NULL);
	virtual ~HelpIndexWidget() {}

signals:
	void requestShowLink(const QUrl&);

private slots:
	void filterChanged();

private:
	HelpEnginePtr mEngine;
	QLineEdit* mFilterEdit;
};

}//end namespace cx

#endif // CXHELPINDEXWIDGET_H
