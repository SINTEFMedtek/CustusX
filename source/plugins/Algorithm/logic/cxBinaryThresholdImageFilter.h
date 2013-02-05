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
	virtual bool postProcess();

	// extensions:
	ssc::DoubleDataAdapterXmlPtr getLowerThresholdOption(QDomElement root);
	ssc::ColorDataAdapterXmlPtr getColorOption(QDomElement root);
	ssc::BoolDataAdapterXmlPtr getGenerateSurfaceOption(QDomElement root);

protected:
	virtual void createOptions();
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


/**
 * @}
 */
}//namespace cx

#endif /* CXSBINARYTHRESHOLDIMAGEFILTER_H_ */
