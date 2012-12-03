#ifndef CXSBINARYTHRESHOLDIMAGEFILTERWIDGET_H_
#define CXSBINARYTHRESHOLDIMAGEFILTERWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscDoubleDataAdapterXml.h"
#include "sscDoubleWidgets.h"
#include "cxBaseWidget.h"
#include "cxContour.h"
#include "cxBinaryThresholdImageFilter.h"
#include "cxThresholdPreview.h"

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
 * \class BinaryThresholdImageFilterWidget
 *
 * \brief Widget for segmenting out parts of volumes using a threshold.
 *
 * \date 12. okt. 2010
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Christian Askeland, SINTEF
 */
class BinaryThresholdImageFilterWidget : public BaseWidget
{
  Q_OBJECT

public:
  BinaryThresholdImageFilterWidget(QWidget* parent);
  virtual ~BinaryThresholdImageFilterWidget();
  virtual QString defaultWhatsThis() const;
  void setDefaultColor(QColor color);
  BinaryThresholdImageFilterOldPtr getSegmentationAlgorithm() { return mSegmentationAlgorithm; }

signals:
  void inputImageChanged(QString uid);
  void outputImageChanged(QString uid);

public slots:
  void setImageInputSlot(QString value);

private slots:
  void segmentSlot();
  void toogleBinarySlot(bool on);
  void thresholdSlot();
  void toogleSurfaceSlot(bool on);
  void toogleSmoothingSlot(bool on);
  void imageChangedSlot(QString uid);
  void handleFinishedSlot();
  void handleContourFinishedSlot();
  void preprocessSegmentation();
  void preprocessContour();
  void obscuredSlot(bool obscured);

private:
  BinaryThresholdImageFilterWidget();
  QWidget* createSegmentationOptionsWidget();

  void generateSurface();

  SelectImageStringDataAdapterPtr mSelectedImage; ///< holds the currently selected image (use setValue/getValue)

  bool mBinary; ///< whether or not the segmentation should create a binary volume
  bool mSurface; ///< Create a surface of the segmented volume
  bool mUseSmothing; ///< whether or not the volume should be smoothed

  ssc::DoubleDataAdapterXmlPtr mSegmentationThresholdAdapter;
  ssc::DoubleDataAdapterXmlPtr mSmoothingSigmaAdapter;
  ssc::SpinBoxAndSliderGroupWidgetPtr mSmoothingSigmaWidget;
  QLabel* mSmoothingSigmaLabel;
  QColor mDefaultColor;
//  QLabel* mStatusLabel;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;

  BinaryThresholdImageFilterOldPtr mSegmentationAlgorithm;
  ContourPtr mContourAlgorithm;
  boost::shared_ptr<WidgetObscuredListener> mObscuredListener;
};

/**
 * @}
 */
}//namespace cx

#endif /* CXSBINARYTHRESHOLDIMAGEFILTERWIDGET_H_ */
