#ifndef CXSBINARYTHRESHOLDIMAGEFILTER_H_
#define CXSBINARYTHRESHOLDIMAGEFILTER_H_

#include "cxThreadedTimedAlgorithm.h"
#include "cxFilterImpl.h"
#include "sscStringDataAdapter.h"
#include "sscDoubleDataAdapter.h"
#include "sscBoolDataAdapter.h"
#include "sscDoubleDataAdapterXml.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginAlgorithm
 * @{
 */

/**
 * \class BinaryThresholdImageFilter
 *
 * \brief Threaded BinaryThresholdImageFilter.
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class BinaryThresholdImageFilterOld : public ThreadedTimedAlgorithm<vtkImageDataPtr>
{
  Q_OBJECT

public:
  BinaryThresholdImageFilterOld();
  virtual ~BinaryThresholdImageFilterOld();

  void setInput(ssc::ImagePtr image, QString outputBasePath, int threshold, bool useSmoothing=true, double smoothSigma=0.1);
  ssc::ImagePtr getOutput();

protected slots:
  virtual void postProcessingSlot();

private:
  virtual vtkImageDataPtr calculate();

  QString       mOutputBasePath;
  ssc::ImagePtr mInput;
  ssc::ImagePtr mOutput;
  int           mTheshold;
  bool          mUseSmoothing;
  double        mSmoothingSigma;
};
typedef boost::shared_ptr<class BinaryThresholdImageFilterOld> BinaryThresholdImageFilterOldPtr;

/** Filter wrapping a itk::BinaryThresholdImageFilter.
 *
 * \ingroup cxPluginAlgorithm
 * \date Nov 21, 2012
 * \author christiana
 */
class BinaryThresholdImageFilter : public FilterImpl
{
    Q_OBJECT

public:
    virtual ~BinaryThresholdImageFilter() {}

    virtual QString getType() const;
    virtual QString getName() const;
    virtual QString getHelp() const;
    virtual void setActive(bool on);

    bool preProcess();
    virtual bool execute();
    virtual void postProcess();

    // extensions:
    ssc::DoubleDataAdapterXmlPtr getLowerThresholdOption(QDomElement root);

protected:
    virtual void createOptions(QDomElement root);
    virtual void createInputTypes();
    virtual void createOutputTypes();

private slots:
    /** Set new value+range of the threshold option.
      */
    void imageChangedSlot(QString uid);
    void thresholdSlot();

private:
    ssc::DoubleDataAdapterXmlPtr mLowerThresholdOption;
    vtkImageDataPtr mRawResult;
};
typedef boost::shared_ptr<class BinaryThresholdImageFilter> BinaryThresholdImageFilterPtr;

/**
 * @}
 */
}//namespace cx

#endif /* CXSBINARYTHRESHOLDIMAGEFILTER_H_ */
