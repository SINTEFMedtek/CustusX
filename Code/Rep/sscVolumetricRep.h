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

typedef boost::shared_ptr<class VolumetricBaseRep> VolumetricBaseRepPtr;
typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;
typedef boost::shared_ptr<class ImageMapperMonitor> ImageMapperMonitorPtr;

/**3D representation for one image.
 * 
 * This abstract interface is implemented by VolumetricRep or ProgressiveLODVolumetricRep.
 * 
 */
class VolumetricBaseRep : public RepImpl
{
	Q_OBJECT
public:
	virtual ~VolumetricBaseRep() {}

	virtual void setImage(ImagePtr image) = 0; ///< set the reps image
	virtual ImagePtr getImage() = 0; ///< get the reps image
	virtual bool hasImage(ImagePtr image) const = 0; ///< check if the reps has the image
	virtual vtkVolumePtr getVtkVolume() = 0; ///< get the images vtkVolume
	//virtual void setResampleFactor(double factor) = 0; ///< set how detailed the image should be
	virtual void setMaxVolumeSize(long maxVoxels) = 0; ///< set max volume size for rendering. Must be set before setImage()
signals:
	void internalVolumeChanged(); ///< emitted when the internal vtkVolume has been replaced with a new one (happens with ProgressiveLODVolumetricRep)
protected:
	VolumetricBaseRep(const QString& uid, const QString& name) : RepImpl(uid,name) {}
};

/**Representation for one Image.
 * Use this to render volumetric image data in a 3D scene.
 * 
 * Used by Sonowand.
 * Used by CustusX.
 */
class VolumetricRep : public VolumetricBaseRep
{
	Q_OBJECT
public:
	virtual ~VolumetricRep();

	static VolumetricRepPtr New(const QString& uid, const QString& name="");
	virtual QString getType() const { return "ssc::VolumetricRep"; }
	virtual void setImage(ImagePtr image); ///< set the reps image
	virtual ImagePtr getImage(); ///< get the reps image
	virtual bool hasImage(ImagePtr image) const; ///< check if the reps has the image
	virtual vtkVolumePtr getVtkVolume() { return mVolume; } ///< get the images vtkVolume
	virtual void setResampleFactor(double factor); ///< set how detailed the image should be
	void setMaxVolumeSize(long maxVoxels); ///< set max volume size for rendering. Must be set before setImage()

	static double computeResampleFactor(long maxVoxels, ssc::ImagePtr image);
	
protected:
	VolumetricRep(const QString& uid, const QString& name="");
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	vtkPiecewiseFunctionPtr mOpacityTransferFunction;
	vtkColorTransferFunctionPtr mColorTransferFunction;
	vtkVolumePropertyPtr mVolumeProperty;
	vtkVolumeTextureMapper3DPtr mTextureMapper3D;
	vtkVolumePtr mVolume;
	long mMaxVoxels; ///< always resample volume below this size.

	ImagePtr mImage;
	double mResampleFactor;
	ImageMapperMonitorPtr mMonitor; ///< helper object for visualizing clipping/cropping

private slots:
	void transformChangedSlot();
	void vtkImageDataChangedSlot();
  void transferFunctionsChangedSlot();
private:
	void updateResampleFactor();
};
//---------------------------------------------------------
} // namespace ssc
//---------------------------------------------------------

#endif /*SSCVOLUMEREP_H_*/
