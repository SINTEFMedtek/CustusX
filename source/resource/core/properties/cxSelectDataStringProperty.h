/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSELECTDATASTRINGPROPERTY_H_
#define CXSELECTDATASTRINGPROPERTY_H_

#include "cxSelectDataStringPropertyBase.h"

namespace cx
{
typedef boost::shared_ptr<class StringPropertyActiveData> StringPropertyActiveDataPtr;
/** Property that connects to the current active data.
 * Example: Active data: [DataName]
 * where active data is the value
 * and DataName is taken from the valuerange
 */
class cxResource_EXPORT StringPropertyActiveData : public SelectDataStringPropertyBase
{
	Q_OBJECT
public:
	static StringPropertyActiveDataPtr New(PatientModelServicePtr patientModelService, QString typeRegexp = ".*")
	{
		return StringPropertyActiveDataPtr(new StringPropertyActiveData(patientModelService, typeRegexp));
	}
	virtual ~StringPropertyActiveData() {}

public: // basic methods
	virtual bool setValue(const QString& value);
	virtual QString getValue() const;
protected:
	ActiveDataPtr mActiveData;
	StringPropertyActiveData(PatientModelServicePtr patientModelService, QString typeRegexp = ".*");
};

typedef boost::shared_ptr<class StringPropertyActiveImage> StringPropertyActiveImagePtr;
/** Property that connects to the current active image.
 * Example: Active image: [DataName]
 * where active image is the value
 * and DataName is taken from the valuerange
 */
class cxResource_EXPORT StringPropertyActiveImage : public SelectDataStringPropertyBase
{
	Q_OBJECT
public:
	static StringPropertyActiveImagePtr New(PatientModelServicePtr patientModelService)
	{
		return StringPropertyActiveImagePtr(new StringPropertyActiveImage(patientModelService));
	}
	StringPropertyActiveImage(PatientModelServicePtr patientModelService);
	virtual ~StringPropertyActiveImage() {}

public: // basic methods
	virtual bool setValue(const QString& value);
	virtual QString getValue() const;
protected:
	ActiveDataPtr mActiveData;
};


typedef boost::shared_ptr<class StringPropertySelectImage> StringPropertySelectImagePtr;
/** Property that selects and stores an image.
 * The image is stored internally in the property.
 * Use setValue/getValue plus changed() to access it.
 */
class cxResource_EXPORT StringPropertySelectImage : public SelectDataStringPropertyBase
{
  Q_OBJECT
public:
	static StringPropertySelectImagePtr New(PatientModelServicePtr patientModelService) { return StringPropertySelectImagePtr(new StringPropertySelectImage(patientModelService)); }
  virtual ~StringPropertySelectImage() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  ImagePtr getImage();

protected:
	StringPropertySelectImage(PatientModelServicePtr patientModelService);
private:
  QString mImageUid;
};

typedef boost::shared_ptr<class StringPropertySelectData> StringPropertySelectDataPtr;
/** Property that selects and stores a data.
 * The data is stored internally in the property.
 * Use setValue/getValue plus changed() to access it.
 */
class cxResource_EXPORT StringPropertySelectData : public SelectDataStringPropertyBase
{
  Q_OBJECT
public:
	static StringPropertySelectDataPtr New(PatientModelServicePtr patientModelService, QString typeRegexp = ".*")
	{
		return StringPropertySelectDataPtr(new StringPropertySelectData(patientModelService, typeRegexp));
	}
  virtual ~StringPropertySelectData() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  virtual DataPtr getData() const;

protected:
	StringPropertySelectData(PatientModelServicePtr patientModelService, QString typeRegexp = ".*");
private:
//  DataPtr mData;
  QString mUid;

};

typedef boost::shared_ptr<class StringPropertySelectMesh> StringPropertySelectMeshPtr;
/** Property that selects and stores a mesh.
 * The mesh is stored internally in the property.
 * Use setValue/getValue plus changed() to access it.
 */
class cxResource_EXPORT StringPropertySelectMesh : public SelectDataStringPropertyBase
{
  Q_OBJECT
public:
	static StringPropertySelectMeshPtr New(PatientModelServicePtr patientModelService) { return StringPropertySelectMeshPtr(new StringPropertySelectMesh(patientModelService)); }
  virtual ~StringPropertySelectMesh() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  MeshPtr getMesh();

protected:
	StringPropertySelectMesh(PatientModelServicePtr patientModelService);
private:
  QString mMeshUid;
};

typedef boost::shared_ptr<class StringPropertySelectTrackedStream> StringPropertySelectTrackedStreamPtr;
/** Property that selects and stores a TrackedStream.
 * The stream is stored internally in the property.
 * Use setValue/getValue plus changed() to access it.
 */
class cxResource_EXPORT StringPropertySelectTrackedStream : public SelectDataStringPropertyBase
{
  Q_OBJECT
public:
	static StringPropertySelectTrackedStreamPtr New(PatientModelServicePtr patientModelService) { return StringPropertySelectTrackedStreamPtr(new StringPropertySelectTrackedStream(patientModelService)); }
  virtual ~StringPropertySelectTrackedStream() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  TrackedStreamPtr getTrackedStream();

protected:
	StringPropertySelectTrackedStream(PatientModelServicePtr patientModelService);
private:
  QString mStreamUid;
};



} // namespace cx

#endif // CXSELECTDATASTRINGPROPERTY_H_
