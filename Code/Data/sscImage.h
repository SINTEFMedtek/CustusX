#ifndef SSCIMAGE_H_
#define SSCIMAGE_H_

#include <map>
#include <boost/shared_ptr.hpp>
#include "sscLandmark.h"

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageReslice> vtkImageReslicePtr;
typedef vtkSmartPointer<class vtkPoints> vtkPointsPtr;
typedef vtkSmartPointer<class vtkDoubleArray> vtkDoubleArrayPtr;
typedef vtkSmartPointer<class vtkImageAccumulate> vtkImageAccumulatePtr;

#include "sscData.h"
typedef boost::shared_ptr<std::map<int, int> > HistogramMapPtr;

class QDomNode;
class QDomDocument;

//#define USE_TRANSFORM_RESCLICER

namespace ssc
{
// forward declarations
typedef std::map<std::string, class Landmark> LandmarkMap;
typedef boost::shared_ptr<class ImageTF3D> ImageTF3DPtr;
typedef boost::shared_ptr<class ImageLUT2D> ImageLUT2DPtr;

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
	Image(const std::string& uid, const vtkImageDataPtr& data, const std::string& name="");
	void setVtkImageData(const vtkImageDataPtr& data);

	virtual vtkImageDataPtr getBaseVtkImageData(); ///< \return the vtkimagedata in the data coordinate space
	virtual vtkImageDataPtr getGrayScaleBaseVtkImageData(); ///< as getBaseVtkImageData(), but constrained to 1 component if multicolor.
	virtual vtkImageDataPtr getRefVtkImageData(); ///< \return the vtkimagedata in the reference coordinate space
	virtual LandmarkMap getLandmarks(); ///< \return all landmarks defined on the image.

	ImageTF3DPtr getTransferFunctions3D();
	ImageLUT2DPtr getLookupTable2D();

	void printLandmarks(); //TODO: JUST FOR TESTING
	DoubleBoundingBox3D boundingBox() const; ///< bounding box in image space
	vtkImageAccumulatePtr getHistogram();///< \return The histogram for the image
	int getMax();///< \return Max alpha position in the histogram = max key value in map. Use getPosMax() instead to allow for signed volumes.
	int getMin();///< \return Min alpha position in the histogram = min key value in map Use getPosMin() instead to allow for signed volumes.
	int getPosMax();///< \return Max alpha position modified by getMin(). For use with signed volumes.
	int getPosMin();///< \return Min alpha position set to zero.
	int getRange();///< For convenience: getMax() – getMin()
	int getMaxAlphaValue();///<Max alpha value (probably 255)

	void addXml(QDomNode& parentNode); ///< adds xml information about the image and its variabels \param parentNode Parent node in the XML tree \return The created subnode
	virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

//	/**Return a reference to a GL texture buffer collection for the image.
//	 * If no buffer exist, it is created. When noone uses the buffer, it is released. */
//	GPUImageBufferPtr getGPUBuffer();
	
signals:
  void landmarkRemoved(std::string uid);
  void landmarkAdded(std::string uid);
	void vtkImageDataChanged(); ///< emitted when the vktimagedata are invalidated and must be retrieved anew.
	void transferFunctionsChanged(); ///< emitted when image transfer functions in 2D or 3D are changed.

public slots:
  void setLandmark(Landmark landmark);
  void removeLandmark(std::string uid);
	
protected slots:
	void transferFunctionsChangedSlot();
  virtual void transformChangedSlot();

protected:
	ImageTF3DPtr mImageTransferFunctions3D;
	ImageLUT2DPtr mImageLookupTable2D;
	
	vtkImageDataPtr mBaseImageData; ///< image data in data space
	vtkImageDataPtr mBaseGrayScaleImageData; ///< image data in data space
	vtkImageReslicePtr mOrientator; ///< converts imagedata to outputimagedata
	vtkMatrix4x4Ptr mOrientatorMatrix;
	vtkImageDataPtr mReferenceImageData; ///< imagedata after filtering through the orientatior, given in reference space
	vtkImageAccumulatePtr mHistogramPtr;///< Histogram

	//landmarks
	LandmarkMap mLandmarks; ///< map with all landmarks always in space d (data).
};

typedef boost::shared_ptr<Image> ImagePtr;

} // end namespace ssc

#endif /*SSCIMAGE_H_*/
