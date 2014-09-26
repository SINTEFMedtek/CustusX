/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXPREFERANCESDIALOG_H_
#define CXPREFERANCESDIALOG_H_

#include "cxPreferenceTab.h"
#include "cxGeneralTab.h"
#include <QDialog>
#include "cxDoubleDataAdapterXml.h"
#include "cxBoolDataAdapterXml.h"
#include "cxStringDataAdapterXml.h"
#include "cxLegacySingletons.h"

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

class VisualizationTab : public PreferenceTab
{
    Q_OBJECT
public:
  VisualizationTab(QWidget *parent = 0);
  void init();

  public slots:
  void saveParametersSlot();

signals:

protected:
  QGridLayout *mMainLayout;
  DoubleDataAdapterXmlPtr mSphereRadius;
  DoubleDataAdapterXmlPtr mLabelSize;
  BoolDataAdapterXmlPtr mShowDataText;
  BoolDataAdapterXmlPtr mShowLabels;
  BoolDataAdapterXmlPtr mShowMetricNamesInCorner;
  StringDataAdapterXmlPtr mAnnotationModel;
  DoubleDataAdapterXmlPtr mAnnotationModelSize;
  DoubleDataAdapterXmlPtr mAnyplaneViewOffset;
  BoolDataAdapterXmlPtr mFollowTooltip;
  DoubleDataAdapterXmlPtr mFollowTooltipBoundary;

  //Stereo
  QComboBox* mStereoTypeComboBox;
  DoubleDataAdapterXmlPtr mEyeAngleAdapter;
  QActionGroup* mStereoTypeActionGroup;
  void initStereoTypeComboBox();

private slots:
  void setBackgroundColorSlot(QColor color);
  //Stereo
  void stereoTypeChangedSlot(int index);
  void stereoFrameSequentialSlot();
  void stereoInterlacedSlot();
  void stereoDresdenSlot();
  void stereoRedBlueSlot();
  void eyeAngleSlot();

//  void setBackgroundColorSlot();
};

/**
 * \class AutomationTab
 *
 * \brief Automatic execution of actions when applicable
 *
 * \date Jan 27, 2011
 * \author Christian Askeland, SINTEF
 */
class AutomationTab : public PreferenceTab
{
  Q_OBJECT

public:
  AutomationTab(QWidget *parent = 0);
  void init();

public slots:
  void saveParametersSlot();


protected:
  QCheckBox* mAutoSelectDominantToolCheckBox;
  QCheckBox* mAutoStartTrackingCheckBox;
  QCheckBox* mAutoStartStreamingCheckBox;
  QCheckBox* mAutoReconstructCheckBox;
  QCheckBox* mAutoSaveCheckBox;
  QCheckBox* mAutoShowNewDataCheckBox;
  QCheckBox* mAutoLoadPatientCheckBox;
  DoubleDataAdapterXmlPtr mAutoLoadPatientWithinHours;
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
class VideoTab : public PreferenceTab
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
 * /endverbatim
 *
 * Tools are available as a filered list from ToolFilterGroupBox,
 * and can be added to a configuration in ToolConfigureGroupBox.
 *
 * The currently selected tool's file can be viewed in FilePreviewWidget.
 *
 * \date Mar 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class ToolConfigTab : public PreferenceTab
{
  Q_OBJECT

public:
  ToolConfigTab(QWidget* parent = 0);
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
};

/**
 * \class PreferencesDialog
 *
 * \brief Set application preferences
 *
 * \author Frank Lindseth, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
	PreferencesDialog(VisualizationServicePtr visualizationService, PatientModelServicePtr patientModelService, QWidget *parent = 0);
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
class DebugTab : public PreferenceTab
{
  Q_OBJECT

public:
  DebugTab(QWidget *parent = 0);
  void init();

public slots:
  void saveParametersSlot();
  void runDebugToolSlot();

protected:
  QCheckBox* mIGSTKDebugLoggingCheckBox;
  QCheckBox* mManualToolPhysicalPropertiesCheckBox;
  QCheckBox* mRenderSpeedLoggingCheckBox;
  QGridLayout *mMainLayout;
};



/**
 * @}
 */
}//namespace cx

#endif
