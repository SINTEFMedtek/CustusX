#ifndef SSCVOLUMEREP_H_
#define SSCVOLUMEREP_H_

#include "sscRepImpl.h"

#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkPiecewiseFunction> vtkPiecewiseFunctionPtr;
typedef	vtkSmartPointer<class vtkColorTransferFunction> vtkColorTransferFunctionPtr;
typedef vtkSmartPointer<class vtkVolumeProperty> vtkVolumePropertyPtr;
typedef vtkSmartPointer<class vtkVolumeTextureMapper3D> vtkVolumeTextureMapper3DPtr;
typedef	vtkSmartPointer<class vtkVolume> vtkVolumePtr;
typedef	vtkSmartPointer<class vtkVolume> vtkVolumePtr;

namespace ssc
{
typedef boost::shared_ptr<class Image> ImagePtr;

typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;
typedef boost::shared_ptr<class ImageMapperMonitor> ImageMapperMonitorPtr;

/**Representation for one Image.
 * Use this to render volumetric image data in a 3D scene.
 * 
 * Used by Sonowand.
 * Used by CustusX.
 */
class VolumetricRep : public RepImpl
{
	Q_OBJECT
public:
	virtual ~VolumetricRep();

	static VolumetricRepPtr New(const std::string& uid, const std::string& name="");
	virtual std::string getType() const { return "ssc::VolumetricRep"; }
	virtual void setImage(ImagePtr image); ///< set the reps image
	virtual ImagePtr getImage(); ///< get the reps image
	virtual bool hasImage(ImagePtr image) const; ///< check if the reps has the image
	vtkVolumePtr getVtkVolume() { return mVolume; } ///< get the images vtkVolume
	virtual void setResampleFactor(double factor); ///< set how detailed the image should be
	
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
	ImageMapperMonitorPtr mMonitor; ///< helper object for visualizing clipping/cropping

private slots:
	void transformChangedSlot();
	void vtkImageDataChangedSlot();
  void transferFunctionsChangedSlot();
};
//---------------------------------------------------------
} // namespace ssc
//---------------------------------------------------------

#endif /*SSCVOLUMEREP_H_*/
