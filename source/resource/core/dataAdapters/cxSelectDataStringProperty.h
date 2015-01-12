/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef CXSELECTDATASTRINGPROPERTY_H_
#define CXSELECTDATASTRINGPROPERTY_H_

#include "cxSelectDataStringPropertyBase.h"

namespace cx
{
typedef boost::shared_ptr<class ActiveImageStringDataAdapter> ActiveImageStringDataAdapterPtr;
/** Adapter that connects to the current active image.
 * Example: Active image: [DataName]
 * where active image is the value
 * and DataName is taken from the valuerange
 */
class cxResource_EXPORT ActiveImageStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
	static ActiveImageStringDataAdapterPtr New(PatientModelServicePtr patientModelService) { return ActiveImageStringDataAdapterPtr(new ActiveImageStringDataAdapter(patientModelService)); }
	ActiveImageStringDataAdapter(PatientModelServicePtr patientModelService);
  virtual ~ActiveImageStringDataAdapter() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;
};


typedef boost::shared_ptr<class SelectImageStringDataAdapter> SelectImageStringDataAdapterPtr;
/** Adapter that selects and stores an image.
 * The image is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 */
class cxResource_EXPORT SelectImageStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
	static SelectImageStringDataAdapterPtr New(PatientModelServicePtr patientModelService) { return SelectImageStringDataAdapterPtr(new SelectImageStringDataAdapter(patientModelService)); }
  virtual ~SelectImageStringDataAdapter() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  ImagePtr getImage();

protected:
	SelectImageStringDataAdapter(PatientModelServicePtr patientModelService);
private:
  QString mImageUid;
};

typedef boost::shared_ptr<class SelectDataStringDataAdapter> SelectDataStringDataAdapterPtr;
/** Adapter that selects and stores a data.
 * The data is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 */
class cxResource_EXPORT SelectDataStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
	static SelectDataStringDataAdapterPtr New(PatientModelServicePtr patientModelService) { return SelectDataStringDataAdapterPtr(new SelectDataStringDataAdapter(patientModelService)); }
  virtual ~SelectDataStringDataAdapter() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  virtual DataPtr getData() const;

protected:
	SelectDataStringDataAdapter(PatientModelServicePtr patientModelService);
private:
//  DataPtr mData;
  QString mUid;

};

typedef boost::shared_ptr<class SelectMeshStringDataAdapter> SelectMeshStringDataAdapterPtr;
/** Adapter that selects and stores an mesh.
 * The image is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 */
class cxResource_EXPORT SelectMeshStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
	static SelectMeshStringDataAdapterPtr New(PatientModelServicePtr patientModelService) { return SelectMeshStringDataAdapterPtr(new SelectMeshStringDataAdapter(patientModelService)); }
  virtual ~SelectMeshStringDataAdapter() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  MeshPtr getMesh();

protected:
	SelectMeshStringDataAdapter(PatientModelServicePtr patientModelService);
private:
  QString mMeshUid;
};


} // namespace cx

#endif // CXSELECTDATASTRINGPROPERTY_H_
