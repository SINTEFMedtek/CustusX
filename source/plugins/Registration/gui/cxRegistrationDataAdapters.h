/*
 * cxRegistrationDataAdapters.h
 *
 *  \date Jun 16, 2011
 *      \author christiana
 */

#ifndef CXREGISTRATIONDATAADAPTERS_H_
#define CXREGISTRATIONDATAADAPTERS_H_

#include "cxDataInterface.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;

typedef boost::shared_ptr<class RegistrationFixedImageStringDataAdapter> RegistrationFixedImageStringDataAdapterPtr;
/** Adapter that connects to the fixed image in the registration manager.
 */
class RegistrationFixedImageStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
//  static RegistrationFixedImageStringDataAdapterPtr New() { return RegistrationFixedImageStringDataAdapterPtr(new RegistrationFixedImageStringDataAdapter()); }
  RegistrationFixedImageStringDataAdapter(RegistrationManagerPtr regManager);
  virtual ~RegistrationFixedImageStringDataAdapter() {}

public: // basic methods
//  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
//  virtual QString getHelp() const;
//  virtual ssc::DataPtr getData() const;

private:
  RegistrationManagerPtr mManager;
};


typedef boost::shared_ptr<class RegistrationMovingImageStringDataAdapter> RegistrationMovingImageStringDataAdapterPtr;
/** Adapter that connects to the fixed image in the registration manager.
 */
class RegistrationMovingImageStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
//  static RegistrationMovingImageStringDataAdapterPtr New() { return RegistrationMovingImageStringDataAdapterPtr(new RegistrationMovingImageStringDataAdapter()); }
  RegistrationMovingImageStringDataAdapter(RegistrationManagerPtr regManager);
  virtual ~RegistrationMovingImageStringDataAdapter() {}

public: // basic methods
//  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
//  virtual QString getHelp() const;
//  virtual ssc::DataPtr getData() const;

private:
  RegistrationManagerPtr mManager;
};

/**
 * @}
 */
}

#endif /* CXREGISTRATIONDATAADAPTERS_H_ */
