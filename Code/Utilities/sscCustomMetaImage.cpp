/*
 * sscCustomMetaImage.cpp
 *
 *  Created on: Mar 1, 2011
 *      Author: christiana
 */

#include <sscCustomMetaImage.h>

#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "sscMessageManager.h"
#include "sscTypeConversions.h"

namespace ssc
{

CustomMetaImage::CustomMetaImage(QString filename) :
    mFilename(filename)
{

}

QString CustomMetaImage::readKey(QString key)
{
	QFile file(mFilename);

	QString line;
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream t(&file);
		while (!t.atEnd())
		{
			line.clear();
			line = t.readLine();
			// do something with the line
			if (line.startsWith(key, Qt::CaseInsensitive))
			{
				QStringList list = line.split("=", QString::SkipEmptyParts);
				if (list.size() >= 2)
				{
					list = list.mid(1);
					return list.join("=");
				}
			}
		}
		file.close();
	}

	return "";
}

QString CustomMetaImage::readModality()
{
	QString mod = this->readKey("Modality");

	if (mod=="MET_MOD_CT")
		return "CT;";
	if (mod=="MET_MOD_MR")
		return "MR;";
	if (mod=="MET_MOD_US")
		return "US;";
	if (mod=="MET_MOD_OTHER")
		return "OTHER;";
	return "UNKNOWN";
}

QString CustomMetaImage::readImageType()
{
	return this->readKey("ImageType3");
}

Transform3D CustomMetaImage::readTransform()
{
  //messageManager()->sendDebug("load filename: "+string_cast(filename));
  //read the specific TransformMatrix-tag from the header
  Vector3D p_r(0, 0, 0);
  Vector3D e_x(1, 0, 0);
  Vector3D e_y(0, 1, 0);
  Vector3D e_z(0, 0, 1);

  QFile file(mFilename);

  QString line;
  if (file.open(QIODevice::ReadOnly))
  {
    QTextStream t(&file);
    while (!t.atEnd())
    {
      line.clear();
      line = t.readLine();
      // do something with the line
      if (line.startsWith("Position", Qt::CaseInsensitive) || line.startsWith("Offset", Qt::CaseInsensitive))
      {
        QStringList list = line.split(" ", QString::SkipEmptyParts);
        if (list.size()>=5)
          p_r = Vector3D(list[2].toDouble(), list[3].toDouble(), list[4].toDouble());
      }
      else if (line.startsWith("TransformMatrix", Qt::CaseInsensitive) || line.startsWith("Orientation",
          Qt::CaseInsensitive))
      {
        QStringList list = line.split(" ", QString::SkipEmptyParts);

        if (list.size()>=8)
        {
          e_x = Vector3D(list[2].toDouble(), list[3].toDouble(), list[4].toDouble());
          e_y = Vector3D(list[5].toDouble(), list[6].toDouble(), list[7].toDouble());
          e_z = cross(e_x, e_y);
        }
      }
    }
    file.close();
  }

  Transform3D rMd = Transform3D::Identity();

  // add rotational part
  for (unsigned i = 0; i < 3; ++i)
  {
    rMd(i,0) = e_x[i];
    rMd(i,1) = e_y[i];
    rMd(i,2) = e_z[i];
  }


  // add translational part
  rMd(0,3) = p_r[0];
  rMd(1,3) = p_r[1];
  rMd(2,3) = p_r[2];
  return rMd;
}

void CustomMetaImage::setTransform(const Transform3D M)
{
  QFile file(mFilename);

  if (!file.open(QIODevice::ReadWrite))
  {
    messageManager()->sendError("Failed to open file " + mFilename + ".");
    return;
  }

  QStringList data = QTextStream(&file).readAll().split("\n");
//  std::cout << "before :" << data.join("\n") << std::endl;
  QRegExp regexp("(^TransformMatrix|^Offset|^Position|^Orientation)");
//  QRegExp regexp("?!(\"^Orientation\"|\"^TransformMatrix)\")");
  QStringList removeThese = data.filter(regexp);
  for (int i=0; i<removeThese.size(); ++i)
    data.removeAll(removeThese[i]);

  int dim = 3; // hardcoded - will fail for 2d images
  std::stringstream tmList;
  for (int c=0; c<dim; ++c)
    for (int r=0; r<dim; ++r)
      tmList << " " << M(r,c);
//  tmList <<" " << M[0][0] <<" "<< M[1][0] <<" "<<  M[2][0];
//  tmList <<" " << M[0][1] <<" "<< M[1][1] <<" "<<  M[2][1];
//  tmList <<" " << M[0][2] <<" "<< M[1][2] <<" "<<  M[2][2];
  data.push_back(QString("TransformMatrix =" + qstring_cast(tmList.str())));

  std::stringstream posList;
  for (int r=0; r<dim; ++r)
    posList << " " << M(r,3);
//  posList << M[0][3] <<" "<< M[1][3] <<" "<<  M[2][3];
  data.push_back(QString("Offset = " + qstring_cast(posList.str())));

//  rMd[0][3] = p_r[0];
//  rMd[1][3] = p_r[1];
//  rMd[2][3] = p_r[2];

//  std::cout << "after :" << data.join("\n") << std::endl;

  file.resize(0);
  file.write(data.join("\n").toAscii());
}

}
