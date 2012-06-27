/*
 * cxResampleWidget.h
 *
 *  \date Apr 29, 2011
 *      \author christiana
 */

#ifndef CXRESAMPLEWIDGET_H_
#define CXRESAMPLEWIDGET_H_

#include <vector>
#include <QtGui>
#include <QString>

#include "sscForwardDeclarations.h"
#include "cxBaseWidget.h"
#include "cxResample.h"
#include "sscDoubleDataAdapterXml.h"

namespace cx
{
typedef boost::shared_ptr<class SelectImageStringDataAdapter> SelectImageStringDataAdapterPtr;

/**
 * \file
 * \addtogroup cxPluginAlgorithm
 * @{
 */

class ResampleWidget : public BaseWidget
{
  Q_OBJECT

public:
  ResampleWidget(QWidget* parent);
  virtual ~ResampleWidget();
  virtual QString defaultWhatsThis() const;

signals:
  void inputImageChanged(QString uid);
  void outputImageChanged(QString uid);

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event); ///<disconnects stuff

private slots:
  void resampleSlot();
  void handleFinishedSlot();
  void preprocessResampler();

private:
  ResampleWidget();
  QWidget* createOptionsWidget();

  SelectImageStringDataAdapterPtr mSelectedImage; ///< holds the currently selected image (use setValue/getValue)
  SelectImageStringDataAdapterPtr mReferenceImage; ///< holds the currently reference, resample to this volume.
  ssc::DoubleDataAdapterXmlPtr mMargin;
  Resample mResampleAlgorithm;

  QLabel* mStatusLabel;
};

/**
 * @}
 */
}

#endif /* CXRESAMPLEWIDGET_H_ */
