#ifndef SSCDATAPROXY_H_
#define SSCDATAPROXY_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "sscImage.h"

namespace ssc
{

typedef boost::shared_ptr<class Image> ImagePtr;

class ImageProxy
{
public:
	void setImage(ImagePtr image);
	void addImage(ImagePtr image);
	void removeImage(ImagePtr image);
	void hasImage(ImagePtr image) const;
//	std::vector<ImagePtr> getImages();
//	void removeImages();

private:
	ImagePtr mImage;
};

typedef boost::shared_ptr<ImageProxy> ImageProxyPtr;
typedef boost::weak_ptr<ImageProxy> ImageProxyWeakPtr;

}

#endif /*SSCDATAPROXY_H_*/
