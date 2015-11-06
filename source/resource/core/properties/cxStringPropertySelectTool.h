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
