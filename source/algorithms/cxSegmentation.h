#ifndef CXSEGMENTATION_H_
#define CXSEGMENTATION_H_

#include <QtGui>
#include <QDateTime>
#include "sscMessageManager.h"

#include <QObject>
#include <QString>
#include "sscForwardDeclarations.h"

#include "sscTransform3D.h"

namespace cx
{
/**
 * \class Segmentation
 *
 * \brief
 *
 * \date Oct 12, 2010
 * \author Christian Askeland, SINTEF
 */

class Segmentation
{
public:
  ssc::MeshPtr contour(ssc::ImagePtr image, QString outputBasePath, int threshold, double decimation=0.8, bool reduceResolution=false, bool smoothing=true);

  ssc::ImagePtr segment(ssc::ImagePtr image, QString outputBasePath, int threshold, bool useSmothing=true, double smoothSigma=0.5);

  //ssc::ImagePtr centerline(ssc::ImagePtr image, QString outputBasePath); ///< finds the images centerline, saves the new centerline volume and adds it internally to the datamanager

  //ssc::ImagePtr resample(ssc::ImagePtr image, ssc::ImagePtr reference, QString outputBasePath, double margin);
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace cx
#endif /* CXSEGMENTATION_H_ */
