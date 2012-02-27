/*
 * cxToolDataAdapters.h
 *
 *  \date May 4, 2011
 *      \author christiana
 */

#ifndef CXTOOLDATAADAPTERS_H_
#define CXTOOLDATAADAPTERS_H_

#include "sscStringDataAdapter.h"
#include "sscTool.h"

namespace cx
{

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


/** Adapter that connects to the current active probe.
 *  It will stick to the probe as much as possible,
 *  i.e. ignore hiding and showing of other non-probes.
 */
class ActiveProbeConfigurationStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ssc::StringDataAdapterPtr New() { return ssc::StringDataAdapterPtr(new ActiveProbeConfigurationStringDataAdapter()); }
  ActiveProbeConfigurationStringDataAdapter();
  virtual ~ActiveProbeConfigurationStringDataAdapter() {}

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
  ssc::ToolPtr mTool;
};


/**
 * @}
 */
}

#endif /* CXTOOLDATAADAPTERS_H_ */
