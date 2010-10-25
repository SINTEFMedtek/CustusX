#ifndef CXSEGMENTATIONWIDGET_H_
#define CXSEGMENTATIONWIDGET_H_

#include <vector>
#include <QtGui>
#include <QString>
#include "sscForwardDeclarations.h"
#include "cxWhatsThisWidget.h"

class QGroupBox;
class QWidget;
class QDoubleSpinBox;

namespace cx
{
typedef boost::shared_ptr<class SelectImageStringDataAdapter> SelectImageStringDataAdapterPtr;

/**
 * \class SegmentationWidget
 *
 * \brief Widget for segmenting out parts of volumes using a threshold.
 *
 * \date 12. okt. 2010
 * \author: Janne Beate Bakeng
 * \author: Christian Askeland, SINTEF
 */
class SegmentationWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  SegmentationWidget(QWidget* parent);
  virtual ~SegmentationWidget();
  virtual QString defaultWhatsThis() const;

signals:
  void inputImageChanged(QString uid);
  void outputImageChanged(QString uid);

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private slots:
  void segmentSlot();
  void toogleBinarySlot(bool on);
  void thresholdSlot(int value);
  void toogleSmoothingSlot(bool on);
  void smoothingSigmaSlot(double value);
  void imageChangedSlot(QString uid);

private:
  SegmentationWidget();
  QWidget* createSegmentationOptionsWidget();

  //ssc::ImagePtr mImage;
  SelectImageStringDataAdapterPtr mSelectedImage; ///< holds the currently selected image (use setValue/getValue)

  int mSegmentationThreshold; ///< the threshold value used when segmenting
  bool mBinary; ///< whether or not the segmentation should create a binary volume
  bool mUseSmothing; ///< whether or not the volume should be smoothed
  double mSmoothSigma; ///< the value used for smoothing (if enabled)

  QSpinBox* mSegmentationThresholdSpinBox;
  QDoubleSpinBox* mSmoothingSigmaSpinBox;
  QLabel* mSmoothingSigmaLabel;
};

/**
 * \class SurfaceWidget
 *
 * \brief Widget for finding the surface of a binary volume using marching cubes.
 *
 * \date 14. okt. 2010
 * \author: Janne Beate Bakeng
 */
class SurfaceWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  SurfaceWidget(QWidget* parent);
  ~SurfaceWidget();
  virtual QString defaultWhatsThis() const;

public slots:
  void setImageInputSlot(QString value);

signals:
  void inputImageChanged(QString uid);
  void outputMeshChanged(QString uid);

private slots:
  void surfaceSlot();
  void thresholdSlot(int value);
  void decimationSlot(int value);
  void reduceResolutionSlot(bool value);
  void smoothingSlot(bool value);
  void imageChangedSlot(QString uid);

private:
  SurfaceWidget();
  QWidget* createSurfaceOptionsWidget();

  SelectImageStringDataAdapterPtr mSelectedImage; ///< holds the currently selected image (use setValue/getValue)
  int mSurfaceThreshold; ///< the threshold value used when contouring
  int mDecimation;
  bool mReduceResolution;
  bool mSmoothing;
  QSpinBox* mSurfaceThresholdSpinBox;
  QSpinBox* mDecimationSpinBox;
};

/**
 * \class CenterlineWidget
 *
 * \brief Widget for extracting the centerline from a segment.
 *
 * \date 12. okt. 2010
 * \author: Janne Beate Bakeng
 */
class CenterlineWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  CenterlineWidget(QWidget* parent);
  ~CenterlineWidget();
  virtual QString defaultWhatsThis() const;

public slots:
  void setImageInputSlot(QString value);

signals:
void inputImageChanged(QString uid);
void outputImageChanged(QString uid);

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private slots:
  void findCenterlineSlot();

private:
  SelectImageStringDataAdapterPtr mSelectedImage; ///< holds the currently selected image (use setValue/getValue)
  QPushButton* mFindCenterlineButton;///<Button for finding centerline in a segment
};

/**
 * \class RegisterI2IWidget
 *
 * \brief Widget for performing the registration between two vessel segments.
 *
 * \date 13. okt. 2010
 * \author: Janne Beate Bakeng
 */
class RegisterI2IWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  RegisterI2IWidget(QWidget* parent);
  ~RegisterI2IWidget();
  virtual QString defaultWhatsThis() const;

public slots:
  void fixedImageSlot(QString uid);
  void movingImageSlot(QString uid);

private slots:
  void testSlot();

private:
  RegisterI2IWidget();

  QPushButton* mRegisterButton;
  QPushButton* mTestButton;
  QLabel* mFixedImageLabel;
  QLabel* mMovingImageLabel;

  ssc::ImagePtr mFixedImage;
  ssc::ImagePtr mMovingImage;
};

}//namespace cx

#endif /* CXSEGMENTATIONWIDGET_H_ */
