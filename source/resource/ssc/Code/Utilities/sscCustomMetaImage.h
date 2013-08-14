/*
 * sscCustomMetaImage.h
 *
 *  Created on: Mar 1, 2011
 *      Author: christiana
 */

#ifndef SSCCUSTOMMETAIMAGE_H_
#define SSCCUSTOMMETAIMAGE_H_

#include <QString>
#include "sscTransform3D.h"

namespace ssc
{

typedef boost::shared_ptr<class CustomMetaImage> CustomMetaImagePtr;

/**\brief utility class for accessing metaheader files.
 *
 * This is meant as a supplement to vtkMetaImageReader/Writer,
 * extending that interface.
 *
 * \ingroup sscUtility
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

private:
  QString mFilename;

  void remove(QStringList* data, QStringList keys);
  void append(QStringList* data, QString key, QString value);

};

}

#endif /* SSCCUSTOMMETAIMAGE_H_ */
