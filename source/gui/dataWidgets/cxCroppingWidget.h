#ifndef CXCROPPINGWIDGET_H_
#define CXCROPPINGWIDGET_H_

#include "cxBaseWidget.h"
#include "sscForwardDeclarations.h"
#include "sscStringDataAdapter.h"
#include "cxViewManager.h"
#include "cxDoubleSpanSlider.h"

typedef boost::shared_ptr<class InteractiveCropper> InteractiveCropperPtr;
class QCheckBox;

namespace cx
{
/**
 * \file
 * \addtogroup cxGUI
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

  QCheckBox* mUseCropperCheckBox;
  QCheckBox* mShowBoxCheckBox;
  SliderRangeGroupWidget* mXRange;
  SliderRangeGroupWidget* mYRange;
  SliderRangeGroupWidget* mZRange;

private slots:
  void boxValuesChanged();
  void cropperChangedSlot();
  ssc::ImagePtr cropClipButtonClickedSlot();///< Crete a new image based on the images crop and clip values
};

/**
 * @}
 */
}//namespace cx


#endif /* CXCROPPINGWIDGET_H_ */
