#ifndef CXTUBESEGMENTATIONFILTER_H_
#define CXTUBESEGMENTATIONFILTER_H_

#include "cxFilterImpl.h"

namespace cx {

#ifdef CX_USE_TSF
/**
 * Filter for segmenting and extract the centerline of a volume.
 *
 * This filter can run either on the gpu or cpu.
 *
 * Algorithm written by Erik Smistad. For more information, see paper:
 * "GPU-Based Airway Segmentation and Centerline Extraction for Image Guided Bronchoscopy."
 *
 * \ingroup cxPluginAlgorithm
 * \date 14.12.2012
 * \author Janne Beate Bakeng, SINTEF
 */
class TubeSegmentationFilter : public FilterImpl
{
	Q_OBJECT

public:
	virtual ~TubeSegmentationFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
	virtual void postProcess();

protected:
	virtual void createOptions(QDomElement root);
	virtual void createInputTypes();
	virtual void createOutputTypes();
//
//private:
//	ssc::StringDataAdapterPtr getStringOption(QDomElement root);
//	ssc::DoubleDataAdapterPtr getDoubleOption(QDomElement root);
//	ssc::BoolDataAdapterPtr getBoolOption(QDomElement root);

};
#endif //CX_USE_TSF

} /* namespace cx */
#endif /* CXTUBESEGMENTATIONFILTER_H_ */
