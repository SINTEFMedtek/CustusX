/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRANSFERFUNCTIONWIDGET_H_
#define CXTRANSFERFUNCTIONWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include <QCheckBox>
#include <QDomElement>
#include "cxImage.h"
#include "cxTransferFunctions3DPresets.h"
#include "cxDoublePropertyBase.h"
#include "cxActiveImageProxy.h"

class QVBoxLayout;
class QComboBox;
class QStringList;
class QAction;
class QActionGroup;

namespace cx
{
class TransferFunctionAlphaWidget;
class TransferFunctionColorWidget;

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/** Superclass for all doubles interacting with a ImageTFData.
 */
class cxGui_EXPORT DoublePropertyImageTFDataBase : public DoublePropertyBase
{
  Q_OBJECT
public:
  DoublePropertyImageTFDataBase();
  virtual ~DoublePropertyImageTFDataBase() {}
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}
public:
  void setImageTFData(ImageTFDataPtr tfData, ImagePtr image);
protected:
  virtual double getValueInternal() const = 0;
  virtual void setValueInternal(double val) = 0;

  ImageTFDataPtr mImageTFData;
  ImagePtr mImage;
};
typedef boost::shared_ptr<DoublePropertyImageTFDataBase> DoublePropertyImageTFDataBasePtr;


/**DataInterface implementation for the tf window value
 */
class cxGui_EXPORT DoublePropertyImageTFDataWindow : public DoublePropertyImageTFDataBase
{
public:
  virtual ~DoublePropertyImageTFDataWindow() {}
  virtual QString getDisplayName() const { return "Window width"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf level value
 */
class cxGui_EXPORT DoublePropertyImageTFDataLevel : public DoublePropertyImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoublePropertyImageTFDataLevel() {}
  virtual QString getDisplayName() const { return "Window level"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf llr value
 */
class cxGui_EXPORT DoublePropertyImageTFDataLLR : public DoublePropertyImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoublePropertyImageTFDataLLR() {}
  virtual QString getDisplayName() const { return "LLR"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf alpha value
 */
class cxGui_EXPORT DoublePropertyImageTFDataAlpha : public DoublePropertyImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoublePropertyImageTFDataAlpha() {}
  virtual QString getDisplayName() const { return "Alpha"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

class cxGui_EXPORT TransferFunction3DWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunction3DWidget(ActiveDataPtr activeData, QWidget* parent, bool connectToActiveImage = true);
  virtual ~TransferFunction3DWidget() {}

public slots:
  void activeImageChangedSlot();

  void imageChangedSlot(ImagePtr image);
protected:
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;

  ActiveImageProxyPtr mActiveImageProxy;
  ActiveDataPtr mActiveData;
};

class cxGui_EXPORT TransferFunction2DWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunction2DWidget(ActiveDataPtr activeData, QWidget* parent, bool connectToActiveImage = true);
  virtual ~TransferFunction2DWidget() {}

public slots:
  void activeImageChangedSlot();

protected:
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  DoublePropertyImageTFDataBasePtr mDataWindow, mDataAlpha, mDataLLR, mDataLevel;

  ActiveImageProxyPtr mActiveImageProxy;
  ActiveDataPtr mActiveData;
};


/**
 * \class TransferFunctionWidget
 *
 * \brief
 *
 * \date Mar 23, 2009
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT TransferFunctionWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunctionWidget(PatientModelServicePtr patientModelService, QWidget* parent, bool connectToActiveImage = true);
  virtual ~TransferFunctionWidget() {}
};

/**
 * @}
 */
}

#endif /* CXTRANSFERFUNCTIONWIDGET_H_ */
