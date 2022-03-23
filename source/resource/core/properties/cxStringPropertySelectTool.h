/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSTRINGPROPERTYSELECTTOOL_H
#define CXSTRINGPROPERTYSELECTTOOL_H

#include "cxResourceExport.h"

#include "cxStringPropertyBase.h"
#include "cxForwardDeclarations.h"

/**
 * \file
 * \addtogroup cx_resource_core_properties
 * @{
 */

namespace cx
{

typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;

/**
 * \brief Adapter that selects and stores a tool.
 * The tool is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 *
 */
class cxResource_EXPORT StringPropertySelectTool : public StringPropertyBase
{
  Q_OBJECT
public:
  static StringPropertySelectToolPtr New(TrackingServicePtr trackingService)
  {
	  return StringPropertySelectToolPtr(new StringPropertySelectTool(trackingService));
  }
  StringPropertySelectTool(TrackingServicePtr trackingService);
  virtual ~StringPropertySelectTool() {}

  void setHelp(QString help);
  void setValueName(QString name);

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  ToolPtr getTool() const;
  bool isActiveToolSelected() const;

  void provideActiveTool(bool on);
  void setActiveTool();
private:
  TrackingServicePtr mTrackingService;
  QString mValueName;
  QString mHelp;
  ToolPtr mTool;
  bool mProvideActiveTool;
  bool mActiveToolSelected;
  const QString mActiveToolName;
};

} // namespace cx

/**
 * @}
 */

#endif // CXSTRINGPROPERTYSELECTTOOL_H
