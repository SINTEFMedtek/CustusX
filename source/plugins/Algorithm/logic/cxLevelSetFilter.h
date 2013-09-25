#ifndef CXLEVELSETFILTER_H_
#define CXLEVELSETFILTER_H_

#ifdef CX_USE_LEVEL_SET
#include "cxFilterImpl.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
namespace cx
{

class LevelSetFilter : public FilterImpl
{
	Q_OBJECT

public:
	virtual ~LevelSetFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
	virtual void setActive(bool on);

	bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	ssc::DoubleDataAdapterXmlPtr getThresholdOption(QDomElement root);
	ssc::DoubleDataAdapterXmlPtr getEpsilonOption(QDomElement root);
	ssc::DoubleDataAdapterXmlPtr getAlphaOption(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();
private:
    vtkImageDataPtr convertToVtkImageData(char * data, int size_x, int size_y, int size_z, ssc::ImagePtr input); ///< converts a char array to a vtkImageDataPtr
    vtkImageDataPtr importRawImageData(void * data, int size_x, int size_y, int size_z, ssc::ImagePtr input, int type); ///< converts a any array to a vtkImageDataPtr


}; // end LevelSetFilter class

} // end namespace

#endif // CX_USE_LEVEL_SET
#endif /* CXLEVELSETFILTER_H_ */
