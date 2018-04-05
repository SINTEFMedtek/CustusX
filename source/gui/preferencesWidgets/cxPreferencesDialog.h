/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPREFERANCESDIALOG_H_
#define CXPREFERANCESDIALOG_H_

#include "cxGuiExport.h"

#include "cxPreferenceTab.h"
#include "cxGeneralTab.h"
#include <QDialog>
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxStringProperty.h"
#include "cxForwardDeclarations.h"

class QTabWidget;
class QToolBox;
class QToolBar;
class QStackedWidget;
class QDialogButtonBox;
class QLabel;
class QComboBox;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QRadioButton;
class QGridLayout;
class QVBoxLayout;
class QLineEdit;
class QActionGroup;

namespace cx
{
class Reporter;
class ViewManager;
class FilePreviewWidget;
class ToolImagePreviewWidget;
class ToolFilterGroupBox;
class ToolConfigureGroupBox;

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */



class cxGui_EXPORT VisualizationTab : public PreferenceTab
{
    Q_OBJECT
public:
  VisualizationTab(PatientModelServicePtr patientModelService, QWidget *parent = 0);
  void init();

  public slots:
  void saveParametersSlot();

signals:

protected:
  PatientModelServicePtr mPatientModelService;
  QGridLayout *mMainLayout;
  DoublePropertyPtr mSphereRadius;
  DoublePropertyPtr mLabelSize;
  BoolPropertyPtr mShowDataText;
  BoolPropertyPtr mShowLabels;
  BoolPropertyPtr mToolCrosshair;
  BoolPropertyPtr mShowMetricNamesInCorner;
  StringPropertyPtr mAnnotationModel;
  DoublePropertyPtr mAnnotationModelSize;
  DoublePropertyPtr mAnyplaneViewOffset;
  BoolPropertyPtr mFollowTooltip;
  DoublePropertyPtr mFollowTooltipBoundary;
  StringPropertyPtr mClinicalView;

  //Stereo
  QComboBox* mStereoTypeComboBox;
  DoublePropertyPtr mEyeAngleAdapter;
  QActionGroup* mStereoTypeActionGroup;
  void initStereoTypeComboBox();

private slots:
  //Stereo
  void stereoTypeChangedSlot(int index);
  void stereoFrameSequentialSlot();
  void stereoInterlacedSlot();
  void stereoDresdenSlot();
  void stereoRedBlueSlot();
  void eyeAngleSlot();
};

/**
 * \class AutomationTab
 *
 * \brief Automatic execution of actions when applicable
 *
 * \date Jan 27, 2011
 * \author Christian Askeland, SINTEF
 */
class cxGui_EXPORT AutomationTab : public PreferenceTab
{
  Q_OBJECT

public:
  AutomationTab(QWidget *parent = 0);
  void init();

public slots:
  void saveParametersSlot();


protected:
  QCheckBox* mAutoSelectActiveToolCheckBox;
  QCheckBox* mAutoStartTrackingCheckBox;
  QCheckBox* mAutoStartStreamingCheckBox;
  QCheckBox* mAutoReconstructCheckBox;
  QCheckBox* mAutoSaveCheckBox;
  QCheckBox* mAutoShowNewDataCheckBox;
  QCheckBox* mAutoLoadPatientCheckBox;
  QCheckBox* mAutoDeleteDICOMDBCheckBox;
  DoublePropertyPtr mAutoLoadPatientWithinHours;
  QVBoxLayout *mMainLayout;
};

/**
 * \class VideoTab
 *
 * \brief Various parameters related to ultrasound acquisition and reconstruction.
 *
 * \date Jan 27, 2011
 * \author Christian Askeland, SINTEF
 */
class cxGui_EXPORT VideoTab : public PreferenceTab
{
  Q_OBJECT

public:
  VideoTab(QWidget *parent = 0);
  void init();

public slots:
  void saveParametersSlot();

protected:
  QLineEdit* mAcquisitionNameLineEdit;
  QVBoxLayout *mMainLayout;

  QRadioButton* m24bitRadioButton;
  QRadioButton* m8bitRadioButton;
  QCheckBox* mCompressCheckBox;
};

/**
 * \class ToolConfigTab
 *
 * \brief Interface for selecting a tool configuration.
 *
 * The tab has three parts:
 * \verbatim
 *
 *  ____________________________________________
 * |                       |                    |
 * | ToolConfigureGroupBox | ToolFilterGroupBox |
 * |_______________________|____________________|
 * |                                            |
 * |                FilePreviewWidget           |
 * |____________________________________________|
 *
 * \endverbatim
 *
 * Tools are available as a filered list from ToolFilterGroupBox,
 * and can be added to a configuration in ToolConfigureGroupBox.
 *
 * The currently selected tool's file can be viewed in FilePreviewWidget.
 *
 * \date Mar 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT ToolConfigTab : public PreferenceTab
{
  Q_OBJECT

public:
  ToolConfigTab(StateServicePtr stateService, TrackingServicePtr trackingService, QWidget* parent = 0);
  virtual ~ToolConfigTab();

  virtual void init();

public slots:
  virtual void saveParametersSlot();

private slots:
  void applicationChangedSlot();
  void globalConfigurationFileChangedSlot(QString key);

private:
  FilePreviewWidget*  mFilePreviewWidget;
  ToolImagePreviewWidget*  mImagePreviewWidget;
  ToolConfigureGroupBox* mToolConfigureGroupBox;
  ToolFilterGroupBox*    mToolFilterGroupBox;
  StateServicePtr mStateService;
};

/**
 * \class PreferencesDialog
 *
 * \brief Set application preferences
 *
 * \author Frank Lindseth, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
	PreferencesDialog(ViewServicePtr viewService, PatientModelServicePtr patientModelService, StateServicePtr stateService, TrackingServicePtr trackingService, QWidget *parent = 0);
	virtual ~PreferencesDialog();

signals:
  void applied();

private slots:
  void selectTabSlot();
  void applySlot();

protected:
  void addTab(PreferenceTab* widget, QString name);

  QActionGroup* mActionGroup;
  QStackedWidget* mTabWidget;
  QToolBar* mToolBar;
  QDialogButtonBox *mButtonBox;
};

/**
 * \brief Debug settings
 *
 * \date Jan 24, 2012
 * \author Christian Askeland, SINTEF
 */
class cxGui_EXPORT DebugTab : public PreferenceTab
{
  Q_OBJECT

public:
  DebugTab(PatientModelServicePtr patientModelService, TrackingServicePtr trackingService, QWidget *parent = 0);
  void init();

public slots:
  void saveParametersSlot();
  void runDebugToolSlot();

protected:
  QCheckBox* mIGSTKDebugLoggingCheckBox;
  QCheckBox* mManualToolPhysicalPropertiesCheckBox;
  QCheckBox* mRenderSpeedLoggingCheckBox;
  QGridLayout *mMainLayout;
  PatientModelServicePtr mPatientModelService;
  TrackingServicePtr mTrackingService;
};



/**
 * @}
 */
}//namespace cx

#endif
