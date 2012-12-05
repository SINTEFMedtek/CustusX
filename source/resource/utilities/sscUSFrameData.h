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

#ifndef SSCUSFRAMEDATA_H_
#define SSCUSFRAMEDATA_H_

#include <vector>
#include "sscImage.h"
#include "sscProbeSector.h"
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;

namespace cx
{
typedef boost::shared_ptr<class ImageDataContainer> ImageDataContainerPtr;
typedef boost::shared_ptr<class CachedImageData> CachedImageDataPtr;
}

namespace ssc
{


/**
 * \addtogroup cxResourceUtilities
 * \{
 */

typedef boost::shared_ptr<class USFrameData> USFrameDataPtr;

typedef boost::shared_ptr<class TimedPosition> TimedPositionPtr;
/** \brief One position with timestamp
 */
class TimedPosition
{
public:
	double mTime;// Should always be in ms
	Transform3D mPos;
};
inline bool operator<(const TimedPosition& lhs, const TimedPosition& rhs)
{
	return lhs.mTime < rhs.mTime;
}

/**\brief Helper class encapsulating a 2S US data set.
 *
 * The class is a thin wrapper around a Image , with the
 * added possibility to remove single frames. (The Image is defined
 * as consisting of a sequence of frames).
 *
 * Note on thread safety: This class is NOT thread-safe in general,
 * because VTK is not thread-safe (ref-counting is not mutexed).
 *
 * However, if the object is properly initialized from one thread
 * (meaning generateCache() has been called), then the const methods
 * can be called.
 *
 *  \date August 17, 2010
 *  \date Nov 06, 2012
 *  \author Ole Vegard Solberg
 *  \author christiana
 */
class USFrameData
{
public:
	static USFrameDataPtr create(ImagePtr inputFrameData);
//	static USFrameDataPtr create(std::vector<vtkImageDataPtr> inputFrameData, QString filename);
	static USFrameDataPtr create(QString inputFilename);
	static USFrameDataPtr create(QString filename, std::vector<cx::CachedImageDataPtr> frames);
	~USFrameData();

	unsigned char* getFrame(unsigned int index) const;
	Eigen::Array3i getDimensions() const;
	Vector3D getSpacing() const;
	QString getName() const;
	QString getUid() const;
	QString getFilePath() const;

	void removeFrame(unsigned int index);
	void setAngio(bool angio);///< Use only angio data as input. reinitialize() must be called afterwards
	void setCropBox(IntBoundingBox3D mCropbox);
//	bool save(QString filename, bool compressed);
	void fillImageImport(vtkImageImportPtr import, int index); ///< fill import with a single frame
	void initializeFrames(); ///< call to enable use of getFrame()

	virtual USFrameDataPtr copy();

protected:
	void initialize();
	virtual void clearCache();
	void generateCache();
//	virtual vtkImageDataPtr getSingleBaseImage();
	USFrameData();
	vtkImageDataPtr useAngio(vtkImageDataPtr inData) const;/// Use only US angio data as input. Removes grayscale from the US data and converts the remaining color to grayscale
//	vtkImageDataPtr mergeFrames(std::vector<vtkImageDataPtr> input) const;

	vtkImageDataPtr cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox) const;
	vtkImageDataPtr toGrayscale(vtkImageDataPtr input) const;

	std::vector<int> mReducedToFull; ///< map from indexes in the reduced volume to the full (original) volume.
	bool mUseAngio;
	IntBoundingBox3D mCropbox;

	QString mFilename;
//	vtkImageDataPtr mOptionalWholeBase; ///< handle for original monolithic data if present
//	std::vector<vtkImageDataPtr> mBaseImage;
	std::vector<vtkImageDataPtr> mProcessedImage;
	cx::ImageDataContainerPtr mImageContainer;
};

struct USReconstructInputData
{
	QString mFilename; ///< filename used for current data read

	ssc::USFrameDataPtr mUsRaw;///<All imported US data frames with pointers to each frame
	std::vector<ssc::TimedPosition> mFrames;
	std::vector<ssc::TimedPosition> mPositions;
	ssc::ImagePtr mMask;///< Clipping mask for the input data
	ssc::ProbeSector mProbeData;
	QString mProbeUid;
};

/**
 * \}
 */

}//namespace ssc

#endif // SSCUSFRAMEDATA_H_
