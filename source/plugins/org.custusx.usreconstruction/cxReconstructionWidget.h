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
#include <memory>

#include <QtWidgets>

#include "cxDoubleWidgets.h"
#include "cxXmlOptionItem.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxFileSelectWidget.h"
#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"


namespace cx
{
class TimedAlgorithmProgressBar;
class UsReconstructionService;
typedef std::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;

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
	ReconstructionWidget(QWidget* parent, UsReconstructionServicePtr reconstructer, PatientModelServicePtr patientModelService);
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
	/** Show only the parameters of the selected position filter, and enable/disable the time calibration parameters
	 *  that are not used with the current selections. */
	void updateParameterStates();
private:
	UsReconstructionServicePtr mReconstructer;

	FileSelectWidget* mFileSelectWidget;
	QPushButton* mReconstructButton;
	QPushButton* mReloadButton;
	QLineEdit* mExtentLineEdit;
	QLineEdit* mInputSpacingLineEdit;
	QLineEdit* mOutputSpacingLineEdit; ///< Indicator for the output volume spacing
	QLineEdit* mDimXLineEdit; ///< Indicators for the output volume dimensions
	QLineEdit* mDimYLineEdit;
	QLineEdit* mDimZLineEdit;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;

	QFrame* mAlgorithmGroup;
	QStackedLayout* mAlgoLayout;
	std::vector<QWidget*> mAlgoWidgets;

	QWidget* mOptionsWidget;
	QGridLayout* mOptionsLayout;
	std::vector<int> mMovingAverageRows; ///< Rows in mOptionsLayout with the parameters of the moving average filter
	std::vector<int> mHoltRows; ///< Rows in mOptionsLayout with the parameters of the Holt filter
	PropertyPtr mApplyLagProperty; ///< Option 'Apply estimated time lag' of the selected algorithm, if it has one
	QWidget* createOptionsWidget();
	QLineEdit* createIndicator(); ///< Read-only line edit, for showing a value
	QString getCurrentPath();
	void updateComboBox();
	void createNewStackedWidget(QString algoName);
};

/**
 * @}
 */
}//namespace
#endif //CXRECONSTRUCTIONWIDGET_H_
