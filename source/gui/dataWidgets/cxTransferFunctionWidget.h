/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXTRANSFERFUNCTIONWIDGET_H_
#define CXTRANSFERFUNCTIONWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include <QCheckBox>
#include <QDomElement>
#include "cxImage.h"
#include "cxTransferFunctions3DPresets.h"
#include "cxDoubleDataAdapter.h"
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
class cxGui_EXPORT DoubleDataAdapterImageTFDataBase : public DoubleDataAdapter
{
  Q_OBJECT
public:
  DoubleDataAdapterImageTFDataBase();
  virtual ~DoubleDataAdapterImageTFDataBase() {}
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
typedef boost::shared_ptr<DoubleDataAdapterImageTFDataBase> DoubleDataAdapterImageTFDataBasePtr;


/**DataInterface implementation for the tf window value
 */
class cxGui_EXPORT DoubleDataAdapterImageTFDataWindow : public DoubleDataAdapterImageTFDataBase
{
public:
  virtual ~DoubleDataAdapterImageTFDataWindow() {}
  virtual QString getDisplayName() const { return "Window width"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf level value
 */
class cxGui_EXPORT DoubleDataAdapterImageTFDataLevel : public DoubleDataAdapterImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapterImageTFDataLevel() {}
  virtual QString getDisplayName() const { return "Window level"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf llr value
 */
class cxGui_EXPORT DoubleDataAdapterImageTFDataLLR : public DoubleDataAdapterImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapterImageTFDataLLR() {}
  virtual QString getDisplayName() const { return "LLR"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf alpha value
 */
class cxGui_EXPORT DoubleDataAdapterImageTFDataAlpha : public DoubleDataAdapterImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapterImageTFDataAlpha() {}
  virtual QString getDisplayName() const { return "Alpha"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

class cxGui_EXPORT TransferFunction3DWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunction3DWidget(PatientModelServicePtr patientModelService, QWidget* parent);
  virtual ~TransferFunction3DWidget() {}
  virtual QString defaultWhatsThis() const;

public slots:
  void activeImageChangedSlot();

protected:
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
//  DoubleDataAdapterImageTFDataBasePtr mDataWindow, mDataAlpha, mDataLLR, mDataLevel;

  ActiveImageProxyPtr mActiveImageProxy;
  PatientModelServicePtr mPatientModelService;
};

class cxGui_EXPORT TransferFunction2DWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunction2DWidget(PatientModelServicePtr patientModelService, QWidget* parent);
  virtual ~TransferFunction2DWidget() {}
  virtual QString defaultWhatsThis() const;

public slots:
  void activeImageChangedSlot();

protected:
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  DoubleDataAdapterImageTFDataBasePtr mDataWindow, mDataAlpha, mDataLLR, mDataLevel;

  ActiveImageProxyPtr mActiveImageProxy;
  PatientModelServicePtr mPatientModelService;
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
  TransferFunctionWidget(PatientModelServicePtr patientModelService, QWidget* parent);
  virtual ~TransferFunctionWidget() {}
  virtual QString defaultWhatsThis() const;
};

/**
 * @}
 */
}

#endif /* CXTRANSFERFUNCTIONWIDGET_H_ */
