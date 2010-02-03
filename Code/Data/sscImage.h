#ifndef SSCIMAGE_H_
#define SSCIMAGE_H_

#include <set>

#include <boost/shared_ptr.hpp>

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageReslice> vtkImageReslicePtr;
typedef vtkSmartPointer<class vtkPoints> vtkPointsPtr;
typedef vtkSmartPointer<class vtkDoubleArray> vtkDoubleArrayPtr;
typedef vtkSmartPointer<class vtkImageAccumulate> vtkImageAccumulatePtr;

#include "sscData.h"
#include "sscRep.h"
#include "sscImageTF3D.h"
typedef boost::shared_ptr<IntIntMap> HistogramMapPtr;

class QDomNode;
class QDomDocument;

#include "sscForwardDeclarations.h"

//#define USE_TRANSFORM_RESCLICER

namespace ssc
{

/**One volumetric data set, represented as a vtkImageData,
 * along with auxiliary data.
 *
 * warning Landmarks are only used by SINTEF atm, so they can change the code
 * at any given point.
 */
class Image : public Data
{
	Q_OBJECT
public:
	virtual ~Image();
	Image(const std::string& uid, const vtkImageDataPtr& data);
	Image(const std::string& uid, const std::string& name,
				const vtkImageDataPtr& data);
	void setVtkImageData(const vtkImageDataPtr& data);

	void setName(const std::string& name);
	virtual void set_rMd(Transform3D rMd);

	virtual std::string getUid() const;
	virtual std::string getName() const;
	virtual REGISTRATION_STATUS getRegistrationStatus() const;

	virtual vtkImageDataPtr getBaseVtkImageData(); ///< \return the vtkimagedata in the data coordinate space
	virtual vtkImageDataPtr getGrayScaleBaseVtkImageData(); ///< as getBaseVtkImageData(), but constrained to 1 component if multicolor.
	virtual vtkImageDataPtr getRefVtkImageData(); ///< \return the vtkimagedata in the reference coordinate space
	virtual vtkDoubleArrayPtr getLandmarks(); ///< \return all landmarks defined on the image.

	ImageTF3DPtr getTransferFunctions3D();
	ImageLUT2DPtr getLookupTable2D();

	void connectRep(const RepWeakPtr& rep); ///< called by Rep when connecting to an Image
	void disconnectRep(const RepWeakPtr& rep); ///< called by Rep when disconnecting from an Image
	void printLandmarks(); //TODO: JUST FOR TESTING
	DoubleBoundingBox3D boundingBox() const; ///< bounding box in image space
	vtkImageAccumulatePtr getHistogram();///< \return The histogram for the image
	int getMax();///< \return Max alpha position in the histogram = max key value in map
	int getMin();///< \return Min alpha position in the histogram = min key value in map
	int getRange();///< For convenience: getMax() – getMin()
	int getMaxAlphaValue();///<Max alpha value (probably 255)

	void addXml(QDomNode& parentNode); ///< adds xml information about the image and its variabels
	void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

//	/**Return a reference to a GL texture buffer collection for the image.
//	 * If no buffer exist, it is created. When noone uses the buffer, it is released. */
//	GPUImageBufferPtr getGPUBuffer();
	
signals:
	void landmarkRemoved(double x, double y, double z, unsigned int index);
	void landmarkAdded(double x, double y, double z, unsigned int index);
	void vtkImageDataChanged(); ///< emitted when the vktimagedata are invalidated and must be retrieved anew.
	void transferFunctionsChanged(); ///< emitted when image transfer functions in 2D or 3D are changed.
	void transformChanged(); ///< emitted when transform is changed
	//void alphaChange(); ///<blending alpha
	//void thresholdChange(double val); 

public slots:
	void addLandmarkSlot(double x, double y, double z, unsigned int index);
	void removeLandmarkSlot(double x, double y, double z, unsigned int index);
	
protected slots:
	void transferFunctionsChangedSlot();

protected:
	ImageTF3DPtr mImageTransferFunctions3D;
	ImageLUT2DPtr mImageLookupTable2D;

	std::string mUid;
	std::string mName;

	std::set<RepWeakPtr> mReps; ///< links to Rep users.
	
	vtkImageDataPtr mBaseImageData; ///< image data in data space
	vtkImageDataPtr mBaseGrayScaleImageData; ///< image data in data space
	vtkImageReslicePtr mOrientator; ///< converts imagedata to outputimagedata
	vtkImageDataPtr mOutputImageData; ///< imagedata after filtering through the orientatior, given in reference space
	vtkDoubleArrayPtr mLandmarks; ///< array consists of 4 components (<x,y,z,index>) for each tuple (landmark)	
	vtkImageAccumulatePtr mHistogramPtr;///< Histogram
//	GPUImageBufferWeakPtr mBuffer; 
};

typedef boost::shared_ptr<Image> ImagePtr;

} // end namespace ssc

#endif /*SSCIMAGE_H_*/
