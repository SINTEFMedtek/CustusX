/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXREPLACABLECONTENTWIDGET_H
#define CXREPLACABLECONTENTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <boost/shared_ptr.hpp>
#include <iostream>

namespace cx
{

/** Widget wrapping another widget that can be replaced,
 *  the previous content being destroyed.
 *
 */
class ReplacableContentWidget : public QWidget
{
public:
	ReplacableContentWidget(QWidget* parent) : QWidget(parent), mWidget(NULL)
	{
		mLayout = new QVBoxLayout(this);
		mLayout->setMargin(0);
	}
	~ReplacableContentWidget()
	{
		this->setWidget(boost::shared_ptr<QWidget>()); // Delete objects in correct order
	}

	void setWidget(boost::shared_ptr<QWidget> widget)
	{
		this->setWidgetDontDeleteOld(widget.get());
		mWidgetBoostPointer = widget;
	}
	void setWidgetDeleteOld(QWidget* widget)
	{
		QWidget* oldwidget = mWidget;
		this->setWidgetDontDeleteOld(widget);
		delete oldwidget;
	}

	QWidget* getWidget()
	{
		return mWidget;
	}

private:
	QVBoxLayout* mLayout;
	QWidget* mWidget;
	boost::shared_ptr<QWidget> mWidgetBoostPointer;

	void setWidgetDontDeleteOld(QWidget* widget)
	{
		if (mWidget)
		{
			mLayout->takeAt(0);
			mWidget->hide();
			mWidget->setParent(NULL);
		}
		mWidget = widget;
		if (mWidget)
		{
			mLayout->addWidget(mWidget);
			mWidget->show();
		}
	}
};

}//end namespace cx

#endif // CXREPLACABLECONTENTWIDGET_H
