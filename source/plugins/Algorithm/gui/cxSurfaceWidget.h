/*
 * cxSurfaceWidget.h
 *
 *  \date Apr 29, 2011
 *      \author christiana
 */

#ifndef CXSURFACEWIDGET_H_
#define CXSURFACEWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscDoubleDataAdapterXml.h"
#include "cxBaseWidget.h"
#include "cxContour.h"

namespace cx
{
typedef boost::shared_ptr<class SelectImageStringDataAdapter> SelectImageStringDataAdapterPtr;
class TimedAlgorithmProgressBar;

/**
 * \file
 * \addtogroup cxPluginAlgorithm
 * @{
 */

/**
 * \class SurfaceWidget
 *
 * \brief Widget for finding the surface of a binary volume using marching cubes.
 *
 * \date 14. okt. 2010
 * \\author Janne Beate Bakeng
 */
class SurfaceWidget : public BaseWidget
{
  Q_OBJECT

public:
  SurfaceWidget(QWidget* parent);
  ~SurfaceWidget();
  virtual QString defaultWhatsThis() const;
  void setDefaultColor(QColor color);

public slots:
  void setImageInputSlot(QString value);

signals:
  void inputImageChanged(QString uid);
  void outputMeshChanged(QString uid);

private slots:
  void surfaceSlot();
  void reduceResolutionSlot(bool value);
  void smoothingSlot(bool value);
  void preserveSlot(bool value);
  void imageChangedSlot(QString uid);
  void handleFinishedSlot();
  void thresholdSlot();
  void preprocessContour();

private:
  SurfaceWidget();
  QWidget* createSurfaceOptionsWidget();

  SelectImageStringDataAdapterPtr mSelectedImage; ///< holds the currently selected image (use setValue/getValue)
  bool mReduceResolution;
  bool mSmoothing;
  bool mPreserveTopology;
  ssc::DoubleDataAdapterXmlPtr mSurfaceThresholdAdapter;
  ssc::DoubleDataAdapterXmlPtr mDecimationAdapter;
  QColor mDefaultColor;
//  QLabel* mStatusLabel;
  QCheckBox* mReduceResolutionCheckBox;
	cx::TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;

	ContourPtr mContourAlgorithm;
};

/**
 * @}
 */
}

#endif /* CXSURFACEWIDGET_H_ */
