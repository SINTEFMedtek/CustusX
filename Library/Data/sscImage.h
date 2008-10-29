#ifndef SSCIMAGE_H_
#define SSCIMAGE_H_

#include <set>

#include <boost/shared_ptr.hpp>

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPoints> vtkPointsPtr;

#include "sscData.h"
#include "sscRep.h"

namespace ssc
{

typedef boost::weak_ptr<class ImageProxy> ImageProxyWeakPtr;

/**One volumetric data set, represented as a vtkImageData,
 * along with auxiliary data.
 */
class Image : public Data
{
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

	virtual vtkImageDataPtr getVtkImageData(); ///< \return the vtkimagedata in the data coordinate space
	virtual vtkPointsPtr getLandmarks(); ///< \return all landmarks defined on the image.
//	virtual vtkLookupTablePtr getLut() const = 0;

	void connectRep(const RepWeakPtr& rep); ///< called by Rep when connecting to an Image
	void disconnectRep(const RepWeakPtr& rep); ///< called by Rep when disconnecting from an Image
private:
	std::string mUid;
	std::string mName;
	Transform3D mTransform;

	std::set<RepWeakPtr> mReps; ///< links to Rep users.

	vtkImageDataPtr mImageData;
	vtkPointsPtr mLandmarks;
};

typedef boost::shared_ptr<Image> ImagePtr;

} // end namespace ssc

#endif /*SSCIMAGE_H_*/
