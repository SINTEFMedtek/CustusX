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

//ShadingWidget

/*
 *
 */
class TransferFunction3DWidget : public QWidget
{
  Q_OBJECT

public:
  TransferFunction3DWidget(QWidget* parent);
  virtual ~TransferFunction3DWidget();

public slots:
  void activeImageChangedSlot();

protected:
  void init();
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
};

/*
 *
 */
class TransferFunction2DWidget : public QWidget
{
  Q_OBJECT

public:
  TransferFunction2DWidget(QWidget* parent);
  virtual ~TransferFunction2DWidget();

public slots:
  void activeImageChangedSlot();

protected:
  void init();
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
};

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
  virtual ~TransferFunctionWidget();

public slots:
  void presetsBoxChangedSlot(const QString& presetName);
  void resetSlot();
  void saveSlot();
//  void activeImageChangedSlot();

protected:
  void init();///< Initialize TransferFunctionWidget. Create members
  //gui
  QVBoxLayout* mLayout;
//  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
//  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  QComboBox* mPresetsComboBox;
  TransferFunction3DWidget* mTF3DWidget;
  TransferFunction2DWidget* mTF2DWidget;

  PresetTransferFunctions3D mPresets;

//	bool mInitialized;///< Is TransferFunctionWidget initialized
};

}

#endif /* CXTRANSFERFUNCTIONWIDGET_H_ */
