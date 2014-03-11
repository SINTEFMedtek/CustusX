#ifndef CXSBINARYTHRESHOLDIMAGEFILTER_H_
#define CXSBINARYTHRESHOLDIMAGEFILTER_H_

#include "cxFilterImpl.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_plugin_algorithm
 * @{
 */

/** Filter wrapping a itk::BinaryThresholdImageFilter.
 *
 * \ingroup cx_plugin_algorithm
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
	DoubleDataAdapterXmlPtr getLowerThresholdOption(QDomElement root);
	ColorDataAdapterXmlPtr getColorOption(QDomElement root);
	BoolDataAdapterXmlPtr getGenerateSurfaceOption(QDomElement root);

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
	DoubleDataAdapterXmlPtr mLowerThresholdOption;
	vtkImageDataPtr mRawResult;
	vtkPolyDataPtr mRawContour;
};
typedef boost::shared_ptr<class BinaryThresholdImageFilter> BinaryThresholdImageFilterPtr;


/**
 * @}
 */
}//namespace cx

#endif /* CXSBINARYTHRESHOLDIMAGEFILTER_H_ */
