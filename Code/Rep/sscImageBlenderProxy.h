#ifndef SSCIMAGEBLENDERPROXY_H_
#define SSCIMAGEBLENDERPROXY_H_

#include <vector>
#include <QtCore>
#include <vtkSmartPointer.h>
#include <vtkImageThreshold.h>
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"
#include "sscImage.h"

typedef vtkSmartPointer<class vtkImageMapToWindowLevelColors> vtkImageMapToWindowLevelColorsPtr;
typedef vtkSmartPointer<class vtkWindowLevelLookupTable> vtkWindowLevelLookupTablePtr;
typedef vtkSmartPointer<class vtkImageMapToColors> vtkImageMapToColorsPtr;
typedef vtkSmartPointer<class vtkImageResample> vtkImageResamplePtr;
typedef vtkSmartPointer<class vtkImageBlend> vtkImageBlendPtr;

//----------------------------------------------------------
namespace ssc
{
//----------------------------------------------------------
typedef boost::shared_ptr<class ImageBlenderProxy> ImageBlenderProxyPtr;

/**Part of sonowand test implementation of image blending.
 * 
 * NOT IN USE!
 * 
 */
class ImageBlenderProxy : public QObject
{
	Q_OBJECT
	
public:
	ImageBlenderProxy();
	virtual ~ImageBlenderProxy();
	virtual std::string getType() const { return "ssc::ImageBlenderProxy"; }
	bool hasImage(ImagePtr image) const;
	void clearImages();
	void addImage(ImagePtr image);
	vtkImageDataPtr getOutput();
	std::vector<ImagePtr> getImages( ) { return mImages; };
	void removeImage(ImagePtr image);
	void createOverlay();
	void updateBlender();	
	void updateAlpha();
	void sliceFirstTheneBlend();
	void volumeBlend();
	void addSlice(vtkImageDataPtr slice);
private:	
	bool firstImage;
	vtkImageDataPtr baseImage;
	vtkImageBlendPtr mBlender;
	std::vector<ImagePtr> mImages;
	std::string mImageUid;
	
	vtkWindowLevelLookupTablePtr mLookupTable;
	vtkImageMapToWindowLevelColorsPtr mWindowLevel;
	vtkImageMapToWindowLevelColorsPtr mColorMapper;
	vtkImageThreshold* mTreshold;
};

//----------------------------	------------------------------
}//end namespace
//----------------------------------------------------------
#endif /*SSCIMAGEBLENDERPROXY_H_*/
