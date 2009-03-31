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
	void setVtkImageData(const vtkImageDataPtr& data);

	void setName(const std::string& name);
	void setTransform(const Transform3D& trans);

	virtual std::string getUid() const;
	virtual std::string getName() const;
	virtual Transform3D getTransform() const;
	virtual REGISTRATION_STATUS getRegistrationStatus() const;

	virtual vtkImageDataPtr getBaseVtkImageData(); ///< \return the vtkimagedata in the data coordinate space
	virtual vtkImageDataPtr getRefVtkImageData(); ///< \return the vtkimagedata in the reference coordinate space
	virtual vtkDoubleArrayPtr getLandmarks(); ///< \return all landmarks defined on the image.

	ImageTF3D& getTransferFunctions3D();
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

protected:
	ImageTF3D mImageTransferFunctions3D;
	ImageLUT2D mImageLookupTable2D;

	std::string mUid;
	std::string mName;
	Transform3D mTransform; ///< the transform from data to reference space
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
};

typedef boost::shared_ptr<Image> ImagePtr;

} // end namespace ssc

#endif /*SSCIMAGE_H_*/
