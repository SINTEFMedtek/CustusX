/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSPACEEDITWIDGET_H
#define CXSPACEEDITWIDGET_H

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include "cxSpacePropertyBase.h"
#include "cxBaseWidget.h"

namespace cx
{

/**\brief Composite widget for string selection.
 *
 * Consists of <namelabel, combobox>.
 * Insert a subclass of SpaceProperty to connect to data.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SpaceEditWidget: public BaseWidget
{
Q_OBJECT
public:
	SpaceEditWidget(QWidget* parent, SpacePropertyBasePtr, QGridLayout* gridLayout = 0, int row = 0);
	virtual ~SpaceEditWidget() {}

	void showLabel(bool on);

protected:
	QHBoxLayout* mTopLayout;

private slots:
	void prePaintEvent();
	void comboIndexChanged();

private:
	void attemptSetValue(COORDINATE_SYSTEM id, QString ref);
	void rebuildIdCombobox();
	std::vector<COORDINATE_SYSTEM> getAvailableSpaceIds();
	void rebuildRefCombobox();
	QStringList getAvailableSpaceRefs(COORDINATE_SYSTEM id);
	void setRefComboVisibilityQueued();

	QLabel* mLabel;
	QComboBox* mIdCombo;
	QComboBox* mRefCombo;
	SpacePropertyBasePtr mData;
};

} // namespace cx

#endif // CXSPACEEDITWIDGET_H
