/*
 * cxSelectRecordSessionStringDataAdapter.h
 *
 *  \date Jun 14, 2011
 *      \author christiana
 */

#ifndef CXSELECTRECORDSESSIONSTRINGDATAADAPTER_H_
#define CXSELECTRECORDSESSIONSTRINGDATAADAPTER_H_

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
class SelectRecordSessionStringDataAdapterBase : public StringDataAdapter
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
class SelectRecordSessionStringDataAdapter : public SelectRecordSessionStringDataAdapterBase
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

#endif /* CXSELECTRECORDSESSIONSTRINGDATAADAPTER_H_ */
