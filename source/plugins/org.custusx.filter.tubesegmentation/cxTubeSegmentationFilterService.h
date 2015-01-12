/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXTUBESEGMENTATIONFILTERSERVICE_H_
#define CXTUBESEGMENTATIONFILTERSERVICE_H_

#include "org_custusx_filter_tubesegmentation_Export.h"

//#include "C:\Dev\cx\CustusX\CustusX\source\resource\filter\cxFilter.h"
//#include "C:\Dev\cx\CustusX\CustusX\source\resource\filter\cxFilterImpl.h"
#include "cxFilter.h"
#include "cxFilterImpl.h"

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

#include "parameters.hpp"
#include "cxPatientModelService.h"

#include "cxStringProperty.h"
#include "cxBoolProperty.h"

class TSFOutput;
class ctkPluginContext;

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;

namespace cx {
typedef boost::shared_ptr<class TSFPresets> TSFPresetsPtr;

/**
 * Filter for segmenting and extract the centerline of a volume.
 *
 * This filter can run either on the gpu or cpu.
 *
 * Algorithm written by Erik Smistad. For more information, see paper:
 * "GPU-Based Airway Segmentation and Centerline Extraction for Image Guided Bronchoscopy."
 *
 * \ingroup cx_module_algorithm
 * \date 14.12.2012
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_filter_tubesegmentation_EXPORT TubeSegmentationFilter : public FilterImpl
{
Q_OBJECT
Q_INTERFACES(cx::Filter)

public:
	TubeSegmentationFilter(ctkPluginContext *pluginContext);
	virtual ~TubeSegmentationFilter(); 

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

} /* namespace cx */
#endif /* CXTUBESEGMENTATIONFILTERSERVICE_H_ */
