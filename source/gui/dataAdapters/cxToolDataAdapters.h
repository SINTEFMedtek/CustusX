/*
 * cxToolDataAdapters.h
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#ifndef CXTOOLDATAADAPTERS_H_
#define CXTOOLDATAADAPTERS_H_

#include "sscStringDataAdapter.h"

namespace cx
{
typedef boost::shared_ptr<class Tool> ToolPtr;

/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

/** Adapter that connects to the current active tool.
 */
class ActiveToolStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ssc::StringDataAdapterPtr New() { return ssc::StringDataAdapterPtr(new ActiveToolStringDataAdapter()); }
  ActiveToolStringDataAdapter();
  virtual ~ActiveToolStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};


/** Adapter that connects to the current active tool.
 */
class ActiveToolConfigurationStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ssc::StringDataAdapterPtr New() { return ssc::StringDataAdapterPtr(new ActiveToolConfigurationStringDataAdapter()); }
  ActiveToolConfigurationStringDataAdapter();
  virtual ~ActiveToolConfigurationStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

private slots:
  void dominantToolChanged();
private:
  ToolPtr mTool;
};


/**
 * @}
 */
}

#endif /* CXTOOLDATAADAPTERS_H_ */
