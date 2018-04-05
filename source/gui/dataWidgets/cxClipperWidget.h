/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCLIPPERWIDGET_H
#define CXCLIPPERWIDGET_H

#include "cxGuiExport.h"

#include <QVBoxLayout>
#include <QCheckBox>
#include "cxBaseWidget.h"
#include "cxStringPropertyBase.h"
#include "cxForwardDeclarations.h"
#include "cxData.h"
class QTableWidget;

namespace cx
{
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;
typedef boost::shared_ptr<class StringPropertyClipPlane> StringPropertyClipPlanePtr;

class LabeledComboBoxWidget;

class cxGui_EXPORT ClipperWidget : public BaseWidget
{
	Q_OBJECT
	void updateCheckBoxFromClipper(QCheckBox *checkbox, DataPtr data);
public:
	ClipperWidget(VisServicesPtr services, QWidget *parent);
	~ClipperWidget();
	void setClipper(InteractiveClipperPtr clipper);
protected slots:
	void setupDataSelectorUI();
	void enable(bool checked);
	void dataTypeSelectorClicked(bool checked);
	void selectAllTableData(bool checked);
	void dataSelectorClicked(QCheckBox *checkBox, DataPtr data);
	void onToolChanged();
protected:
	void setupUI();
	virtual void prePaintEvent();

	InteractiveClipperPtr mClipper;
//	StringPropertyClipPlanePtr mPlaneAdapter;
//	LabeledComboBoxWidget* planeSelector;
	QVBoxLayout* mLayout;
	QCheckBox* mUseClipperCheckBox;
	QCheckBox *mAttachedToTool;
	QCheckBox *mSelectAllData;
	QCheckBox *mInvertPlane;

	QCheckBox *mShowImages;
	QCheckBox *mShowMeshes;
	QCheckBox *mShowMetrics;
	QCheckBox *mShowTrackedStreams;

	VisServicesPtr mServices;
	std::map<QString, DataPtr> mDataToClip;
	QTableWidget *mDataTableWidget;
	std::map<QString, DataPtr> getDatas();
	bool mInitializedWithClipper;
	StringPropertySelectToolPtr mToolSelector;
	void setupDataStructures();
	void connectToNewClipper();
	QGroupBox *dataTableWidget();
	QLayout *planeLayout();
	QLayout *toolLayout();
	QString getDataTypeRegExp();
	void updateSelectAllCheckbox();
	void createNewCheckboxesBasedOnData();
};

}

#endif // CXCLIPPERWIDGET_H
