#ifndef SSCPROGRESSIVELODVOLUMEREP_H_
#define SSCPROGRESSIVELODVOLUMEREP_H_

#include <vector>
#include "sscVolumetricRep.h"

#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkAssembly> vtkAssemblyPtr;

namespace ssc
{
// forward declarations
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;

typedef boost::shared_ptr<class ProgressiveLODVolumetricRep> ProgressiveLODVolumetricRepPtr;

/**Volume rendering of one image.
 *
 * Use this in place of VolumetricRep when the initial render time is high.
 * The rendering will be performed at progressively higher sample rates, thus
 * the user will get immediate feedback at low detail, and when the high-resolution
 * rendering is finished (in another thread) the display will be updated.
 * 
 * Used by Sonowand.  
 * 
 * Not working for Windows and Mac. OK on Fedora.
 *
 */
class ProgressiveLODVolumetricRep : public VolumetricBaseRep
{
	Q_OBJECT
public:
	virtual ~ProgressiveLODVolumetricRep();

	static ProgressiveLODVolumetricRepPtr New(const std::string& uid, const std::string& name="");

	virtual std::string getType() const { return "ssc::ProgressiveLODVolumetricRep"; }
	virtual void setImage(ImagePtr image);
	virtual ImagePtr getImage();
	virtual bool hasImage(ImagePtr image) const;
	
//virtual void setImage(ImagePtr image) = 0; ///< set the reps image
//irtual ImagePtr getImage() = 0; ///< get the reps image
//	virtual bool hasImage(ImagePtr image) const = 0; ///< check if the reps has the image
	virtual vtkVolumePtr getVtkVolume(); ///< get the images vtkVolume
	//virtual void setResampleFactor(double factor); ///< set how detailed the image should be
	virtual void setMaxVolumeSize(long maxVoxels); ///< set max volume size for rendering. Must be set before setImage()	


protected:
	ProgressiveLODVolumetricRep(const std::string& uid, const std::string& name="");
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	VolumetricRepPtr getNextResampleLevel();
	void startThread(VolumetricRepPtr rep);
	VolumetricRepPtr mCurrent;
	bool mClearing; ///< quick fix for shutting down faster
	vtkAssemblyPtr mAssembly;
	View* mView;
	std::vector<double> mResampleFactors;
	long mMaxVoxels; ///< always resample volume below this size.

	ImagePtr mImage;
	void resetResampleList();
	void clearThreads();
	typedef boost::shared_ptr<class VolumetricRepThreadedRenderer> VolumetricRepThreadedRendererPtr;
	VolumetricRepThreadedRendererPtr mThread;


private slots:
	void volumetricThreadFinishedSlot();
};

} // namespace ssc

#endif /*SSCPROGRESSIVELODVOLUMEREP_H_*/
