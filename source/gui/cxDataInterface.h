/*
 * cxDataInterface.h
 *
 *  Created on: Apr 13, 2010
 *      Author: christiana
 */
#ifndef CXDATAINTERFACE_H_
#define CXDATAINTERFACE_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QString>
#include "sscForwardDeclarations.h"
#include "sscDoubleDataAdapter.h"
#include "sscStringDataAdapter.h"
#include "sscDefinitions.h"
#include "sscRealTimeStreamSource.h"
#include "cxRegistrationManager.h"
#include "cxForwardDeclarations.h"
#include "cxRecordSession.h"

namespace cx
{

/** Interface to the tool offset of the dominant tool
 */
class DoubleDataAdapterActiveToolOffset : public ssc::DoubleDataAdapter
{
  Q_OBJECT
public:
  DoubleDataAdapterActiveToolOffset();
  virtual ~DoubleDataAdapterActiveToolOffset() {}
  virtual QString getValueName() const { return "Offset (mm)"; }
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}
  ssc::DoubleRange getValueRange() const;

private slots:
  void dominantToolChangedSlot();
protected:
  ssc::ToolPtr mTool;
};

/** Superclass for all doubles interacting with the active image.
 */
class DoubleDataAdapterActiveImageBase : public ssc::DoubleDataAdapter
{
  Q_OBJECT
public:
  DoubleDataAdapterActiveImageBase();
  virtual ~DoubleDataAdapterActiveImageBase() {}
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}
private slots:
  void activeImageChanged();
protected:
  virtual double getValueInternal() const = 0;
  virtual void setValueInternal(double val) = 0;

  ssc::ImagePtr mImage;
};

/**DataInterface implementation for the LUT 2D window value
 */
class DoubleDataAdapter2DWindow : public DoubleDataAdapterActiveImageBase
{
public:
  virtual ~DoubleDataAdapter2DWindow() {}
  virtual QString getValueName() const { return "Window"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual ssc::DoubleRange getValueRange() const;
};

/**DataInterface implementation for the LUT 2D level value
 */
class DoubleDataAdapter2DLevel : public DoubleDataAdapterActiveImageBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapter2DLevel() {}
  virtual QString getValueName() const { return "Level"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual ssc::DoubleRange getValueRange() const;
};

/** Base class for all DataAdapters that selects an image.
 */
class SelectImageStringDataAdapterBase : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  SelectImageStringDataAdapterBase();
  virtual ~SelectImageStringDataAdapterBase() {}

public: // basic methods

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};
typedef boost::shared_ptr<class SelectImageStringDataAdapterBase> SelectImageStringDataAdapterBasePtr;

/** Base class for all DataAdapters that selects a real time source.
 */
class SelectRTSourceStringDataAdapterBase : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  SelectRTSourceStringDataAdapterBase();
  virtual ~SelectRTSourceStringDataAdapterBase() {}

public: // basic methods

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};
typedef boost::shared_ptr<class SelectRTSourceStringDataAdapterBase> SelectRTSourceStringDataAdapterBasePtr;

/** Base class for all DataAdapters that selects a data.
 */
class SelectDataStringDataAdapterBase : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  SelectDataStringDataAdapterBase();
  virtual ~SelectDataStringDataAdapterBase() {}

public: // basic methods

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};
typedef boost::shared_ptr<class SelectDataStringDataAdapterBase> SelectDataStringDataAdapterBasePtr;

/** Base class for all DataAdapters that selects a tool.
 */
class SelectToolStringDataAdapterBase : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  SelectToolStringDataAdapterBase();
  virtual ~SelectToolStringDataAdapterBase() {}

public: // basic methods

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};
typedef boost::shared_ptr<class SelectToolStringDataAdapterBase> SelectToolStringDataAdapterBasePtr;

/** Base class for all DataAdapters that selects a record session.
 */
class SelectRecordSessionStringDataAdapterBase : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  SelectRecordSessionStringDataAdapterBase();
  virtual ~SelectRecordSessionStringDataAdapterBase() {}

public: // basic methods

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};
typedef boost::shared_ptr<class SelectRecordSessionStringDataAdapterBase> SelectRecordSessionStringDataAdapterBasePtr;

