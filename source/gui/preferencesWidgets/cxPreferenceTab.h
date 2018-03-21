/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPREFERENCETAB_H_
#define CXPREFERENCETAB_H_

#include "cxGuiExport.h"

#include "cxDoubleProperty.h"
#include "cxStringProperty.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QGridLayout;
class QVBoxLayout;

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

class cxGui_EXPORT PreferenceTab : public QWidget
{
  Q_OBJECT
public:
	PreferenceTab(QWidget *parent = 0);

  virtual void init() = 0;

public slots:
  virtual void saveParametersSlot() = 0;

signals:
  void savedParameters();

protected:
  QVBoxLayout* mTopLayout;
};

/**
 * \class PerformanceTab
 *
 * \brief Configure performance tab in preferences dialog
 *
 * \date Mar 8, 2010
 * \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT PerformanceTab : public PreferenceTab
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
  QCheckBox* mGPU2DRenderCheckBox;
	QCheckBox* mLinearInterpolationIn2DCheckBox;
  QCheckBox* mOptimizedViewsCheckBox;
  QCheckBox* mGPU3DDepthPeelingCheckBox;
  QCheckBox* mShadingCheckBox;
  QGridLayout* mMainLayout;
  DoublePropertyPtr mMaxRenderSize;
  DoublePropertyPtr mStillUpdateRate;
  StringPropertyPtr m3DVisualizer;

private slots:
  void renderingIntervalSlot(int interval);
  static QStringList getAvailableVisualizers();
  static std::map<QString, QString> getAvailableVisualizerDisplayNames();
};

} /* namespace cx */
#endif /* CXPREFERENCETAB_H_ */
