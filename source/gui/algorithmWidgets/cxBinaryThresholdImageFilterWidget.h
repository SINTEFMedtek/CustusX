#ifndef CXSBINARYTHRESHOLDIMAGEFILTERWIDGET_H_
#define CXSBINARYTHRESHOLDIMAGEFILTERWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscDoubleDataAdapterXml.h"
#include "sscDoubleWidgets.h"
#include "cxBaseWidget.h"
#include "cxCenterline.h"
#include "cxResample.h"
#include "cxBinaryThresholdImageFilter.h"

namespace cx
{
typedef boost::shared_ptr<class SelectImageStringDataAdapter> SelectImageStringDataAdapterPtr;

/**
 * \class BinaryThresholdImageFilterWidget
 *
 * \brief Widget for segmenting out parts of volumes using a threshold.
 *
 * \date 12. okt. 2010
 * \author: Janne Beate Bakeng, SINTEF
 * \author: Christian Askeland, SINTEF
 */
class BinaryThresholdImageFilterWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  BinaryThresholdImageFilterWidget(QWidget* parent);
  virtual ~BinaryThresholdImageFilterWidget();
  virtual QString defaultWhatsThis() const;

signals:
  void inputImageChanged(QString uid);
  void outputImageChanged(QString uid);

public slots:
  void setImageInputSlot(QString value);

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event); ///<disconnects stuff

private slots:
  void removeIfNotVisible(); ///<Remove segmentation preview coloring if widget is not visible
  void segmentSlot();
  void toogleBinarySlot(bool on);
  void thresholdSlot();
  void toogleSmoothingSlot(bool on);
  void imageChangedSlot(QString uid);
  void revertTransferFunctions();
  void handleFinishedSlot();

private:
  BinaryThresholdImageFilterWidget();
  QWidget* createSegmentationOptionsWidget();

  SelectImageStringDataAdapterPtr mSelectedImage; ///< holds the currently selected image (use setValue/getValue)

  bool mBinary; ///< whether or not the segmentation should create a binary volume
  bool mUseSmothing; ///< whether or not the volume should be smoothed

  ssc::DoubleDataAdapterXmlPtr mSegmentationThresholdAdapter;
  ssc::DoubleDataAdapterXmlPtr mSmoothingSigmaAdapter;
  ssc::SpinBoxAndSliderGroupWidgetPtr mSmoothingSigmaWidget;
  QLabel* mSmoothingSigmaLabel;
  QLabel* mStatusLabel;

  ssc::ImagePtr mModifiedImage; ///< image that have its TF changed temporarily
  ssc::ImageTF3DPtr mTF3D_original; ///< original TF of modified image.
  ssc::ImageLUT2DPtr mTF2D_original; ///< original TF of modified image.
  bool mShadingOn_original; ///< Was shading originally enabled in image
  QTimer *mRemoveTimer;///< Timer for removing segmentation preview coloring if widget is not visible

  BinaryThresholdImageFilter mSegmentationAlgorithm;
};






}//namespace cx

#endif /* CXSBINARYTHRESHOLDIMAGEFILTERWIDGET_H_ */
