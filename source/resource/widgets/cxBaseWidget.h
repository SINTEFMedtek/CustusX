/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXBASEWIDGET_H_
#define CXBASEWIDGET_H_

#include "cxResourceWidgetsExport.h"
#include <QFrame>
#include <QToolButton>
#include <QAction>
#include <QLayout>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include "cxOptimizedUpdateWidget.h"
#include "boost/function.hpp"

class QWidget;
class QGroupBox;
class QCheckBox;
class QLabel;

/**
* \file
* \addtogroup cx_resource_widgets
* @{
*/

/** Frame descendant with dedicated style sheet: framed
  *
  * \date Nov 25, 2012
  * \author Christian Askeland, SINTEF
  */
class cxResourceWidgets_EXPORT CXFrame : public QFrame
{
    Q_OBJECT
public:
    CXFrame(QWidget* parent) : QFrame(parent) {}
    virtual ~CXFrame() {}
};

/** QToolButton descendant with dedicated style sheet: rounded corners
  *
  * \date Feb 4, 2014
  * \author Christian Askeland, SINTEF
  */
class cxResourceWidgets_EXPORT CXToolButton : public QToolButton
{
	Q_OBJECT
public:
	CXToolButton(QWidget* parent=0) : QToolButton(parent) {}
};

/** QToolButton descendant with dedicated style sheet: smaller
  *
  * \date Nov 25, 2012
  * \author Christian Askeland, SINTEF
  */
class cxResourceWidgets_EXPORT CXSmallToolButton : public CXToolButton
{
    Q_OBJECT
public:
	CXSmallToolButton(QWidget* parent=0) : CXToolButton(parent) {}
};


namespace cx
{
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class ViewService> ViewServicePtr;
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;
/**
 * \class BaseWidget
 *
 * \brief Interface for QWidget which handles widgets uniformly for the system.
 *
 * \date 28. sep. 2010
 * \author Janne Beate Bakeng, SINTEF
 */

class cxResourceWidgets_EXPORT BaseWidget: public OptimizedUpdateWidget
{
	Q_OBJECT

public:
	BaseWidget(QWidget* parent, QString objectName, QString windowTitle);
	virtual ~BaseWidget() {}

    QWidget* createMethodWidget(QWidget* inputWidget, QWidget* outputWidget, QString methodname,
    		bool inputChecked = false, bool outputVisible = true);///< Create a specialized widget for filters, with input/ouput, enable and options.

    QGroupBox* createGroupbox(QWidget* widget, QString boxname);///< Create a group box with a given name.
    static QFrame* createHorizontalLine(); ///< Creates a horizontal line which can be inserted into widgets
    QGroupBox* wrapInGroupBox(QWidget* base, QString name);
    CXFrame* wrapInFrame(QWidget* base);

	/**Shorthand method for action creation.
	 * If layout is used, a QToolButton is created and added to it.
	 * If tip is empty, it is set equal to text.
	 */
	QAction* createAction2(QObject* parent, QIcon iconName, QString text, QString tip, QLayout* layout=NULL, QToolButton* button = new QToolButton())
	{
		if (tip.isEmpty())
			tip = text;
	  QAction* action = new QAction(iconName, text, parent);
	  action->setStatusTip(tip);
	  action->setWhatsThis(tip);
	  action->setToolTip(tip);
	  if (layout)
	  {
		  button->setDefaultAction(action);
		  layout->addWidget(button);
	  }
	  return action;
	}

	/**Shorthand method for action creation.
	 * If layout is used, a QToolButton is created and added to it.
	 * If tip is empty, it is set equal to text.
	 */
	template<class T>
	QAction* createAction(QObject* parent, QIcon iconName, QString text, QString tip, T slot, QLayout* layout=NULL, QToolButton* button = new QToolButton())
	{
	  QAction* action = createAction2(parent, iconName, text, tip, layout, button);
	  connect(action, SIGNAL(triggered()), this, slot);
	  return action;
	}


public slots:
    void adjustSizeSlot();

protected:
	virtual void showEvent(QShowEvent* event);

private:
    QString mObjectName;
	QString mWindowTitle;
};

}

/**
* @}
*/

#endif /* CXBASEWIDGET_H_ */
