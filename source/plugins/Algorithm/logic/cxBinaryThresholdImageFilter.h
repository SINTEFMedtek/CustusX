#ifndef CXSBINARYTHRESHOLDIMAGEFILTER_H_
#define CXSBINARYTHRESHOLDIMAGEFILTER_H_

#include "cxThreadedTimedAlgorithm.h"
#include "cxFilterImpl.h"
#include "sscStringDataAdapter.h"
#include "sscDoubleDataAdapter.h"
#include "sscColorDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"
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
	ssc::ColorDataAdapterXmlPtr getColorOption(QDomElement root);
	ssc::BoolDataAdapterXmlPtr getGenerateSurfaceOption(QDomElement root);

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
	vtkPolyDataPtr mRawContour;
};
typedef boost::shared_ptr<class BinaryThresholdImageFilter> BinaryThresholdImageFilterPtr;


///** Filter decorating another filter by adding a contouring step.
// *
// * The input filter is passed unchanged, but in addition the output
// * is contoured and added as a secondary output.
// *
// *
// * \ingroup cxPluginAlgorithm
// * \date Nov 21, 2012
// * \author christiana
// */
//class ContouringDecoratingFilter : public FilterImpl
//{
//	Q_OBJECT

//public:
//	explicit ContouringDecoratingFilter(FilterPtr base);
//	virtual ~ContouringDecoratingFilter() {}

//	virtual QString getType() const;
//	virtual QString getName() const;
//	virtual QString getHelp() const;

//	virtual std::vector<DataAdapterPtr> getOptions(QDomElement root);
//	virtual std::vector<SelectDataStringDataAdapterBasePtr> getInputTypes();
//	virtual std::vector<SelectDataStringDataAdapterBasePtr> getOutputTypes();
//	virtual void setActive(bool on);
//	virtual bool preProcess();
//	virtual bool execute();
//	virtual void postProcess();

//	// extensions:
//	ssc::BoolDataAdapterXmlPtr getGenerateSurfaceOption(QDomElement root);

//protected:
//	virtual void createOptions(QDomElement root);
//	virtual void createInputTypes();
//	virtual void createOutputTypes();

//private slots:

//private:
////	std::vector<SelectDataStringDataAdapterBasePtr> mInputTypes;
////	std::vector<SelectDataStringDataAdapterBasePtr> mOutputTypes;
////	std::vector<DataAdapterPtr> mOptionsAdapters;
////	QDomElement mOptions;

////	// data used by execute - copied for thread safety purposes
////	std::vector<ssc::DataPtr> mCopiedInput;
////	QDomElement mCopiedOptions;
////	bool mActive;
////	QString mUid;


////	ssc::BoolDataAdapterXmlPtr mGenerateSurfaceOption;
//	FilterPtr mBase;
//};
//typedef boost::shared_ptr<class ContouringDecoratingFilter> ContouringDecoratingFilterPtr;


/**
 * @}
 */
}//namespace cx

#endif /* CXSBINARYTHRESHOLDIMAGEFILTER_H_ */
