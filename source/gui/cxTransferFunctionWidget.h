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

public slots:
  void presetsBoxChangedSlot(const QString& presetName);
  void resetSlot();
  void saveSlot();
  void activeImageChangedSlot();

protected:
  void init();///< Initialize TransferFunctionWidget. Create members
  //gui
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  QComboBox* mPresetsComboBox;

  PresetTransferFunctions3D mPresets;

	bool mInitialized;///< Is TransferFunctionWidget initialized
};
}

#endif /* CXTRANSFERFUNCTIONWIDGET_H_ */
