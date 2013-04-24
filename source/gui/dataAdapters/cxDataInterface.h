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
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

#ifndef CXDATAINTERFACE_H_
#define CXDATAINTERFACE_H_

#include "sscDoubleDataAdapter.h"
#include "sscStringDataAdapter.h"
#include "cxForwardDeclarations.h"
#include "sscCoordinateSystemHelpers.h"

namespace cx
{
typedef boost::shared_ptr<class ActiveImageProxy> ActiveImageProxyPtr;

/**
 * \brief Interface to the tool offset of the dominant tool
 */
class DoubleDataAdapterActiveToolOffset : public ssc::DoubleDataAdapter
{
  Q_OBJECT
public:
	static boost::shared_ptr<DoubleDataAdapterActiveToolOffset> create()
	{
		return boost::shared_ptr<DoubleDataAdapterActiveToolOffset>(new DoubleDataAdapterActiveToolOffset());
	}

  DoubleDataAdapterActiveToolOffset();
  virtual ~DoubleDataAdapterActiveToolOffset() {}
  virtual QString getValueName() const { return "Offset"; }
  virtual QString getHelp() const
  {
	  return "Tool tip virtual offset, in mm.";
  }
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}
  ssc::DoubleRange getValueRange() const;

private slots:
  void dominantToolChangedSlot();
protected:
  ssc::ToolPtr mTool;
};

/**
 * \brief Superclass for all doubles interacting with the active image.
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

/**
 * \brief DataInterface implementation for the LUT 2D window value
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

/**
 * \brief DataInterface implementation for the LUT 2D level value
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

/**
 * \brief Base class for all DataAdapters that selects a real time source.
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

/**
 * \brief Base class for all DataAdapters that selects a tool.
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


typedef boost::shared_ptr<class SelectRTSourceStringDataAdapter> SelectRTSourceStringDataAdapterPtr;

/**
 * \brief Adapter that selects and stores an rtsource.
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

typedef boost::shared_ptr<class ActiveVideoSourceStringDataAdapter> ActiveVideoSourceStringDataAdapterPtr;
/**
 * \brief Adapter for controlling the active video source in cx::VideoService
 */
class ActiveVideoSourceStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ActiveVideoSourceStringDataAdapterPtr New() { return ActiveVideoSourceStringDataAdapterPtr(new ActiveVideoSourceStringDataAdapter()); }
  ActiveVideoSourceStringDataAdapter();
  virtual ~ActiveVideoSourceStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;
  virtual QStringList getValueRange() const;
  virtual QString getHelp() const;
};

typedef boost::shared_ptr<class SelectCoordinateSystemStringDataAdapter> SelectCoordinateSystemStringDataAdapterPtr;

/**
 * \brief Adapter that selects and stores a coordinate systems.
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

/**
 * \brief Adapter that selects and stores a tool.
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


typedef boost::shared_ptr<class ParentFrameStringDataAdapter> ParentFrameStringDataAdapterPtr;

/**
 * \brief Adapter that selects the parent frame of the given ssc::Data.
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

/**
 * \brief Adapter that force sets the parent frame of the given ssc::Data,
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

/**
 * \brief Adapter for displaying and changing name of a ssc::Data.
 */
class DataNameEditableStringDataAdapter : public ssc::StringDataAdapter
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

	virtual bool isReadOnly() const { return false; }
	virtual bool getAllowOnlyValuesInRange() const { return false; }


private:
  ssc::DataPtr mData;
};

typedef boost::shared_ptr<class DataUidEditableStringDataAdapter> DataUidEditableStringDataAdapterPtr;

/**
 * \brief Adapter for displaying and changing name of a ssc::Data.
 */
class DataUidEditableStringDataAdapter : public ssc::StringDataAdapter
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
	virtual bool getAllowOnlyValuesInRange() const { return false; }

private:
  ssc::DataPtr mData;
};
typedef boost::shared_ptr<class DataModalityStringDataAdapter> DataModalityStringDataAdapterPtr;

/**
 * \brief Adapter that selects the modality of the given ssc::Data.
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

/**
 * \brief Adapter that selects the image type of the given ssc::Data.
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
