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

class ctkPluginContext;


namespace cx
{
class TimedAlgorithmProgressBar;
class UsReconstructionService;
typedef std::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;
class UsReconstructionExtensionService;
class UsReconstructionRunner;
class ReconstructionResultsWidget;
template <class T> class ServiceTrackerListener;

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
	ReconstructionWidget(QWidget* parent, UsReconstructionServicePtr reconstructer, PatientModelServicePtr patientModelService, ViewServicePtr viewService, ctkPluginContext* pluginContext);
	virtual ~ReconstructionWidget();
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
	void runFinished();
	void progressChanged(int done, int total);
	void updateButtons();

	void reconstructAboutToStartSlot();

	void updateFileSelectorPath(QString path);
	/** Show only the parameters of the selected position filter, and enable/disable the time calibration parameters
	 *  that are not used with the current selections. */
	void updateParameterStates();
private:
	struct Extension
	{
		UsReconstructionExtensionService* mService;
		std::vector<std::pair<QString, QCheckBox*> > mOutputs;
		QWidget* mTab;
	};
	UsReconstructionServicePtr mReconstructer;
	PatientModelServicePtr mPatientModelService;

	QTabWidget* mTabs;
	FileSelectWidget* mFileSelectWidget;
	QCheckBox* mUsVolumeCheckBox;
	QCheckBox* mAngioVolumeCheckBox;
	QCheckBox* mAngioMeshCheckBox;
	QVBoxLayout* mOutputsLayout;
	QPushButton* mReconstructButton;
	QProgressBar* mProgressBar;
	QLabel* mStatusLabel;
	ReconstructionResultsWidget* mResultsWidget;
	UsReconstructionRunner* mRunner;
	bool mUsReconstructing;
	std::vector<Extension> mExtensions;
	std::shared_ptr<ServiceTrackerListener<UsReconstructionExtensionService> > mExtensionListener;

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

	QGridLayout* mOptionsLayout;
	std::vector<int> mMovingAverageRows; ///< Rows in mOptionsLayout with the parameters of the moving average filter
	std::vector<int> mHoltRows; ///< Rows in mOptionsLayout with the parameters of the Holt filter
	PropertyPtr mApplyLagProperty; ///< Option 'Apply estimated time lag' of the selected algorithm, if it has one
	QWidget* createReconstructTab(ViewServicePtr viewService);
	QWidget* createSettingsTab();
	QWidget* createOptionsWidget(QWidget* parent);
	QCheckBox* createOutputCheckBox(QString uid, QString name, QString help, bool defaultValue);
	QLineEdit* createIndicator(QWidget* parent); ///< Read-only line edit, for showing a value
	QString getCurrentPath();
	void updateComboBox();
	void createNewStackedWidget(QString algoName);
	void onExtensionAdded(UsReconstructionExtensionService* service);
	void onExtensionRemoved(UsReconstructionExtensionService* service);
	void storeOutputSelections();
	bool isAnyOutputSelected() const;
	QColor getDataColor(DataPtr data) const;
	void setDataColor(DataPtr data, QColor color);
};

/**
 * @}
 */
}//namespace
#endif //CXRECONSTRUCTIONWIDGET_H_
