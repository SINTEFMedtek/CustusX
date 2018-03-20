/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSTRINGPROPERTYACTIVEVIDEOSOURCE_H
#define CXSTRINGPROPERTYACTIVEVIDEOSOURCE_H

#include "cxStringPropertyBase.h"
#include "cxResourceExport.h"

namespace cx
{

typedef boost::shared_ptr<class VideoService> VideoServicePtr;
typedef boost::shared_ptr<class StringPropertyActiveVideoSource> StringPropertyActiveVideoSourcePtr;

/**
 * \brief Property for controlling the active video source in cx::VideoService
 */
class cxResource_EXPORT StringPropertyActiveVideoSource : public StringPropertyBase
{
  Q_OBJECT
public:
	static StringPropertyActiveVideoSourcePtr create(VideoServicePtr service) { return StringPropertyActiveVideoSourcePtr(new StringPropertyActiveVideoSource(service)); }
  StringPropertyActiveVideoSource(VideoServicePtr service);
  virtual ~StringPropertyActiveVideoSource() {}

public:
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;
  virtual QStringList getValueRange() const;
  virtual QString getHelp() const;
private:
  VideoServicePtr mService;
};

} // namespace cx

#endif // CXSTRINGPROPERTYACTIVEVIDEOSOURCE_H
