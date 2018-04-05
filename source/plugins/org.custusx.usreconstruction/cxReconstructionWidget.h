/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXRECONSTRUCTIONWIDGET_H_
#define CXRECONSTRUCTIONWIDGET_H_

#include "org_custusx_usreconstruction_Export.h"

#include <QtWidgets>

#include "cxDoubleWidgets.h"
#include "cxXmlOptionItem.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxFileSelectWidget.h"
#include "cxBaseWidget.h"


namespace cx
{
class TimedAlgorithmProgressBar;
class UsReconstructionService;
typedef boost::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;

/**
 * \file
 * \addtogroup org_custusx_usreconstruction
 * @{
 */

/**
 *  sscReconstructionWidget.h
 *
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */
class org_custusx_usreconstruction_EXPORT ReconstructionWidget: public BaseWidget
{
Q_OBJECT
public:
	ReconstructionWidget(QWidget* parent, UsReconstructionServicePtr reconstructer);
	UsReconstructionServicePtr reconstructer()
	{
		return mReconstructer;
	}

public slots:
	void selectData(QString inputfile);
	void reconstruct();
	void reload();
	void paramsChangedSlot();
private slots:
	void inputDataSelected(QString mhdFileName);
	/** Add the widgets for the current algorithm to a stacked widget.*/
	void repopulateAlgorithmGroup();
	void reconstructStartedSlot();
	void reconstructFinishedSlot();
	void toggleDetailsSlot();

	void reconstructAboutToStartSlot();

	void updateFileSelectorPath(QString path);
private:
	UsReconstructionServicePtr mReconstructer;

	FileSelectWidget* mFileSelectWidget;
	QPushButton* mReconstructButton;
	QPushButton* mReloadButton;
	QLineEdit* mExtentLineEdit;
	QLineEdit* mInputSpacingLineEdit;
	SpinBoxGroupWidget* mSpacingWidget;
	SpinBoxGroupWidget* mDimXWidget;
	SpinBoxGroupWidget* mDimYWidget;
	SpinBoxGroupWidget* mDimZWidget;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;

	QFrame* mAlgorithmGroup;
	QStackedLayout* mAlgoLayout;
	std::vector<QWidget*> mAlgoWidgets;

	QWidget* mOptionsWidget;
	QWidget* createOptionsWidget();
	QString getCurrentPath();
	void updateComboBox();
	void createNewStackedWidget(QString algoName);
};

/**
 * @}
 */
}//namespace
#endif //CXRECONSTRUCTIONWIDGET_H_
