/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXUSFRAMEDATA_H_
#define CXUSFRAMEDATA_H_

#include "cxResourceExport.h"

#include "cxProbeSector.h"
#include "cxUSReconstructInputData.h"
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;

namespace cx
{
typedef boost::shared_ptr<class ImageDataContainer> ImageDataContainerPtr;
typedef boost::shared_ptr<class CachedImageDataContainer> CachedImageDataContainerPtr;
}

namespace cx
{


/**
 * \addtogroup cx_resource_usreconstructiontypes
 * \{
 */

typedef boost::shared_ptr<class USFrameData> USFrameDataPtr;
//typedef boost::shared_ptr<class TimedPosition> TimedPositionPtr;

/** Output from the reconstruct preprocessing and is input to the reconstruction.
  *
  * Interface is thread-safe.
  */
class cxResource_EXPORT ProcessedUSInputData
{
public:
	ProcessedUSInputData(std::vector<vtkImageDataPtr> frames, std::vector<TimedPosition> pos, vtkImageDataPtr mask, QString path, QString uid);

	unsigned char* getFrame(unsigned int index) const;
	Eigen::Array3i getDimensions() const;
	Vector3D getSpacing() const;
	std::vector<TimedPosition> getFrames() const;
	vtkImageDataPtr getMask();

	QString getFilePath();
	QString getUid();
	bool validate() const;

private:
	std::vector<vtkImageDataPtr> mProcessedImage;
	std::vector<TimedPosition> mFrames;
	vtkImageDataPtr mMask;///< Clipping mask for the input data
	QString mPath;
	QString mUid;
};
typedef boost::shared_ptr<ProcessedUSInputData> ProcessedUSInputDataPtr;


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
class cxResource_EXPORT USFrameData
{
public:
	static USFrameDataPtr create(ImagePtr inputFrameData);
	static USFrameDataPtr create(QString inputFilename, FileManagerServicePtr fileManager);
	static USFrameDataPtr create(QString name, cx::ImageDataContainerPtr images);
	static USFrameDataPtr create(QString name, std::vector<vtkImageDataPtr> frames);
	~USFrameData();

	Eigen::Array3i getDimensions() const;
	Vector3D getSpacing() const;
	QString getName() const;
	cx::ImageDataContainerPtr getImageContainer() { return mImageContainer; }
	unsigned getNumImages();

	void resetRemovedFrames();
	void removeFrame(unsigned int index);
	void setCropBox(IntBoundingBox3D mCropbox);
	void fillImageImport(vtkImageImportPtr import, int index); ///< fill import with a single frame
	void setPurgeInputDataAfterInitialize(bool value);

	/** Use the input raw data and control parameters to generate filtered frames.
	  * The input angio controls how many output objects that will be created, and if each
	  * of them should be angio or grayscale.
	  */
	std::vector<std::vector<vtkImageDataPtr> > initializeFrames(std::vector<bool> angio);

	virtual USFrameDataPtr copy();
	void purgeAll();

	bool is4D();//Do this object contain 4D data? (Opposed to the usual 3D data)
	bool is8bit() const;

protected:
	USFrameData();
	vtkImageDataPtr useAngio(vtkImageDataPtr inData, vtkImageDataPtr grayFrame, int frameNum) const;/// Use only US angio data as input. Removes grayscale from the US data and converts the remaining color to grayscale

	vtkImageDataPtr cropImageExtent(vtkImageDataPtr input, IntBoundingBox3D cropbox) const;
	vtkImageDataPtr to8bitGrayscaleAndEffectuateCropping(vtkImageDataPtr input) const;

	std::vector<int> mReducedToFull; ///< map from indexes in the reduced volume to the full (original) volume.
	IntBoundingBox3D mCropbox;

	QString mName;
	cx::ImageDataContainerPtr mImageContainer;
	bool mPurgeInput;
private:
	vtkImageDataPtr convertTo8bit(vtkImageDataPtr input) const;
};

/**
 * \}
 */

}//namespace cx

#endif // CXUSFRAMEDATA_H_
