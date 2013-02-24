// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

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


typedef boost::shared_ptr<class ActiveProbeConfigurationStringDataAdapter> ActiveProbeConfigurationStringDataAdapterPtr;

/** Adapter that connects to the current active probe.
 *  It will stick to the probe as much as possible,
 *  i.e. ignore hiding and showing of other non-probes.
 */
class ActiveProbeConfigurationStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ActiveProbeConfigurationStringDataAdapterPtr New() { return ActiveProbeConfigurationStringDataAdapterPtr(new ActiveProbeConfigurationStringDataAdapter()); }
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

  // extensions
  ssc::ToolPtr getTool() { return mTool; }

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
