/*
 * sscCustomMetaImage.h
 *
 *  Created on: Mar 1, 2011
 *      Author: christiana
 */

#ifndef CXCUSTOMMETAIMAGE_H_
#define CXCUSTOMMETAIMAGE_H_

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
class CustomMetaImage
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
