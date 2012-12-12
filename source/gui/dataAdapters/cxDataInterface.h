/*
 * cxDataInterface.h
 *
 *  \date Apr 13, 2010
 *      \author christiana
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
#include "sscVideoSource.h"
//#include "cxRegistrationManager.h"
#include "cxForwardDeclarations.h"
//#include "cxRecordSession.h"
#include "cxActiveImageProxy.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

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
  ActiveImageProxyPtr mActiveImageProxy;
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

/** Base class for all DataAdapters that selects a real time source.
 */
class SelectRTSourceStringDataAdapterBase : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  SelectRTSourceStringDataAdapterBase();
  virtual ~SelectRTSourceStringDataAdapterBase() {}

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};
typedef boost::shared_ptr<class SelectRTSourceStringDataAdapterBase> SelectRTSourceStringDataAdapterBasePtr;

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
  ssc::VideoSourcePtr getRTSource();
  void setValueName(const QString name);

private slots:
  void setDefaultSlot();

private:
  ssc::VideoSourcePtr mRTSource;
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

  void setHelp(QString help);
  void setValueName(QString name);

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  ssc::ToolPtr getTool() const;

private:
  QString mValueName;
  QString mHelp;
  ssc::ToolPtr mTool;
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

protected:
  ssc::DataPtr mData;
};

typedef boost::shared_ptr<class SetParentFrameStringDataAdapter> SetParentFrameStringDataAdapterPtr;
/** Adapter that force sets the parent frame of the given ssc::Data,
 * i.e. overwriting history. Nice for initial definition
 */
class SetParentFrameStringDataAdapter : public ParentFrameStringDataAdapter
{
  Q_OBJECT
public:
  static SetParentFrameStringDataAdapterPtr New() { return SetParentFrameStringDataAdapterPtr(new SetParentFrameStringDataAdapter()); }
  virtual ~SetParentFrameStringDataAdapter() {}
public: // basic methods
  virtual bool setValue(const QString& value);
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


typedef boost::shared_ptr<class DataModalityStringDataAdapter> DataModalityStringDataAdapterPtr;
/** Adapter that selects the modality of the given ssc::Data.
 */
class DataModalityStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static DataModalityStringDataAdapterPtr New() { return DataModalityStringDataAdapterPtr(new DataModalityStringDataAdapter()); }
  DataModalityStringDataAdapter();
  void setData(ssc::ImagePtr data);
  virtual ~DataModalityStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;

protected:
  ssc::ImagePtr mData;
};

typedef boost::shared_ptr<class ImageTypeStringDataAdapter> ImageTypeStringDataAdapterPtr;
/** Adapter that selects the image type of the given ssc::Data.
 */
class ImageTypeStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ImageTypeStringDataAdapterPtr New() { return ImageTypeStringDataAdapterPtr(new ImageTypeStringDataAdapter()); }
  ImageTypeStringDataAdapter();
  void setData(ssc::ImagePtr data);
  virtual ~ImageTypeStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;

protected:
  ssc::ImagePtr mData;
};

/**
 * @}
 */
} // namespace cx

#endif /* CXDATAINTERFACE_H_ */
