#ifndef CXTRANSFERFUNCTIONWIDGET_H_
#define CXTRANSFERFUNCTIONWIDGET_H_

#include <QWidget>
#include <QCheckBox>
#include <QDomElement>
#include <sscImage.h>
#include "cxPresetTransferFunctions3D.h"
#include "sscDoubleDataAdapter.h"

class QVBoxLayout;
class QComboBox;
class QStringList;

namespace cx
{
class TransferFunctionAlphaWidget;
class TransferFunctionColorWidget;

/** Superclass for all doubles interacting with a ImageTFData.
 */
class DoubleDataAdapterImageTFDataBase : public ssc::DoubleDataAdapter
{
  Q_OBJECT
public:
  DoubleDataAdapterImageTFDataBase();
  virtual ~DoubleDataAdapterImageTFDataBase() {}
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}
public:
  void setImageTFData(ssc::ImageTFDataPtr tfData);
protected:
  virtual double getValueInternal() const = 0;
  virtual void setValueInternal(double val) = 0;

  ssc::ImageTFDataPtr mImageTFData;
};
typedef boost::shared_ptr<DoubleDataAdapterImageTFDataBase> DoubleDataAdapterImageTFDataBasePtr;


/**DataInterface implementation for the tf window value
 */
class DoubleDataAdapterImageTFDataWindow : public DoubleDataAdapterImageTFDataBase
{
public:
  virtual ~DoubleDataAdapterImageTFDataWindow() {}
  virtual QString getValueName() const { return "Window"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual ssc::DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf level value
 */
class DoubleDataAdapterImageTFDataLevel : public DoubleDataAdapterImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapterImageTFDataLevel() {}
  virtual QString getValueName() const { return "Level"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual ssc::DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf llr value
 */
class DoubleDataAdapterImageTFDataLLR : public DoubleDataAdapterImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapterImageTFDataLLR() {}
  virtual QString getValueName() const { return "LLR"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual ssc::DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf alpha value
 */
class DoubleDataAdapterImageTFDataAlpha : public DoubleDataAdapterImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapterImageTFDataAlpha() {}
  virtual QString getValueName() const { return "Alpha"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual ssc::DoubleRange getValueRange() const;
};


/*
 *
 */
class TransferFunction3DWidget : public QWidget
{
  Q_OBJECT

public:
  TransferFunction3DWidget(QWidget* parent);
  virtual ~TransferFunction3DWidget() {}

public slots:
  void activeImageChangedSlot();

protected:
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  DoubleDataAdapterImageTFDataBasePtr mDataWindow, mDataAlpha, mDataLLR, mDataLevel;
};

/*
 *
 */
class TransferFunction2DWidget : public QWidget
{
  Q_OBJECT

public:
  TransferFunction2DWidget(QWidget* parent);
  virtual ~TransferFunction2DWidget() {}

public slots:
  void activeImageChangedSlot();

protected:
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  DoubleDataAdapterImageTFDataBasePtr mDataWindow, mDataAlpha, mDataLLR, mDataLevel;
};

class TransferFunctionPresetWidget : public QWidget
{
  Q_OBJECT

public:
  TransferFunctionPresetWidget(QWidget* parent);
  virtual ~TransferFunctionPresetWidget() {}

public slots:
  void presetsBoxChangedSlot(const QString& presetName);
  void resetSlot();
  void saveSlot();

protected:
  QVBoxLayout* mLayout;
  QComboBox* mPresetsComboBox;
  PresetTransferFunctions3D mPresets;
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
  virtual ~TransferFunctionWidget() {}
public slots:
protected:
  QVBoxLayout* mLayout;
  TransferFunction3DWidget* mTF3DWidget;
  TransferFunction2DWidget* mTF2DWidget;
  TransferFunctionPresetWidget* mTFPresetWidget;

};

}

#endif /* CXTRANSFERFUNCTIONWIDGET_H_ */
