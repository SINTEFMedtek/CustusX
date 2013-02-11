#ifndef CXTUBESEGMENTATIONFILTER_H_
#define CXTUBESEGMENTATIONFILTER_H_

#include "cxFilterImpl.h"

#include <boost/unordered_map.hpp>

#ifdef CX_USE_TSF
#include "parameters.hpp"
class TSFOutput;
#endif //CX_USE_TSF

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
	TubeSegmentationFilter();
	virtual ~TubeSegmentationFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
	virtual bool postProcess();

protected:
	//TODO create a reset option (to default values)!!!
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:
	void patientChangedSlot();
	void inputChangedSlot();
	void parametersFileChanged();
	void resetOptions();
//	void centerlineMethodChanged();

private:
	vtkImageDataPtr convertToVtkImageData(char * data, int size_x, int size_y, int size_z, ssc::ImagePtr input);
	void createDefaultOptions(QDomElement root); ///< generate options based on file with all valid parameters for smistads algorithm
	paramList getParametersFromOptions(); ///< fetches the parameters set by the user
	void setParamtersToOptions(paramList parameters); ///< set the options to a given set of parameters
	void setOptionValue(QString valueName, QString value); ///< set one option to a specific value

	void printParameters(paramList params); //helper function

	ssc::StringDataAdapterXmlPtr makeStringOption(QDomElement root, std::string name, StringParameter parameter);
	ssc::BoolDataAdapterXmlPtr makeBoolOption(QDomElement root, std::string name, BoolParameter parameter);
	ssc::DoubleDataAdapterXmlPtr makeDoubleOption(QDomElement root, std::string name, NumericParameter parameter);

	ssc::StringDataAdapterXmlPtr mResetOption; ///< temporary fix for resetting options to default
//	ssc::StringDataAdapterXmlPtr mSaveParametersOption; ///< temporary fix for saving parameters to file

	std::vector<ssc::StringDataAdapterXmlPtr> mStringOptions; ///< string options to be displayed to the user
	std::vector<ssc::BoolDataAdapterXmlPtr> mBoolOptions; ///< bool options to be displayed to the user
	std::vector<ssc::DoubleDataAdapterXmlPtr> mDoubleOptions; ///< double options to be displayed to the user

	TSFOutput* mOutput; ///< output from last execution
	paramList mParameters; ///< the parameters used in last execution

};
typedef boost::shared_ptr<class TubeSegmentationFilter> TubeSegmentationFilterPtr;

#endif //CX_USE_TSF

} /* namespace cx */
#endif /* CXTUBESEGMENTATIONFILTER_H_ */
