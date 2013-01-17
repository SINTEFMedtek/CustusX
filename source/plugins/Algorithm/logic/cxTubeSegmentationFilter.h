#ifndef CXTUBESEGMENTATIONFILTER_H_
#define CXTUBESEGMENTATIONFILTER_H_

#include "cxFilterImpl.h"
#include "sscStringDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"

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

private slots:
	void toggleAutoMinimum();
	void toggleAutoMaximum();

private:
	vtkImageDataPtr convertToVtkImageData(char * data, int size_x, int size_y, int size_z);

	ssc::StringDataAdapterXmlPtr makeDeviceOption(QDomElement root);
	ssc::BoolDataAdapterXmlPtr makeBuffersOnlyOption(QDomElement root);
	ssc::BoolDataAdapterXmlPtr makeAutoMinimumOption(QDomElement root);
	ssc::DoubleDataAdapterXmlPtr makeMinimumOption(QDomElement root);
	ssc::BoolDataAdapterXmlPtr makeAutoMaximumOption(QDomElement root);
	ssc::DoubleDataAdapterXmlPtr makeMaximumOption(QDomElement root);
	ssc::StringDataAdapterXmlPtr makeModeOption(QDomElement root);
	ssc::BoolDataAdapterXmlPtr makeNoSegmentationOption(QDomElement root);
	ssc::StringDataAdapterXmlPtr makeCenterlineMethodOption(QDomElement root);

	vtkImageDataPtr mRawCenterlineResult;
	vtkImageDataPtr mRawSegmentationResult;

	ssc::BoolDataAdapterXmlPtr mAutoMinimumOption;
	ssc::BoolDataAdapterXmlPtr mAutoMaximumOption;
	ssc::DoubleDataAdapterXmlPtr mMinimumOption;
	ssc::DoubleDataAdapterXmlPtr mMaximumOption;

};
typedef boost::shared_ptr<class TubeSegmentationFilter> TubeSegmentationFilterPtr;

#endif //CX_USE_TSF

} /* namespace cx */
#endif /* CXTUBESEGMENTATIONFILTER_H_ */
