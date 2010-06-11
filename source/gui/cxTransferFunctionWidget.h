#ifndef CXTRANSFERFUNCTIONWIDGET_H_
#define CXTRANSFERFUNCTIONWIDGET_H_

#include <QWidget>
#include <QCheckBox>
#include <QDomElement>
#include <sscImage.h>

class QVBoxLayout;
class QComboBox;
class QStringList;

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
	void init();///< Initialize TransferFunctionWidget. Create members

public slots:
  //void currentImageChangedSlot(ssc::ImagePtr currentImage); ///< listens to the contextdockwidget for when the current image is changed
  void activeImageChangedSlot(); ///< listens to the contextdockwidget for when the current image is changed
  void shadingToggledSlot(bool val);
  void presetsBoxChangedSlot(int val);

/*signals:
  void currentImageChanged(ssc::ImagePtr currentImage); ///< sends out a signal when the user chooses a different image to work on*/

protected:
  //gui
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  //TransferFunctionInfoWidget*  mInfoWidget;
  QCheckBox* mShadingCheckBox;
  QComboBox* mPresetsComboBox;
  QStringList* mPresets;
  QDomElement mTransferfunctionPresetCTFire;
  QDomElement mTransferfunctionPresetCTBlue;

  ssc::ImagePtr mCurrentImage;
	bool mInitialized;///< Is TransferFunctionWidget initialized
  
  void initTransferFunctionPresets();
};
}

#endif /* CXTRANSFERFUNCTIONWIDGET_H_ */
