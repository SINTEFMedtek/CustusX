#ifndef SSCVOLUMEREP_H_
#define SSCVOLUMEREP_H_

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkPiecewiseFunction> vtkPiecewiseFunctionPtr;
typedef	vtkSmartPointer<class vtkColorTransferFunction> vtkColorTransferFunctionPtr;
typedef vtkSmartPointer<class vtkVolumeProperty> vtkVolumePropertyPtr;
typedef vtkSmartPointer<class vtkVolumeTextureMapper3D> vtkVolumeTextureMapper3DPtr;
typedef	vtkSmartPointer<class vtkVolume> vtkVolumePtr;

#include "sscRepImpl.h"
#include "sscDataProxy.h"

namespace ssc
{

/**Represenation for one Image.
 * Use this to render volumetric image data in a 3D scene. 
 */
class VolumeRep : public RepImpl
{
public:
	VolumeRep(const std::string& uid, const std::string& name, ImagePtr image);
	VolumeRep();
	virtual ~VolumeRep();
	virtual std::string getType() const { return "ssc::VolumeRep"; }
protected:
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);
	
	vtkPiecewiseFunctionPtr mOpacityTransferFunction;
	vtkColorTransferFunctionPtr mColorTransferFunction;
	vtkVolumePropertyPtr mVolumeProperty;
	vtkVolumeTextureMapper3DPtr mTextureMapper3D;
	vtkVolumePtr mVolume;
	ImageProxyPtr mProxy;
};

} // namespace ssc

#endif /*SSCVOLUMEREP_H_*/
