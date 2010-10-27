#ifndef SSCIMAGEBLENDERPROXY_H_
#define SSCIMAGEBLENDERPROXY_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <vector>

#include "vtkForwardDeclarations.h"

//----------------------------------------------------------
namespace ssc
{
//----------------------------------------------------------
typedef boost::shared_ptr<class Image> ImagePtr;

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
	virtual QString getType() const { return "ssc::ImageBlenderProxy"; }
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
	QString mImageUid;
	
	vtkWindowLevelLookupTablePtr mLookupTable;
	vtkImageMapToWindowLevelColorsPtr mWindowLevel;
	vtkImageMapToWindowLevelColorsPtr mColorMapper;
	vtkImageThreshold* mTreshold;
};

//----------------------------	------------------------------
}//end namespace
//----------------------------------------------------------
#endif /*SSCIMAGEBLENDERPROXY_H_*/
