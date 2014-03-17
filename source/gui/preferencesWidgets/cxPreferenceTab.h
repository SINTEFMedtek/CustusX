#ifndef CXPREFERENCETAB_H_
#define CXPREFERENCETAB_H_

#include "cxDoubleDataAdapterXml.h"
#include "cxStringDataAdapterXml.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QGridLayout;
class QVBoxLayout;

namespace cx
{

class PreferenceTab : public QWidget
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
class PerformanceTab : public PreferenceTab
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
//  QCheckBox* mGPU3DDepthPeelingCheckBox;
  QCheckBox* mShadingCheckBox;
  QGridLayout* mMainLayout;
  DoubleDataAdapterXmlPtr mMaxRenderSize;
  DoubleDataAdapterXmlPtr mStillUpdateRate;
  StringDataAdapterXmlPtr m3DVisualizer;

private slots:
  void renderingIntervalSlot(int interval);
};

} /* namespace cx */
#endif /* CXPREFERENCETAB_H_ */