/** Base class for all DataAdapters that selects a coordinatesystem.
 */
class SelectCoordinateSystemStringDataAdapterBase : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  SelectCoordinateSystemStringDataAdapterBase();
  virtual ~SelectCoordinateSystemStringDataAdapterBase() {}

public: // basic methods

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};
typedef boost::shared_ptr<class SelectCoordinateSystemStringDataAdapterBase> SelectCoordinateSystemStringDataAdapterBasePtr;

/** Adapter that connects to the current active image.
 * Example: Active image: [DataName]
 * where active image is the value
 * and DataName is taken from the valuerange
 */
class ActiveImageStringDataAdapter : public SelectImageStringDataAdapterBase
{
  Q_OBJECT
public:
  static ssc::StringDataAdapterPtr New() { return ssc::StringDataAdapterPtr(new ActiveImageStringDataAdapter()); }
  ActiveImageStringDataAdapter();
  virtual ~ActiveImageStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
};

/** Adapter that connects to the fixed image in the registration manager.
 */
class RegistrationFixedImageStringDataAdapter : public SelectImageStringDataAdapterBase
{
  Q_OBJECT
public:
  static ssc::StringDataAdapterPtr New() { return ssc::StringDataAdapterPtr(new RegistrationFixedImageStringDataAdapter()); }
  RegistrationFixedImageStringDataAdapter();
  virtual ~RegistrationFixedImageStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
};

/** Adapter that connects to the fixed image in the registration manager.
 */
class RegistrationMovingImageStringDataAdapter : public SelectImageStringDataAdapterBase
{
  Q_OBJECT
public:
  static ssc::StringDataAdapterPtr New() { return ssc::StringDataAdapterPtr(new RegistrationMovingImageStringDataAdapter()); }
  RegistrationMovingImageStringDataAdapter();
  virtual ~RegistrationMovingImageStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
};

typedef boost::shared_ptr<class SelectImageStringDataAdapter> SelectImageStringDataAdapterPtr;
/** Adapter that selects and stores an image.
 * The image is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 */
class SelectImageStringDataAdapter : public SelectImageStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectImageStringDataAdapterPtr New() { return SelectImageStringDataAdapterPtr(new SelectImageStringDataAdapter()); }
  SelectImageStringDataAdapter();
  virtual ~SelectImageStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  ssc::ImagePtr getImage();
  void setValueName(const QString name);

signals:
    void imageChanged(QString);

private:
  QString mImageUid;
  QString mValueName;
};

typedef boost::shared_ptr<class SelectRTSourceStringDataAdapter> SelectRTSourceStringDataAdapterPtr;
/** Adapter that selects and stores an rtsource.
 * The rtsource is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 */
class SelectRTSourceStringDataAdapter : public SelectRTSourceStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectRTSourceStringDataAdapterPtr New() { return SelectRTSourceStringDataAdapterPtr(new SelectRTSourceStringDataAdapter()); }
  SelectRTSourceStringDataAdapter();
  virtual ~SelectRTSourceStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  ssc::RealTimeStreamSourcePtr getRTSource();
  void setValueName(const QString name);

signals:
    void rtSourceChanged();

private slots:
  void setDefaultSlot();

private:
  QString mRTSourceUid;
  QString mValueName;
};

typedef boost::shared_ptr<class SelectCoordinateSystemStringDataAdapter> SelectCoordinateSystemStringDataAdapterPtr;
/** Adapter that selects and stores a coordinate systems.
 * The coordinatesystem is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 *
 * Class reacts to toolmanagers configurerd signal and automatically sets patientref as default
 */
class SelectCoordinateSystemStringDataAdapter : public SelectCoordinateSystemStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectCoordinateSystemStringDataAdapterPtr New() { return SelectCoordinateSystemStringDataAdapterPtr(new SelectCoordinateSystemStringDataAdapter()); }
  SelectCoordinateSystemStringDataAdapter();
  virtual ~SelectCoordinateSystemStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  ssc::COORDINATE_SYSTEM getCoordinateSystem();

private slots: //interface extension
  void setDefaultSlot();

