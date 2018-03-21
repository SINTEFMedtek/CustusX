/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSELECTCLIPPERSFORDATAWIDGET_H
#define CXSELECTCLIPPERSFORDATAWIDGET_H

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
class QTableWidget;

namespace cx
{

typedef boost::shared_ptr<class SelectDataStringPropertyBase> SelectDataStringPropertyBasePtr;

/**\brief Turn clippers on/off for a spesific data structure.
 *
 *  \date 02 Nov, 2015
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT SelectClippersForDataWidget: public BaseWidget
{
	Q_OBJECT
	void createDataCheckBox(int row, QString clipperName);
	QCheckBox *createCheckBox(QString clipperName);
	void updateCheckBoxesFromClipper(QCheckBox *dataCheckBox, QCheckBox *invertCheckBox, QString clipperName);
	cx::InteractiveClipperPtr getClipper(QString clipperName);
public:
	SelectClippersForDataWidget(VisServicesPtr services, QWidget *parent);
	void setActiveDataProperty(SelectDataStringPropertyBasePtr property);
protected slots:
	void clipDataClicked(QCheckBox *checkBox, QString clipperName);
	void invertClicked(QCheckBox *checkBox, QString clipperName);
protected:
	SelectDataStringPropertyBasePtr mActiveDataProperty;
	VisServicesPtr mServices;
	QVBoxLayout* mLayout;
	QLabel *mHeading;
	QTableWidget *mClipperTableWidget;
	void createNewCheckBoxesBasedOnClippers();
	void initUI();
	void setupClipperSelectorUI();
	virtual void prePaintEvent();
};

/// -------------------------------------------------------

class SelectClippersForImageWidget : public BaseWidget
{
	Q_OBJECT
public:
	SelectClippersForImageWidget(VisServicesPtr services, QWidget *parent);
};

/// -------------------------------------------------------

class SelectClippersForMeshWidget : public BaseWidget
{
	Q_OBJECT
public:
	SelectClippersForMeshWidget(VisServicesPtr services, QWidget *parent);
};

}//cx

#endif // CXSELECTCLIPPERSFORDATAWIDGET_H
