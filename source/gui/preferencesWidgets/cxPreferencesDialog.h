#ifndef CXPREFERANCESDIALOG_H_
#define CXPREFERANCESDIALOG_H_

#include <QDialog>
#include "sscDoubleDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"
#include "sscStringDataAdapterXml.h"

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
class MessageManager;
class ViewManager;
class FilePreviewWidget;
class ToolImagePreviewWidget;
class ToolFilterGroupBox;
class ToolConfigureGroupBox;

/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

class PreferencesTab : public QWidget
{
  Q_OBJECT
public:
  PreferencesTab(QWidget *parent = 0);

  virtual void init() = 0;

public slots:
  virtual void saveParametersSlot() = 0;

signals:
  void savedParameters();

protected:
  QVBoxLayout* mTopLayout;
};

/**
 * \class GeneralTab
 *
 * \brief Tab for general settings in the system
 *
 * \date Jan 25, 2010
 * \author Frank Lindseth, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class GeneralTab : public PreferencesTab
{
  Q_OBJECT

public:
  GeneralTab(QWidget *parent = 0);
  virtual ~GeneralTab();
  void init();

public slots:
  void saveParametersSlot();
  
private slots:
  void browsePatientDataFolderSlot();

  void currentApplicationChangedSlot(int index);
  void applicationStateChangedSlot();

private:
  void setApplicationComboBox();

  QComboBox* mPatientDataFolderComboBox;

  QComboBox* mToolConfigFolderComboBox;
  QComboBox* mChooseApplicationComboBox;

  QString mGlobalPatientDataFolder;
};

/**
 * \class PerformanceTab
 *
 * \brief Configure performance tab in preferences dialog
 *
 * \date Mar 8, 2010
 * \author Ole Vegard Solberg, SINTEF
 */
class PerformanceTab : public PreferencesTab
{
    Q_OBJECT

public:
  PerformanceTab(QWidget *parent = 0);
  void init();
  
public slots:
  void saveParametersSlot();

protected:
  QSpinBox* mRenderingIntervalSpinBox;
  QLabel* mRenderingRateLabel;
  QCheckBox* mSmartRenderCheckBox;
  QCheckBox* mGPURenderCheckBox;
  QCheckBox* mGPU2DRenderCheckBox;
//  QCheckBox* mGPU3DDepthPeelingCheckBox;
  QCheckBox* mShadingCheckBox;
  QGridLayout *mMainLayout;
  ssc::DoubleDataAdapterXmlPtr mMaxRenderSize;
  ssc::DoubleDataAdapterXmlPtr mStillUpdateRate;

private slots:
  void renderingIntervalSlot(int interval);
};

class VisualizationTab : public PreferencesTab
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
  ssc::DoubleDataAdapterXmlPtr mSphereRadius;
  ssc::DoubleDataAdapterXmlPtr mLabelSize;
  ssc::BoolDataAdapterXmlPtr mShowDataText;
  ssc::BoolDataAdapterXmlPtr mShowLabels;
  ssc::StringDataAdapterXmlPtr mAnnotationModel;
  ssc::DoubleDataAdapterXmlPtr mAnnotationModelSize;

  //Stereo
  QComboBox* mStereoTypeComboBox;
  ssc::DoubleDataAdapterXmlPtr mEyeAngleAdapter;
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
class AutomationTab : public PreferencesTab
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
class VideoTab : public PreferencesTab
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
 * \beginverbatim
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
class ToolConfigTab : public PreferencesTab
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
  PreferencesDialog(QWidget *parent = 0);
  virtual ~PreferencesDialog();

signals:
  void applied();

private slots:
  void selectTabSlot();
  void applySlot();

protected:
  void addTab(PreferencesTab* widget, QString name);

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
class DebugTab : public PreferencesTab
{
  Q_OBJECT

public:
  DebugTab(QWidget *parent = 0);
  void init();

public slots:
  void saveParametersSlot();

protected:
  QCheckBox* mIGSTKDebugLoggingCheckBox;
  QCheckBox* mManualToolPhysicalPropertiesCheckBox;
  QGridLayout *mMainLayout;
};



/**
 * @}
 */
}//namespace cx

#endif
