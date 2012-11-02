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

namespace ssc
{

/**
 * \addtogroup sscUSReconstruction
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
 *  \date August 17, 2010
 *  \author Ole Vegard Solberg
 */
class USFrameData
{
public:
	static USFrameDataPtr create(ImagePtr inputFrameData);
	static USFrameDataPtr create(std::vector<vtkImageDataPtr> inputFrameData, QString filename);
	virtual ~USFrameData() {}
	virtual void removeFrame(unsigned int index);
	virtual unsigned char* getFrame(unsigned int index);
	virtual Eigen::Array3i getDimensions();
	virtual Vector3D getSpacing();
	virtual QString getName();
	virtual QString getUid();
	virtual QString getFilePath();
	virtual void setAngio(bool angio);///< Use only angio data as input. reinitialize() must be called afterwards
	virtual void setCropBox(IntBoundingBox3D mCropbox);
	bool save(QString filename, bool compressed);
	virtual void fillImageImport(vtkImageImportPtr import, int index); ///< fill import with a single frame

	virtual USFrameDataPtr copy();

protected:
	void initialize();
	virtual void clearCache();
	void generateCache();
	virtual vtkImageDataPtr getSingleBaseImage();
	USFrameData();
	vtkImageDataPtr useAngio(vtkImageDataPtr inData);/// Use only US angio data as input. Removes grayscale from the US data and converts the remaining color to grayscale
	vtkImageDataPtr mergeFrames(std::vector<vtkImageDataPtr> input);

	vtkImageDataPtr cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox);
	vtkImageDataPtr toGrayscale(vtkImageDataPtr input);

	std::vector<int> mReducedToFull; ///< map from indexes in the reduced volume to the full (original) volume.
//	int* mDimensions;
//	Vector3D mSpacing;
	bool mUseAngio;
	IntBoundingBox3D mCropbox;

	QString mFilename;
	vtkImageDataPtr mOptionalWholeBase; ///< handle for original monolithic data if present
	std::vector<vtkImageDataPtr> mBaseImage;
	std::vector<vtkImageDataPtr> mProcessedImage;
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
