#ifndef CXTUBESEGMENTATIONFILTER_H_
#define CXTUBESEGMENTATIONFILTER_H_

#include "cxFilterImpl.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;

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

private:
	vtkImageDataPtr convertToVtkImageData(char * data, int size_x, int size_y, int size_z);

	vtkImageDataPtr mRawCenterlineResult;
	vtkImageDataPtr mRawSegmentationResult;

};
typedef boost::shared_ptr<class TubeSegmentationFilter> TubeSegmentationFilterPtr;

#endif //CX_USE_TSF

} /* namespace cx */
#endif /* CXTUBESEGMENTATIONFILTER_H_ */
