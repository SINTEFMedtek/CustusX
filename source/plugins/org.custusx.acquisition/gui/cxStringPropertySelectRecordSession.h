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
#ifndef CXSTRINGPROPERTYSELECTRECORDSESSION_H_
#define CXSTRINGPROPERTYSELECTRECORDSESSION_H_

#include "org_custusx_acquisition_Export.h"

#include "cxDataInterface.h"
#include "cxRecordSession.h"
#include "cxAcquisitionData.h"

namespace cx
{
/**
* \file
* \addtogroup cx_module_acquisition
* @{
*/

/** Base class for all DataAdapters that selects a record session.
 */
class org_custusx_acquisition_EXPORT SelectRecordSessionStringDataAdapterBase : public StringDataAdapter
{
  Q_OBJECT
public:
  SelectRecordSessionStringDataAdapterBase(AcquisitionDataPtr pluginData);
  virtual ~SelectRecordSessionStringDataAdapterBase() {}

public: // basic methods

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

protected:
  AcquisitionDataPtr mPluginData;
};
typedef boost::shared_ptr<class SelectRecordSessionStringDataAdapterBase> SelectRecordSessionStringDataAdapterBasePtr;


typedef boost::shared_ptr<class SelectRecordSessionStringDataAdapter> SelectRecordSessionStringDataAdapterPtr;

/** Adapter that selects and stores a tool.
 * The tool is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 *
 */
class org_custusx_acquisition_EXPORT  SelectRecordSessionStringDataAdapter : public SelectRecordSessionStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectRecordSessionStringDataAdapterPtr New(AcquisitionDataPtr pluginData) { return SelectRecordSessionStringDataAdapterPtr(new SelectRecordSessionStringDataAdapter(pluginData)); }
  SelectRecordSessionStringDataAdapter(AcquisitionDataPtr pluginData);
  virtual ~SelectRecordSessionStringDataAdapter() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: //interface extencion
  RecordSessionPtr getRecordSession();

private slots:
  void setDefaultSlot();

private:
  RecordSessionPtr mRecordSession;

};

/**
* @}
*/
}

#endif /* CXSTRINGPROPERTYSELECTRECORDSESSION_H_ */
