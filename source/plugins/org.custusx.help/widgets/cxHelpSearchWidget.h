/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXHELPSEARCHWIDGET_H
#define CXHELPSEARCHWIDGET_H

#include <QWidget>
#include "boost/shared_ptr.hpp"

QT_BEGIN_NAMESPACE
class QHelpSearchEngine;
class QVBoxLayout;
QT_END_NAMESPACE;

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
class HelpSearchWidget : public QWidget
{
	Q_OBJECT
public:
	HelpSearchWidget(HelpEnginePtr engine, QWidget* parent = NULL);

signals:
	void requestShowLink(const QUrl&);

protected slots:
  void indexingStarted();
  void indexingFinished();
  void searchingIsStarted();
  void searchingIsFinished(int val);
  void search();
private:
  QVBoxLayout* mVerticalLayout;
  HelpEnginePtr mEngine;
  QHelpSearchEngine* helpSearchEngine;
};

}//end namespace cx

#endif // CXHELPSEARCHWIDGET_H
