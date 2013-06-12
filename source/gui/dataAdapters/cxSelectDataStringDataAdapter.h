// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXSELECTDATASTRINGDATAADAPTER_H
#define CXSELECTDATASTRINGDATAADAPTER_H

#include "sscStringDataAdapter.h"
#include "cxForwardDeclarations.h"

namespace cx
{

/** Base class for all DataAdapters that selects a ssc::Data or descendants.
 */
class SelectDataStringDataAdapterBase : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  virtual ~SelectDataStringDataAdapterBase() {}

public: // basic methods
	virtual QString getValueName() const;

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
  virtual QString getHelp() const;

public: // interface extension
  virtual ssc::DataPtr getData() const;
  virtual void setValueName(const QString name);
  virtual void setHelp(QString text);

signals:
  void dataChanged(QString);
protected:
	/** Construct base with a filter that determined allowed ssc::Data types based
	  * on their getType() return value. The default of ".*" means any type.
	  */
	explicit SelectDataStringDataAdapterBase(QString typeRegexp = ".*");
	std::map<QString, ssc::DataPtr> filterOnType(std::map<QString, ssc::DataPtr> input, QString regexp) const;
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
  ssc::ImagePtr getImage();

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
  virtual ssc::DataPtr getData() const;

protected:
  SelectDataStringDataAdapter();
private:
//  ssc::DataPtr mData;
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
  ssc::MeshPtr getMesh();

protected:
  SelectMeshStringDataAdapter();
private:
  QString mMeshUid;
};


} // namespace cx

#endif // CXSELECTDATASTRINGDATAADAPTER_H
