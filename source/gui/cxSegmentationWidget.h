#ifndef CXSEGMENTATIONWIDGET_H_
#define CXSEGMENTATIONWIDGET_H_

#include <vector>
#include <QtGui>
#include <QString>
#include "sscForwardDeclarations.h"
#include "cxWhatsThisWidget.h"

class QGroupBox;
class QWidget;

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
  void imageChanged(QString);

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private slots:
  void segmentSlot();
  void toogleBinarySlot(bool on);
  void thresholdSlot(int value);
  void toogleSmoothingSlot(bool on);
  void smoothingSigmaSlot(int value);
  //void adjustSizeSlot();
  //void activeImageChangedSlot();

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
  QSpinBox* mSmoothingSigmaSpinBox;
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

signals:
  void imageChanged(QString);

private slots:
  void surfaceSlot();
  void thresholdSlot(int value);

private:
  SurfaceWidget();
  QWidget* createSurfaceOptionsWidget();

  SelectImageStringDataAdapterPtr mSelectedImage; ///< holds the currently selected image (use setValue/getValue)
  int mSurfaceThreshold; ///< the threshold value used when contouring
  QSpinBox* mSurfaceThresholdSpinBox;
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

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private slots:
  void findCenterlineSlot();

private:
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
  void fixedImageSlot(const std::string uid);
  void movingImageSlot(const std::string uid);

private:
  RegisterI2IWidget();

  QPushButton* mRegisterButton;
  QLabel* mFixedImageLabel;
  QLabel* mMovingImageLabel;

  ssc::ImagePtr mFixedImage;
  ssc::ImagePtr mMovingImage;
};

}//namespace cx

#endif /* CXSEGMENTATIONWIDGET_H_ */
