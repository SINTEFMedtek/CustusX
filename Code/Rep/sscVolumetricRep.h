#ifndef SSCVOLUMEREP_H_
#define SSCVOLUMEREP_H_

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkPiecewiseFunction> vtkPiecewiseFunctionPtr;
typedef	vtkSmartPointer<class vtkColorTransferFunction> vtkColorTransferFunctionPtr;
typedef vtkSmartPointer<class vtkVolumeProperty> vtkVolumePropertyPtr;
typedef vtkSmartPointer<class vtkVolumeTextureMapper3D> vtkVolumeTextureMapper3DPtr;
typedef	vtkSmartPointer<class vtkVolume> vtkVolumePtr;

#include "sscRepImpl.h"
#include "sscImage.h"

namespace ssc
{
typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;

/**Represenation for one Image.
 * Use this to render volumetric image data in a 3D scene.
 */
class VolumetricRep : public RepImpl
{
public:
	virtual ~VolumetricRep();

	static VolumetricRepPtr create(const std::string& uid);

	virtual std::string getType() const { return "ssc::VolumeRep"; }
	virtual void setImage(ImagePtr image);
	//void addImage(ImagePtr image);
	//void removeImage(ImagePtr image);
	virtual bool hasImage(ImagePtr image) const;
protected:
	VolumetricRep(const std::string& uid);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	vtkPiecewiseFunctionPtr mOpacityTransferFunction;
	vtkColorTransferFunctionPtr mColorTransferFunction;
	vtkVolumePropertyPtr mVolumeProperty;
	vtkVolumeTextureMapper3DPtr mTextureMapper3D;
	vtkVolumePtr mVolume;

	ImagePtr mImage;
};

} // namespace ssc

#endif /*SSCVOLUMEREP_H_*/
