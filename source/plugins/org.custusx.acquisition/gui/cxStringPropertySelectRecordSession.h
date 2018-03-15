/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSTRINGPROPERTYSELECTRECORDSESSION_H_
#define CXSTRINGPROPERTYSELECTRECORDSESSION_H_

#include "org_custusx_acquisition_Export.h"

//#include "cxDataInterface.h"
#include "cxRecordSession.h"
#include "cxAcquisitionData.h"
#include "cxStringPropertyBase.h"

namespace cx
{
/**
* \file
* \addtogroup org_custusx_acquisition
* @{
*/

/** Base class for all Properties that selects a record session.
 */
class org_custusx_acquisition_EXPORT StringPropertySelectRecordSessionBase : public StringPropertyBase
{
  Q_OBJECT
public:
  StringPropertySelectRecordSessionBase(AcquisitionDataPtr pluginData);
  virtual ~StringPropertySelectRecordSessionBase() {}

public: // basic methods

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

protected:
  AcquisitionDataPtr mPluginData;
};
typedef boost::shared_ptr<class StringPropertySelectRecordSessionBase> StringPropertySelectRecordSessionBasePtr;


typedef boost::shared_ptr<class StringPropertySelectRecordSession> StringPropertySelectRecordSessionPtr;

/** Adapter that selects and stores a tool.
 * The tool is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 *
 */
class org_custusx_acquisition_EXPORT  StringPropertySelectRecordSession : public StringPropertySelectRecordSessionBase
{
  Q_OBJECT
public:
  static StringPropertySelectRecordSessionPtr New(AcquisitionDataPtr pluginData) { return StringPropertySelectRecordSessionPtr(new StringPropertySelectRecordSession(pluginData)); }
  StringPropertySelectRecordSession(AcquisitionDataPtr pluginData);
  virtual ~StringPropertySelectRecordSession() {}

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
