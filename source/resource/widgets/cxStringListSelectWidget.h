/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTRINGLISTSELECTWIDGET_H
#define CXSTRINGLISTSELECTWIDGET_H

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include "cxStringListProperty.h"
#include "cxBaseWidget.h"

namespace cx
{


/** \brief Composite widget for string list selection.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT StringListSelectWidget: public BaseWidget
{
Q_OBJECT
public:
	StringListSelectWidget(QWidget* parent, StringListPropertyPtr property, QGridLayout* gridLayout = 0, int row = 0);
	virtual ~StringListSelectWidget() {}

	void showLabel(bool on);
	void setIcon(QIcon icon);

protected:
	QHBoxLayout* mTopLayout;

private slots:
	void prePaintEvent();

private:
	void onCheckToggled(QString nodeType, bool value);

	QLabel* mLabel;
	QToolButton* mButton;
	QMenu* mMenu;
	StringListPropertyPtr mData;
	QStringList mCachedRange;
	std::vector<QCheckBox*> mCheckBoxes;
};

} // namespace cx

#endif // CXSTRINGLISTSELECTWIDGET_H
