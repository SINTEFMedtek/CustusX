/*
 * cxRegisterI2IWidget.h
 *
 *  Created on: Apr 29, 2011
 *      Author: christiana
 */

#ifndef CXREGISTERI2IWIDGET_H_
#define CXREGISTERI2IWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscDoubleDataAdapterXml.h"
#include "sscDoubleWidgets.h"
#include "cxWhatsThisWidget.h"
#include "cxBinaryThresholdImageFilter.h"


namespace cx
{
typedef boost::shared_ptr<class SelectImageStringDataAdapter> SelectImageStringDataAdapterPtr;
class SeansVesselRegistrationWidget;


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

private:
  RegisterI2IWidget();

  SeansVesselRegistrationWidget* mSeansVesselRegsitrationWidget;
};

}

#endif /* CXREGISTERI2IWIDGET_H_ */
