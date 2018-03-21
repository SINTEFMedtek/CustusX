/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXVECTOR3DWIDGET_H_
#define CXVECTOR3DWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxVector3D.h"
#include <QWidget>
class QBoxLayout;
#include "cxVector3DPropertyBase.h"
#include "cxDoubleProperty.h"

namespace cx
{


/**
 * \brief Widget for displaying a Vector3D
 *
 * \ingroup cx_resource_widgets
 *
 */
class cxResourceWidgets_EXPORT Vector3DWidget: public QWidget
{
Q_OBJECT
public:
	Vector3DWidget(QWidget* parent, Vector3DPropertyBasePtr data);
	virtual ~Vector3DWidget() {}

	static Vector3DWidget* createSmallHorizontal(QWidget* parent, Vector3DPropertyBasePtr data);
	static Vector3DWidget* createVerticalWithSliders(QWidget* parent, Vector3DPropertyBasePtr data);
	void showDim(int dim, bool visible);

private:
	void addSliderControlsForIndex(QString name, QString help, int index, QBoxLayout* layout);
	void addSmallControlsForIndex(QString name, QString help, int index, QBoxLayout* layout);

	Vector3DPropertyBasePtr mData;
	boost::array<DoublePropertyBasePtr, 3> mDoubleAdapter;
	boost::array<QWidget*, 3> mWidgets;
};

}

#endif /* CXVECTOR3DWIDGET_H_ */