private:
    ssc::COORDINATE_SYSTEM mCoordinateSystem;
  QString mValueName;
};

typedef boost::shared_ptr<class SelectToolStringDataAdapter> SelectToolStringDataAdapterPtr;
/** Adapter that selects and stores a tool.
 * The tool is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 *
 */
class SelectToolStringDataAdapter : public SelectToolStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectToolStringDataAdapterPtr New() { return SelectToolStringDataAdapterPtr(new SelectToolStringDataAdapter()); }
  SelectToolStringDataAdapter();
  virtual ~SelectToolStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  ssc::ToolPtr getTool() const;

private:
  ssc::ToolPtr mTool;
};

typedef boost::shared_ptr<class SelectRecordSessionStringDataAdapter> SelectRecordSessionStringDataAdapterPtr;
/** Adapter that selects and stores a tool.
 * The tool is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 *
 */
class SelectRecordSessionStringDataAdapter : public SelectRecordSessionStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectRecordSessionStringDataAdapterPtr New() { return SelectRecordSessionStringDataAdapterPtr(new SelectRecordSessionStringDataAdapter()); }
  SelectRecordSessionStringDataAdapter();
  virtual ~SelectRecordSessionStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: //interface extencion
  RecordSessionPtr getRecordSession();

private slots:
  void setDefaultSlot();

private:
  RecordSessionPtr mRecordSession;
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
  SelectDataStringDataAdapter();
  virtual ~SelectDataStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  ssc::DataPtr getData() const;

private:
  ssc::DataPtr mData;
};

typedef boost::shared_ptr<class SelectMeshStringDataAdapter> SelectMeshStringDataAdapterPtr;
/** Adapter that selects and stores an mesh.
 * The image is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 */
class SelectMeshStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static SelectMeshStringDataAdapterPtr New() { return SelectMeshStringDataAdapterPtr(new SelectMeshStringDataAdapter()); }
  SelectMeshStringDataAdapter();
  virtual ~SelectMeshStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

public: // interface extension
  ssc::MeshPtr getMesh();
  void setValueName(const QString name);

signals:
    void meshChanged(QString);

private:
  QString mMeshUid;
  QString mValueName;
};

typedef boost::shared_ptr<class ParentFrameStringDataAdapter> ParentFrameStringDataAdapterPtr;
/** Adapter that selects the parent frame of the given ssc::Data.
 */
class ParentFrameStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ParentFrameStringDataAdapterPtr New() { return ParentFrameStringDataAdapterPtr(new ParentFrameStringDataAdapter()); }
  ParentFrameStringDataAdapter();
  void setData(ssc::DataPtr data);
  virtual ~ParentFrameStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

private:
  ssc::DataPtr mData;
};

typedef boost::shared_ptr<class DataNameEditableStringDataAdapter> DataNameEditableStringDataAdapterPtr;
/** Adapter for displaying and changing name of a ssc::Data.
 */
class DataNameEditableStringDataAdapter : public ssc::EditableStringDataAdapter
{
  Q_OBJECT
public:
  static DataNameEditableStringDataAdapterPtr New() { return DataNameEditableStringDataAdapterPtr(new DataNameEditableStringDataAdapter()); }
  DataNameEditableStringDataAdapter();
  virtual void setData(ssc::DataPtr data);
  virtual ~DataNameEditableStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

private:
  ssc::DataPtr mData;
};

typedef boost::shared_ptr<class DataUidEditableStringDataAdapter> DataUidEditableStringDataAdapterPtr;
/** Adapter for displaying and changing name of a ssc::Data.
 */
class DataUidEditableStringDataAdapter : public ssc::EditableStringDataAdapter
{
  Q_OBJECT
public:
  static DataUidEditableStringDataAdapterPtr New() { return DataUidEditableStringDataAdapterPtr(new DataUidEditableStringDataAdapter()); }
  DataUidEditableStringDataAdapter();
  virtual void setData(ssc::DataPtr data);
  virtual ~DataUidEditableStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;
  virtual bool isReadOnly() const { return true; }

private:
  ssc::DataPtr mData;
};

} // namespace cx

#endif /* CXDATAINTERFACE_H_ */
