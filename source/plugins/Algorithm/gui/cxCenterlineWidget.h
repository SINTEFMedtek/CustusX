/*
 * cxCenterlineWidget.h
 *
 *  \date Apr 29, 2011
 *      \author christiana
 */

#ifndef CXCENTERLINEWIDGET_H_
#define CXCENTERLINEWIDGET_H_

#include <vector>
#include <QtGui>
#include "cxBaseWidget.h"
#include "cxCenterline.h"

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
 * \class CenterlineWidget
 *
 * \brief Widget for extracting the centerline from a segment.
 *
 * \date 12. okt. 2010
 * \\author Janne Beate Bakeng
 */
class CenterlineWidget : public BaseWidget
{
  Q_OBJECT

public:
  CenterlineWidget(QWidget* parent);
  ~CenterlineWidget();
  virtual QString defaultWhatsThis() const;
  void setDefaultColor(QColor color);
  CenterlinePtr  getCenterlineAlgorithm() { return mCenterlineAlgorithm; }

public slots:
  void setImageInputSlot(QString value);
  void findCenterlineSlot();

signals:
void inputImageChanged(QString uid);
void outputImageChanged(QString uid);

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private slots:
  void handleFinishedSlot();
  void preprocessResampler();

private:
  SelectImageStringDataAdapterPtr mSelectedImage; ///< holds the currently selected image (use setValue/getValue)
  QPushButton* mFindCenterlineButton;///<Button for finding centerline in a segment
//  QLabel* mStatusLabel;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;

  CenterlinePtr  mCenterlineAlgorithm;
};

/**
 * @}
 */
}

#endif /* CXCENTERLINEWIDGET_H_ */
