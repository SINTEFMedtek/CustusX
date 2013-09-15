#ifndef CXTUBESEGMENTATIONFILTER_H_
#define CXTUBESEGMENTATIONFILTER_H_

#include "cxFilterImpl.h"

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

#ifdef CX_USE_TSF
#include "parameters.hpp"
class TSFOutput;
#endif //CX_USE_TSF

#include "sscStringDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;

namespace cx {
typedef boost::shared_ptr<class TSFPresets> TSFPresetsPtr;

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

	virtual bool hasPresets();
	virtual PresetsPtr getPresets();
	virtual QDomElement generatePresetFromCurrentlySetOptions(QString name); ///< get a xml element containing the currently set parameters
	virtual void requestSetPresetSlot(QString name); ///< try to set a specific preset

	virtual bool execute();
	virtual bool postProcess();

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:
	void patientChangedSlot(); ///< updates where tsf will save temp data
	void inputChangedSlot(); ///< updates where tsf will save the .vtk file
	void loadNewParametersSlot(); ///< updates all options
	void resetOptionsAdvancedSlot(); ///< resets all options to their default advanced settings
	void resetOptionsSlot(); ///< resets all options to their default settings
	void setOptionsSlot(paramList& list); ///< sets options to a given list of parameters

private:
	vtkImageDataPtr convertToVtkImageData(char * data, int size_x, int size_y, int size_z, ImagePtr input); ///< converts a char array to a vtkImageDataPtr
	vtkImageDataPtr convertToVtkImageData(float * data, int size_x, int size_y, int size_z, ImagePtr input); ///< converts a float array to a vtkImageDataPtr
	vtkImageDataPtr importRawImageData(void * data, int size_x, int size_y, int size_z, ImagePtr input, int type); ///< converts a any array to a vtkImageDataPtr
	MeshPtr loadVtkFile(QString pathToFile, QString newDatasUid); ///< loads a vtk file into CustusX

	void createDefaultOptions(QDomElement root); ///< generate options based on file with all valid parameters for smistads algorithm
	paramList getParametersFromOptions(); ///< fetches the parameters set by the user
	void setParamtersToOptions(paramList& parameters); ///< set the options to a given set of parameters

	StringDataAdapterXmlPtr getStringOption(QString valueName); ///< get string dataadapter with give name
	BoolDataAdapterXmlPtr getBoolOption(QString valueName); ///< get bool dataadapter with give name
	DoubleDataAdapterXmlPtr getDoubleOption(QString valueName); ///< get double dataadapter with give name
	DataAdapterPtr getOption(QString valueName); ///< get option/dataadapter with given name
	void setOptionAdvanced(QString valueName, bool advanced); ///< set one option to be advanced or not
	void setOptionValue(QString valueName, QString value); ///< set one option to a specific value

	std::vector<std::string> getNotDefault(paramList list); ///< gets the parameters that are not in the default list
	std::vector<std::string> getDifference(paramList list1, paramList list2); ///< returns the parameter names of the parameters that has different values
	std::vector<DataAdapterPtr> getNotDefaultOptions(); ///< gets the options that are not set to default values
	paramList getDefaultParameters(); ///< returns a list with the default parameters
	void printParameters(paramList params); ///< helper function

	StringDataAdapterXmlPtr makeStringOption(QDomElement root, std::string name, StringParameter parameter); ///< constructs a string option
	BoolDataAdapterXmlPtr makeBoolOption(QDomElement root, std::string name, BoolParameter parameter); ///< constructs a bool option
	DoubleDataAdapterXmlPtr makeDoubleOption(QDomElement root, std::string name, NumericParameter parameter); ///< constructs a double option

	TSFPresetsPtr populatePresets(); ///< converts the parameters files to internal presets

	QString mParameterFile; ///< the last selected parameter file

	std::vector<StringDataAdapterXmlPtr> mStringOptions; ///< string options to be displayed to the user
	std::vector<BoolDataAdapterXmlPtr> mBoolOptions; ///< bool options to be displayed to the user
	std::vector<DoubleDataAdapterXmlPtr> mDoubleOptions; ///< double options to be displayed to the user

	TSFOutput* mOutput; ///< output from last execution
	paramList mParameters; ///< the parameters used in last execution

	TSFPresetsPtr mPresets;

};
typedef boost::shared_ptr<class TubeSegmentationFilter> TubeSegmentationFilterPtr;

#endif //CX_USE_TSF

} /* namespace cx */
#endif /* CXTUBESEGMENTATIONFILTER_H_ */
