#ifndef SSCIMAGE_H_
#define SSCIMAGE_H_

#include <set>

#include <boost/shared_ptr.hpp>

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageReslice> vtkImageReslicePtr;
typedef vtkSmartPointer<class vtkPoints> vtkPointsPtr;
typedef vtkSmartPointer<class vtkDoubleArray> vtkDoubleArrayPtr;

#include "sscData.h"
#include "sscRep.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
typedef boost::shared_ptr<IntIntMap> HistogramMapPtr;

//#define USE_TRANSFORM_RESCLICER

namespace ssc
{

//typedef boost::weak_ptr<class ImageProxy> ImageProxyWeakPtr;

/**One volumetric data set, represented as a vtkImageData,
 * along with auxiliary data.
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
	virtual vtkImageDataPtr getRefVtkImageData(); ///< \return the vtkimagedata in the reference coordinate space
	virtual vtkDoubleArrayPtr getLandmarks(); ///< \return all landmarks defined on the image.

	ImageTF3DPtr getTransferFunctions3D();
	ImageLUT2D& getLookupTable2D();

	void connectRep(const RepWeakPtr& rep); ///< called by Rep when connecting to an Image
	void disconnectRep(const RepWeakPtr& rep); ///< called by Rep when disconnecting from an Image
	void printLandmarks(); //TODO: JUST FOR TESTING
	double getAlpha();//TODO: JUST FOR TESTING
	void setAlpha(double val);//TODO: JUST FOR TESTING
	DoubleBoundingBox3D boundingBox() const;
	///preset colorTabel
	void setClut(vtkLookupTablePtr clut);
	double treshold();
	void setTreshold( double val );
	HistogramMapPtr getHistogram();///< \return The histogram for the image
	int getMaxHistogramValue();///< \return Max number of occurences for a single point in the histogram
	int getMax();///< \return Max alpha position in the histogram = max key value in map
	int getMin();///< \return Min alpha position in the histogram = min key value in map
	int getRange();///< For convenience: getMax() – getMin()
	int getMaxAlphaValue();///<Max alpha value (probably 255)

	QDomNode getXml(QDomDocument& doc);///< Create a XML node for this object. It's up to the caller to add this node to the XML tree. \return A XML data representation for this object. \param doc The root of the document tree.
	void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

signals:
	void landmarkRemoved(double x, double y, double z, unsigned int index);
	void landmarkAdded(double x, double y, double z, unsigned int index);
	void vtkImageDataChanged(); ///< emitted when the vktimagedata are invalidated and must be retrieved anew.
	void transformChanged(); ///< emitted when transform is changed
	void alphaChange(); ///<blending alpha
	void thresholdChange(double val); 

public slots:
	void addLandmarkSlot(double x, double y, double z, unsigned int index);
	void removeLandmarkSlot(double x, double y, double z, unsigned int index);
	
protected slots:
	void transferFunctionsChangedSlot();

protected:
	ImageTF3DPtr mImageTransferFunctions3D;
	ImageLUT2D mImageLookupTable2D;

	std::string mUid;
	std::string mName;
	vtkLookupTablePtr mLut;
	std::set<RepWeakPtr> mReps; ///< links to Rep users.
	vtkImageDataPtr mBaseImageData; ///< image data in data space
	vtkImageReslicePtr mOrientator; ///< converts imagedata to outputimagedata
	vtkImageDataPtr mOutputImageData; ///< imagedata after filtering through the orientatior, given in reference space

	//Cannot remove points from vtkPoint, using vtkDoubleArray instead,
	// to create vtkPoints if/when needed:
	// vtkPointsPtr points = vtkPoints::New();
	// points->SetData(mLandmarks);
	//vtkPointsPtr mLandmarks;
	vtkDoubleArrayPtr mLandmarks; ///< array consists of 4 components (<x,y,z,index>) for each tuple (landmark)
	double mAlpha ;
	double mTreshold;
	
	HistogramMapPtr mHistogramPtr;///< Histogram
	int mMaxHistogramValue;///< Max number of occurences for a single point in the histogram
};

typedef boost::shared_ptr<Image> ImagePtr;

} // end namespace ssc

#endif /*SSCIMAGE_H_*/
