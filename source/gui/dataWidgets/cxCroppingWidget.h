#ifndef CXCROPPINGWIDGET_H_
#define CXCROPPINGWIDGET_H_

#include "sscForwardDeclarations.h"
#include "sscStringDataAdapter.h"
#include "cxViewManager.h"
#include "cxBaseWidget.h"
#include "cxLegacySingletons.h"

#include "cxBoundingBoxWidget.h"

typedef boost::shared_ptr<class InteractiveCropper> InteractiveCropperPtr;
class QCheckBox;
class QLabel;

namespace cx
{
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/**
 * \class CroppingWidget
 *
 * \date  Aug 20, 2010
 * \author Christian Askeland, SINTEF
 */
class CroppingWidget : public BaseWidget
{
  Q_OBJECT

public:
  CroppingWidget(QWidget* parent);
  virtual QString defaultWhatsThis() const;

private:
  InteractiveCropperPtr mInteractiveCropper;
  BoundingBoxWidget* mBBWidget;
  QCheckBox* mUseCropperCheckBox;
  QCheckBox* mShowBoxCheckBox;
  QLabel* mBoundingBoxDimensions;

private slots:
  void boxValuesChanged();
  void cropperChangedSlot();
  ImagePtr cropClipButtonClickedSlot();///< Crete a new image based on the images crop and clip values
};

/**
 * @}
 */
}//namespace cx


#endif /* CXCROPPINGWIDGET_H_ */
