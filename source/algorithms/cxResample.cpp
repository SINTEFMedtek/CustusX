#include "cxResample.h"

#include "sscImageAlgorithms.h"
#include "sscDataManager.h"

namespace cx
{
Resample::Resample() :
    TimedAlgorithm("resampling", 5)
{
  connect(&mWatcher, SIGNAL(finished()), this, SLOT(finishedSlot()));
}

Resample::~Resample()
{}

void Resample::setInput(ssc::ImagePtr image, ssc::ImagePtr reference, QString outputBasePath, double margin)
{
  mMargin = margin;
  mInput = image;
  mInputReference = reference;
  mOutputBasePath = outputBasePath;

  this->generate();
}

ssc::ImagePtr Resample::getOutput()
{
  return mOutput;
}

void Resample::finishedSlot()
{
  mOutput = mWatcher.future().result();
  if(!mOutput)
  {
    ssc::messageManager()->sendError("Resampling failed.");
    return;
  }

  ssc::dataManager()->loadData(mOutput);
  ssc::dataManager()->saveImage(mOutput, mOutputBasePath);

  this->stopTiming();
  ssc::messageManager()->sendSuccess("Done resampling: \"" + mOutput->getName()+"\"");

  emit finished();
}

void Resample::generate()
{
  this->startTiming();

  mFutureResult = QtConcurrent::run(this, &Resample::calculate);
  mWatcher.setFuture(mFutureResult);
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
  ssc::ImagePtr oriented = resampleImage(mInput, refMi);

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

  ssc::ImagePtr resampled = resampleImage(cropped, ssc::Vector3D(mInputReference->getBaseVtkImageData()->GetSpacing()));

  return resampled;
}

}//namespace cx
