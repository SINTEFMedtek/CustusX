// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCPROGRESSIVELODVOLUMEREP_H_
#define SSCPROGRESSIVELODVOLUMEREP_H_

#include <vector>
#include "sscVolumetricRep.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
// forward declarations
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class VolumetricRep> VolumetricRepPtr;

typedef boost::shared_ptr<class ProgressiveLODVolumetricRep> ProgressiveLODVolumetricRepPtr;

/**\brief Volume texture rendering of one image.
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
 * \ingroup sscRep
 * \ingroup sscRep3D
 */
class ProgressiveLODVolumetricRep : public VolumetricBaseRep
{
	Q_OBJECT
public:
	virtual ~ProgressiveLODVolumetricRep();

	static ProgressiveLODVolumetricRepPtr New(const QString& uid, const QString& name="");

	virtual QString getType() const { return "ssc::ProgressiveLODVolumetricRep"; }
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
	ProgressiveLODVolumetricRep(const QString& uid, const QString& name="");
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

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
