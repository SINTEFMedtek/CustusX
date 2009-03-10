#ifndef SSCVOLUMEREP_H_
#define SSCVOLUMEREP_H_

#include <vtkSmartPointer.h>
#include "sscRepImpl.h"
#include "sscImage.h"

typedef vtkSmartPointer<class vtkPiecewiseFunction> vtkPiecewiseFunctionPtr;
typedef	vtkSmartPointer<class vtkColorTransferFunction> vtkColorTransferFunctionPtr;
typedef vtkSmartPointer<class vtkVolumeProperty> vtkVolumePropertyPtr;
typedef vtkSmartPointer<class vtkVolumeTextureMapper3D> vtkVolumeTextureMapper3DPtr;
typedef	vtkSmartPointer<class vtkVolume> vtkVolumePtr;
typedef	vtkSmartPointer<class vtkVolume> vtkVolumePtr;

namespace ssc
{
typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;

/**Represenation for one Image.
 * Use this to render volumetric image data in a 3D scene.
 */
class VolumetricRep : public RepImpl
{
	Q_OBJECT
public:
	virtual ~VolumetricRep();

	static VolumetricRepPtr New(const std::string& uid, const std::string& name="");
	virtual std::string getType() const { return "ssc::VolumeRep"; }
	virtual void setImage(ImagePtr image);
	virtual ImagePtr getImage();
	virtual bool hasImage(ImagePtr image) const;
	vtkVolumePtr getVtkVolume() { return mVolume; }
	virtual void setResampleFactor(double factor);	
	
protected:
	VolumetricRep(const std::string& uid, const std::string& name="");
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	vtkPiecewiseFunctionPtr mOpacityTransferFunction;
	vtkColorTransferFunctionPtr mColorTransferFunction;
	vtkVolumePropertyPtr mVolumeProperty;
	vtkVolumeTextureMapper3DPtr mTextureMapper3D;
	vtkVolumePtr mVolume;

	ImagePtr mImage;
	double mResampleFactor;

private slots:
	void transformChangedSlot();
	void vtkImageDataChangedSlot();
};
//---------------------------------------------------------
} // namespace ssc
//---------------------------------------------------------

#endif /*SSCVOLUMEREP_H_*/
