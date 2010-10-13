#ifndef CXSEGMENTATIONWIDGET_H_
#define CXSEGMENTATIONWIDGET_H_

#include <vector>
#include <QtGui>
#include "cxWhatsThisWidget.h"
#include <QString>

class QGroupBox;
class QWidget;

namespace cx
{

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

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private slots:
  void segmentSlot();
  void contourSlot();
  void adjustSizeSlot();

private:
  SegmentationWidget();
  QWidget* createSegmentationOptionsWidget();
  QWidget* createCotourOptionsWidget();

  int mThreshold; ///< the threshold value used when segmenting
  bool mBinary; ///< whether or not the segmentation should create a binary volume
  bool mUseSmothing; ///< whether or not the volume should be smoothed
  double mSmoothSigma; ///< the value used for smoothing (if enabled)
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

}//namespace cx

#endif /* CXSEGMENTATIONWIDGET_H_ */
