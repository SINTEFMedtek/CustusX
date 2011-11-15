/*
 * sscDICOMLibAPI.h
 *
 *  Created on: Nov 8, 2011
 *      Author: christiana
 */

#ifndef SSCDICOMLIBAPI_H_
#define SSCDICOMLIBAPI_H_

#include <boost/shared_ptr.hpp>
#include <vector>
#include <QString>
#include "sscForwardDeclarations.h"
#include "sscSNW2Volume.h"

typedef boost::shared_ptr<class Series> SeriesPtr;

/** Interface to a DICOM series.
 *
 */
class Series
{
public:
	Series(struct series_t* base);

	ssc::ImagePtr getImage();

private:
	struct series_t* mBase;

	bool rawLoadVtkImageData(vtkImageDataPtr mImageData, ssc::SNW2VolumeMetaData metaData, const char* rawVolume);
	bool rawLoadMetaData(ssc::SNW2VolumeMetaData &data) const;

};


typedef boost::shared_ptr<class Study> StudyPtr;

/** Interface to a DICOM study.
 *
 */
class Study
{
public:
	Study(struct study_t *base);
	std::vector<SeriesPtr> getSeries();

private:
	struct study_t *mBase; ///< C implementation
};

typedef boost::shared_ptr<class DICOMLibAPI> DICOMLibAPIPtr;


/** C++ Interface to DICOMLib
 *
 * Select a single root folder, then retrieve the
 * studies located within the folder tree.
 *
 */
class DICOMLibAPI
{
public:
	static DICOMLibAPIPtr New();
	DICOMLibAPI();
	virtual ~DICOMLibAPI();

	void setRootFolder(QString root); ///< set a root, look for dcm recursively in that folder tree.
	void refresh(); ///< reread the root folder to check for new files.

	std::vector<StudyPtr> getStudies(); ///< get a list of all available studies.

private:
	QString mRootFolder;
	struct study_t *mData; ///< pointer to C-linked list implementation
};

#endif /* SSCDICOMLIBAPI_H_ */
