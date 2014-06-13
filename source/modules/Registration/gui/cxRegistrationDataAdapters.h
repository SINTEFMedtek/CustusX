/*
 * cxRegistrationDataAdapters.h
 *
 *  \date Jun 16, 2011
 *      \author christiana
 */

#ifndef CXREGISTRATIONDATAADAPTERS_H_
#define CXREGISTRATIONDATAADAPTERS_H_

#include "cxDataInterface.h"
#include "cxSelectDataStringDataAdapter.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_module_registration
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
  RegistrationFixedImageStringDataAdapter(RegistrationManagerPtr regManager);
  virtual ~RegistrationFixedImageStringDataAdapter() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

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
  RegistrationMovingImageStringDataAdapter(RegistrationManagerPtr regManager);
  virtual ~RegistrationMovingImageStringDataAdapter() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

private:
  RegistrationManagerPtr mManager;
};

/**
 * @}
 */
}

#endif /* CXREGISTRATIONDATAADAPTERS_H_ */
