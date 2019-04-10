/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxCustomMetaImage.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include "cxLogger.h"
#include "cxData.h"

#include "cxTypeConversions.h"
#include "cxEnumConversion.h"

namespace cx
{


IMAGE_MODALITY convertToModality(QString modalityString)
{
	IMAGE_MODALITY retval = imUNKNOWN;

	if(modalityString.contains(enum2string<IMAGE_MODALITY>(imUNKNOWN), Qt::CaseInsensitive))
		retval = imUNKNOWN;
	else if(modalityString.contains(enum2string<IMAGE_MODALITY>(imCT), Qt::CaseInsensitive))
		retval = imCT;
	else if(modalityString.contains(enum2string<IMAGE_MODALITY>(imMR), Qt::CaseInsensitive))
		retval = imMR;
	else if(modalityString.contains(enum2string<IMAGE_MODALITY>(imUS), Qt::CaseInsensitive))
		retval = imUS;
	else if(modalityString.contains(enum2string<IMAGE_MODALITY>(imPET), Qt::CaseInsensitive))
		retval = imPET;
	else if(modalityString.contains(enum2string<IMAGE_MODALITY>(imSC), Qt::CaseInsensitive))
		retval = imSC;
	else
		CX_LOG_WARNING() << "convertToModality - Cannot convert the string \"" << modalityString << "\" to a known image modality";

	return retval;
}


IMAGE_SUBTYPE convertToImageSubType(QString imageTypeSubString)
{
	IMAGE_SUBTYPE retval = istUNKNOWN;

	if (imageTypeSubString.isEmpty() || imageTypeSubString == " ")
		retval = istEMPTY;
	else if(imageTypeSubString.contains(enum2string<IMAGE_SUBTYPE>(istUNKNOWN), Qt::CaseInsensitive))
		retval = istUNKNOWN;
	else if(imageTypeSubString.contains(enum2string<IMAGE_SUBTYPE>(istMRT1), Qt::CaseInsensitive))
		retval = istMRT1;
	else if(imageTypeSubString.contains(enum2string<IMAGE_SUBTYPE>(istMRT2), Qt::CaseInsensitive))
		retval = istMRT2;
	else if(imageTypeSubString.contains(enum2string<IMAGE_SUBTYPE>(istMRFLAIR), Qt::CaseInsensitive))
		retval = istMRFLAIR;
	else if(imageTypeSubString.contains(enum2string<IMAGE_SUBTYPE>(istANGIO), Qt::CaseInsensitive))
		retval = istANGIO;
	else if(imageTypeSubString.contains(enum2string<IMAGE_SUBTYPE>(istUSBMODE), Qt::CaseInsensitive))
		retval = istUSBMODE;
	else if(imageTypeSubString.contains("bmode", Qt::CaseInsensitive))
		retval = istUSBMODE;
	else if(imageTypeSubString.contains("b_mode", Qt::CaseInsensitive))
		retval = istUSBMODE;
	else if(imageTypeSubString.contains("b mode", Qt::CaseInsensitive))
		retval = istUSBMODE;
	else if(imageTypeSubString.contains("seg", Qt::CaseInsensitive))
		retval = istSEGMENTATION;
	else if(imageTypeSubString.contains("label", Qt::CaseInsensitive))
		retval = istSEGMENTATION;
	else
		CX_LOG_WARNING() << "convertToImageSubType - Cannot convert the string \"" << imageTypeSubString << "\" to a known image subtype";

	return retval;
}


CustomMetaImage::CustomMetaImage(QString filename) :
    mFilename(filename)
{}

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

IMAGE_MODALITY CustomMetaImage::readModality()
{
	QString modalityString = this->readKey("Modality");
	return convertToModality(modalityString);
}

IMAGE_SUBTYPE CustomMetaImage::readImageType()
{
	QString imageTypeString = this->readKey("ImageType3");
	return convertToImageSubType(imageTypeString);
}

/** Remove all lines starting with a key from data.
  *
  */
void CustomMetaImage::remove(QStringList* data, QStringList keys)
{
	QRegExp regexp(QString("(^%1)").arg(keys.join("|^")));
	QStringList removeThese = data->filter(regexp);
	for (int i=0; i<removeThese.size(); ++i)
		data->removeAll(removeThese[i]);
}

/** Append key, value pair to data.
  * The line is added last, but BEFORE the "ElementDataFile" key,
  * which is required to be last.
  *
  */
void CustomMetaImage::append(QStringList* data, QString key, QString value)
{
	// find index of ElementDataFile - this is the last element according to MHD standard (but we might have appended something else after it).
    int last = data->lastIndexOf(QRegExp("^ElementDataFile.*"));
	data->insert(last, QString("%1 = %2").arg(key).arg(value));
}

void CustomMetaImage::setKey(QString key, QString value)
{
	QFile file(mFilename);

	if (!file.open(QIODevice::ReadWrite))
	{
	  reportError("Failed to open file " + mFilename + ".");
	  return;
	}

	QStringList data = QTextStream(&file).readAll().split("\n");

	this->remove(&data, QStringList()<<key);
	this->append(&data, key, value);

	file.resize(0);
	file.write(data.join("\n").toLatin1());
}

void CustomMetaImage::setModality(IMAGE_MODALITY value)
{
	this->setKey("Modality", enum2string(value));
}

void CustomMetaImage::setImageType(IMAGE_SUBTYPE value)
{
	this->setKey("ImageType3", enum2string(value));
}


Transform3D CustomMetaImage::readTransform()
{
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
    reportWarning("Could not save transform because: Failed to open file " + mFilename);
    return;
  }

  QStringList data = QTextStream(&file).readAll().split("\n");

  this->remove(&data, QStringList()<<"TransformMatrix"<<"Offset"<<"Position"<<"Orientation");

  int dim = 3; // hardcoded - will fail for 2d images
  std::stringstream tmList;
  for (int c=0; c<dim; ++c)
    for (int r=0; r<dim; ++r)
      tmList << " " << M(r,c);
  this->append(&data, "TransformMatrix", qstring_cast(tmList.str()));

  std::stringstream posList;
  for (int r=0; r<dim; ++r)
    posList << " " << M(r,3);
  this->append(&data, "Offset", qstring_cast(posList.str()));

  file.resize(0);
  file.write(data.join("\n").toLatin1());
}

}
