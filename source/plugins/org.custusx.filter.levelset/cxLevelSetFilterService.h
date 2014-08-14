#ifndef CXLEVELSETFILTER_H_
#define CXLEVELSETFILTER_H_

#ifdef CX_USE_LEVEL_SET
#include "cxFilterImpl.h"
#include "cxVector3D.h"
#include "org_custusx_filter_levelset_Export.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
namespace cx
{

class org_custusx_filter_levelset_EXPORT LevelSetFilter: public FilterImpl
{
Q_OBJECT

public:
	virtual ~LevelSetFilter()
	{
	}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
	virtual void setActive(bool on);

	bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	DoubleDataAdapterXmlPtr getThresholdOption(QDomElement root);
	DoubleDataAdapterXmlPtr getEpsilonOption(QDomElement root);
	DoubleDataAdapterXmlPtr getAlphaOption(QDomElement root);
	DoubleDataAdapterXmlPtr getRadiusOption(QDomElement root);

	static Vector3D getSeedPointFromTool(DataPtr image);
	static bool isSeedPointInsideImage(Vector3D, DataPtr);
	QDomElement getmOptions();

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();
private:
	vtkImageDataPtr convertToVtkImageData(char * data, int size_x, int size_y,
			int size_z, ImagePtr input); ///< converts a char array to a vtkImageDataPtr
	vtkImageDataPtr importRawImageData(void * data, int size_x, int size_y,
			int size_z, ImagePtr input, int type); ///< converts a any array to a vtkImageDataPtr
	std::string filename;
	Vector3D seedPoint;
	ImagePtr image;
	vtkImageDataPtr rawSegmentation;

};
// end LevelSetFilter class

typedef boost::shared_ptr<class LevelSetFilter> LevelSetFilterPtr;

} // end namespace

#endif // CX_USE_LEVEL_SET
#endif /* CXLEVELSETFILTER_H_ */
