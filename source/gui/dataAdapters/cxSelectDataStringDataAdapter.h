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
#ifndef CXSELECTDATASTRINGDATAADAPTER_H
#define CXSELECTDATASTRINGDATAADAPTER_H

#include "cxStringDataAdapter.h"
#include "cxForwardDeclarations.h"
#include "cxLegacySingletons.h"

namespace cx
{

/** Base class for all DataAdapters that selects a Data or descendants.
 */
class SelectDataStringDataAdapterBase : public StringDataAdapter
{
  Q_OBJECT
public:
  virtual ~SelectDataStringDataAdapterBase() {}

public: // basic methods
	virtual QString getDisplayName() const;

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
  virtual QString getHelp() const;

public: // interface extension
  virtual DataPtr getData() const;
  virtual void setValueName(const QString name);
  virtual void setHelp(QString text);

signals:
  void dataChanged(QString);
protected:
	/** Construct base with a filter that determined allowed Data types based
	  * on their getType() return value. The default of ".*" means any type.
	  */
	explicit SelectDataStringDataAdapterBase(QString typeRegexp = ".*");
	std::map<QString, DataPtr> filterOnType(std::map<QString, DataPtr> input, QString regexp) const;
	QString mTypeRegexp;
	QString mValueName;
	QString mHelp;

};
typedef boost::shared_ptr<class SelectDataStringDataAdapterBase> SelectDataStringDataAdapterBasePtr;

typedef boost::shared_ptr<class ActiveImageStringDataAdapter> ActiveImageStringDataAdapterPtr;
/** Adapter that connects to the current active image.
 * Example: Active image: [DataName]
 * where active image is the value
 * and DataName is taken from the valuerange
 */
class ActiveImageStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
  static ActiveImageStringDataAdapterPtr New() { return ActiveImageStringDataAdapterPtr(new ActiveImageStringDataAdapter()); }
  ActiveImageStringDataAdapter();
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
class SelectImageStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectImageStringDataAdapterPtr New() { return SelectImageStringDataAdapterPtr(new SelectImageStringDataAdapter()); }
  virtual ~SelectImageStringDataAdapter() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  ImagePtr getImage();

protected:
  SelectImageStringDataAdapter();
private:
  QString mImageUid;
};

typedef boost::shared_ptr<class SelectDataStringDataAdapter> SelectDataStringDataAdapterPtr;
/** Adapter that selects and stores a data.
 * The data is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 */
class SelectDataStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectDataStringDataAdapterPtr New() { return SelectDataStringDataAdapterPtr(new SelectDataStringDataAdapter()); }
  virtual ~SelectDataStringDataAdapter() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  virtual DataPtr getData() const;

protected:
  SelectDataStringDataAdapter();
private:
//  DataPtr mData;
  QString mUid;

};

typedef boost::shared_ptr<class SelectMeshStringDataAdapter> SelectMeshStringDataAdapterPtr;
/** Adapter that selects and stores an mesh.
 * The image is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 */
class SelectMeshStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectMeshStringDataAdapterPtr New() { return SelectMeshStringDataAdapterPtr(new SelectMeshStringDataAdapter()); }
  virtual ~SelectMeshStringDataAdapter() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  MeshPtr getMesh();

protected:
  SelectMeshStringDataAdapter();
private:
  QString mMeshUid;
};


} // namespace cx

#endif // CXSELECTDATASTRINGDATAADAPTER_H
