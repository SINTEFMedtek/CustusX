/*
 * cxToolTipSampleWidget.h
 *
 *  \date May 4, 2011
 *      \author christiana
 */

#ifndef CXTOOLTIPSAMPLEWIDGET_H_
#define CXTOOLTIPSAMPLEWIDGET_H_

#include "cxBaseWidget.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscForwardDeclarations.h"
#include "cxDataInterface.h"

class QPushButton;
class QGroupBox;
class QLineEdit;

namespace cx
{
typedef boost::shared_ptr<class SelectDataStringDataAdapter> SelectDataStringDataAdapterPtr;

/**
 * \file
 * \addtogroup cxPluginCalibration
 * @{
 */

/**
 * Class for sampling points in a chosable coordinate system and then saving them to file.
 */
class ToolTipSampleWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolTipSampleWidget(QWidget* parent);
  ~ToolTipSampleWidget();
  virtual QString defaultWhatsThis() const;

private slots:
  void saveFileSlot();
  void sampleSlot();
  void coordinateSystemChanged();

private:
  ssc::CoordinateSystem getSelectedCoordinateSystem();

  QPushButton* mSampleButton;
  QLabel*      mSaveToFileNameLabel;
  QPushButton* mSaveFileButton;
  SelectCoordinateSystemStringDataAdapterPtr mCoordinateSystems;
  SelectToolStringDataAdapterPtr mTools;
  SelectDataStringDataAdapterPtr mData;
  ssc::LabeledComboBoxWidget* mCoordinateSystemComboBox;
  ssc::LabeledComboBoxWidget* mToolComboBox;
  ssc::LabeledComboBoxWidget* mDataComboBox;
  bool mTruncateFile;
};


/**
 * @}
 */
}

#endif /* CXTOOLTIPSAMPLEWIDGET_H_ */
