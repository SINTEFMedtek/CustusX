/*
 * sscDICOMLibAPI.h
 *
 *  Created on: Nov 8, 2011
 *      Author: christiana
 */

#ifndef SSCDICOMLIBAPI_H_
#define SSCDICOMLIBAPI_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>
#include <QThread>
#include <QMutex>
#include <QString>
#include "sscForwardDeclarations.h"
#include "sscSNW2Volume.h"

struct series_t;
struct study_t;

namespace ssc
{

typedef boost::shared_ptr<class Series> SeriesPtr;
typedef boost::shared_ptr<class SNW2VolumeMetaData> SNW2VolumeMetaDataPtr;
typedef boost::shared_ptr<class Study> StudyPtr;
typedef boost::weak_ptr<class Study> StudyWeakPtr;

/** Interface to a DICOM series.
 *
 */
class Series
{
public:
	Series(series_t* base, StudyWeakPtr parent);

	ssc::SNW2VolumeMetaDataPtr getMetaData();
	QString getUid() const { return mUid; } ///< get the uid that the image will get when created.
	ssc::ImagePtr getImage(); ///< load entire volume: expensive first call
	StudyPtr getParentStudy();

private:
	struct series_t* mBase;
	ssc::ImagePtr mImage;
	ssc::SNW2VolumeMetaDataPtr mMetaData;
	QString mUid;
	StudyWeakPtr mParent;

	bool rawLoadVtkImageData(vtkImageDataPtr mImageData, ssc::SNW2VolumeMetaData metaData, const char* rawVolume, int rescaleIntercept);
	bool rawLoadMetaData(ssc::SNW2VolumeMetaData &data) const;

};

struct StudyData
{
	QString mPatientName;
	QString mPatientID;
	QDate mPatientBirthDate;
	QString mPatientSex;
	QDateTime mStudyDate;
	QString mStudyID;
	QString mStudyDescription;

	void put(std::ostream& s) const;
};



/** Interface to a DICOM study.
 *
 */
class Study
{
public:
	static StudyPtr New(study_t *base);
	std::vector<SeriesPtr> getSeries();
	StudyData getData();

private:
	Study(study_t *base);
	void buildSeries();
	study_t *mBase; ///< C implementation
	std::vector<SeriesPtr> mSeries;
	StudyWeakPtr mSelf;
};


typedef boost::shared_ptr<class DICOMLibAPI> DICOMLibAPIPtr;

/** Private class - used internally by DICOMLibAPI
 *
 */
class RefreshThread : public QThread
{
	Q_OBJECT
public:
	RefreshThread(QObject* parent, DICOMLibAPI* base);
	virtual ~RefreshThread();
	void progressFunc(int value);
signals:
	void progress(int val);
protected:
	void run();
	DICOMLibAPI* mBase;
private:
};

/** C++ Interface to DICOMLib
 *
 * Select a single root folder, then retrieve the
 * studies located within the folder tree.
 *
 */
class DICOMLibAPI : public QObject
{
	Q_OBJECT
public:
	static DICOMLibAPIPtr New();
	DICOMLibAPI();
	virtual ~DICOMLibAPI();

	void setRootFolder(QString root); ///< set a root, look for dcm recursively in that folder tree.
	QString getRootFolder() const;
	void refresh(); ///< reread the root folder to check for new files.

	std::vector<StudyPtr> getStudies(); ///< get a list of all available studies.

signals:
	void changed();
	void refreshStarted();
	void refreshProgress(int val);
	void refreshFinished();
private slots:
	void refreshFinishedSlot();
private:
	void buildStudies();

	friend class RefreshThread;
	mutable QMutex mMutex; ///< protects mRootFolder and mData, NOT the contents of mData,
	QString mRootFolder;
	study_t *mData; ///< pointer to C-linked list implementation

	std::vector<StudyPtr> mStudies;
	class RefreshThread* mRefreshThread;
};

} // namespace ssc

#endif /* SSCDICOMLIBAPI_H_ */
