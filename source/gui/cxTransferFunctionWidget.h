#ifndef CXTRANSFERFUNCTIONWIDGET_H_
#define CXTRANSFERFUNCTIONWIDGET_H_

#include <QWidget>
#include <QCheckBox>
#include <QDomElement>
#include <sscImage.h>
#include "cxPresetTransferFunctions3D.h"

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
  void activeImageChangedSlot(); ///< listens to the contextdockwidget for when the current image is changed
  void shadingToggledSlot(bool val);
  void presetsBoxChangedSlot(const QString& presetName);


protected:
  //gui
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  //TransferFunctionInfoWidget*  mInfoWidget;
  QCheckBox* mShadingCheckBox;
  QComboBox* mPresetsComboBox;

  PresetTransferFunctions3D mPresets;
  //QStringList* mPresets;
  //QDomElement mTransferfunctionPresetCTFire;
  //QDomElement mTransferfunctionPresetCTBlue;

  ssc::ImagePtr mCurrentImage;
	bool mInitialized;///< Is TransferFunctionWidget initialized
  
  //void initTransferFunctionPresets();
};
}

#endif /* CXTRANSFERFUNCTIONWIDGET_H_ */
