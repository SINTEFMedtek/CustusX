/*
 * cxShadingWidget.h
 *
 *  \date Aug 20, 2010
 *      \author christiana
 */

#ifndef CXSHADINGWIDGET_H_
#define CXSHADINGWIDGET_H_

#include <QWidget>
#include <QCheckBox>
#include <QDomElement>
#include <sscImage.h>
#include "sscTransferFunctions3DPresets.h"
#include "cxActiveImageProxy.h"
#include "cxBaseWidget.h"

class QVBoxLayout;
class QComboBox;
class QStringList;

namespace cx
{

/**
 * \ingroup cxGUI
 */
class ShadingWidget : public BaseWidget
{
  Q_OBJECT

public:
  ShadingWidget(QWidget* parent);
  ~ShadingWidget();

public slots:
  void activeImageChangedSlot(); ///< listens to the contextdockwidget for when the current image is changed
  void shadingToggledSlot(bool val);
  virtual QString defaultWhatsThis() const;

protected:
  void init();///< Initialize TransferFunctionWidget. Create members
  //gui
  QVBoxLayout* mLayout;
  QCheckBox* mShadingCheckBox;

  //ImagePtr mCurrentImage;
  bool mInitialized;///< Is TransferFunctionWidget initialized
  ActiveImageProxyPtr mActiveImageProxy;
};
}

#endif /* CXSHADINGWIDGET_H_ */
