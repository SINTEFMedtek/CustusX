#ifndef CXTRANSFERFUNCTIONWIDGET_H_
#define CXTRANSFERFUNCTIONWIDGET_H_

#include <QWidget>
#include "sscImage.h"

class QVBoxLayout;

namespace cx
{
class TransferFunctionAlphaWidget;
class TransferFunctionColorWidget;

/**
 * \class TransferFunctionWidget
 *
 * \brief
 *
 * \date Mar 23, 2009
 * \author: Janne Beate Bakeng, SINTEF
 * \author: Ole Vegard Solberg, SINTEF
 */
class TransferFunctionWidget : public QWidget
{
  Q_OBJECT

public:
  TransferFunctionWidget(QWidget* parent);
  ~TransferFunctionWidget();

public slots:
  void currentImageChangedSlot(ssc::ImagePtr currentImage); ///< listens to the contextdockwidget for when the current image is changed

signals:
  void currentImageChanged(ssc::ImagePtr currentImage); ///< sends out a signal when the user chooses a different image to work on

protected:
  //gui
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  //TransferFunctionInfoWidget*  mInfoWidget;

  ssc::ImagePtr mCurrentImage;
};
}

#endif /* CXTRANSFERFUNCTIONWIDGET_H_ */
