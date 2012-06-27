#include "cxResample.h"

#include "sscImageAlgorithms.h"
#include "sscDataManager.h"

namespace cx
{
Resample::Resample() :
    ThreadedTimedAlgorithm<ssc::ImagePtr>("resampling", 5)
{}

Resample::~Resample()
{}

void Resample::setInput(ssc::ImagePtr image, ssc::ImagePtr reference, QString outputBasePath, double margin)
{
  mMargin = margin;
  mInput = image;
  mInputReference = reference;
  mOutputBasePath = outputBasePath;

//  this->generate();
}

ssc::ImagePtr Resample::getOutput()
{
  return mOutput;
}

void Resample::postProcessingSlot()
{
  mOutput = this->getResult();
  if(!mOutput)
  {
    ssc::messageManager()->sendError("Resampling failed.");
    return;
  }

  ssc::dataManager()->loadData(mOutput);
  ssc::dataManager()->saveImage(mOutput, mOutputBasePath);

  ssc::messageManager()->sendSuccess("Done resampling: \"" + mOutput->getName()+"\"");

//  emit finished();
}

/** Crop the image to the bounding box bb_q.
 *  bb_q is given in the output space q, defined relative to the image space d
 *  with qMd. If qMd is non-identity, image is resampled to space q.
 *  outputSpacing can be used to resample the volume (after cropping).
 */
ssc::ImagePtr Resample::calculate()
{
  if (!mInput || !mInputReference)
    return ssc::ImagePtr();

  ssc::Transform3D refMi = mInputReference->get_rMd().inv() * mInput->get_rMd();
  ssc::ImagePtr oriented = resampleImage(mInput, refMi);//There is an error with the transfer functions in this image

  ssc::Transform3D orient_M_ref = oriented->get_rMd().inv() * mInputReference->get_rMd();
  ssc::DoubleBoundingBox3D bb_crop = transform(orient_M_ref, mInputReference->boundingBox());

  // increase bb size by margin
  bb_crop[0] -= mMargin;
  bb_crop[1] += mMargin;
  bb_crop[2] -= mMargin;
  bb_crop[3] += mMargin;
  bb_crop[4] -= mMargin;
  bb_crop[5] += mMargin;

  oriented->setCroppingBox(bb_crop);

  ssc::ImagePtr cropped = cropImage(oriented);

  QString uid = mInput->getUid() + "_resample%1";
  QString name = mInput->getName() + " resample%1";

  ssc::ImagePtr resampled = resampleImage(cropped, ssc::Vector3D(mInputReference->getBaseVtkImageData()->GetSpacing()), uid, name);

  // important! move thread affinity to main thread - ensures signals/slots is still called correctly
  resampled->moveThisAndChildrenToThread(QApplication::instance()->thread());

  return resampled;
}

}//namespace cx
