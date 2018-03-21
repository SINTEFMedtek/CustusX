/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXCUSTOMMETAIMAGE_H_
#define CXCUSTOMMETAIMAGE_H_

#include "cxResourceExport.h"

#include <QString>
#include "cxTransform3D.h"

namespace cx
{

typedef boost::shared_ptr<class CustomMetaImage> CustomMetaImagePtr;

/**\brief utility class for accessing metaheader files.
 *
 * This is meant as a supplement to vtkMetaImageReader/Writer,
 * extending that interface.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT CustomMetaImage
{
public:
  static CustomMetaImagePtr create(QString filename) { return CustomMetaImagePtr(new CustomMetaImage(filename)); }
  explicit CustomMetaImage(QString filename);

  Transform3D readTransform();
  void setTransform(const Transform3D M);

  QString readModality();
  QString readImageType();
  void setModality(QString value);
  void setImageType(QString value);

  QString readKey(QString key);
  void setKey(QString key, QString value);

private:
  QString mFilename;

  void remove(QStringList* data, QStringList keys);
  void append(QStringList* data, QString key, QString value);

};

}

#endif /* CXCUSTOMMETAIMAGE_H_ */
