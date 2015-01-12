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
#ifndef CXDATAINTERFACE_H_
#define CXDATAINTERFACE_H_

#include "cxGuiExport.h"

#include "cxDoublePropertyBase.h"
#include "cxStringPropertyBase.h"
#include "cxForwardDeclarations.h"
#include "cxCoordinateSystemHelpers.h"

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

namespace cx
{
typedef boost::shared_ptr<class ActiveImageProxy> ActiveImageProxyPtr;
typedef boost::shared_ptr<class DominantToolProxy> DominantToolProxyPtr;

/**
 * \brief Interface to the tool offset of the dominant tool
 */
class cxGui_EXPORT DoubleDataAdapterActiveToolOffset : public DoubleDataAdapter
{
  Q_OBJECT
public:
	static boost::shared_ptr<DoubleDataAdapterActiveToolOffset> create()
	{
		return boost::shared_ptr<DoubleDataAdapterActiveToolOffset>(new DoubleDataAdapterActiveToolOffset());
	}

  DoubleDataAdapterActiveToolOffset();
  virtual ~DoubleDataAdapterActiveToolOffset() {}
  virtual QString getDisplayName() const { return "Offset"; }
  virtual QString getHelp() const
  {
	  return "Tool tip virtual offset, in mm.";
  }
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}
  DoubleRange getValueRange() const;

protected:
  DominantToolProxyPtr mActiveTool;
};

/**
 * \brief Superclass for all doubles interacting with the active image.
 */
class cxGui_EXPORT DoubleDataAdapterActiveImageBase : public DoubleDataAdapter
{
  Q_OBJECT
public:
  DoubleDataAdapterActiveImageBase(PatientModelServicePtr patientModelService);
  virtual ~DoubleDataAdapterActiveImageBase() {}
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}
private slots:
  void activeImageChanged();
protected:
  virtual double getValueInternal() const = 0;
  virtual void setValueInternal(double val) = 0;

  ImagePtr mImage;
  ActiveImageProxyPtr mActiveImageProxy;
  PatientModelServicePtr mPatientModelService;
};

/**
 * \brief DataInterface implementation for the LUT 2D window value
 */
class cxGui_EXPORT DoubleDataAdapter2DWindow : public DoubleDataAdapterActiveImageBase
{
public:
  virtual ~DoubleDataAdapter2DWindow() {}
  virtual QString getDisplayName() const { return "Window"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**
 * \brief DataInterface implementation for the LUT 2D level value
 */
class cxGui_EXPORT DoubleDataAdapter2DLevel : public DoubleDataAdapterActiveImageBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapter2DLevel() {}
  virtual QString getDisplayName() const { return "Level"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**
 * \brief Base class for all DataAdapters that selects a real time source.
 */
class cxGui_EXPORT SelectRTSourceStringDataAdapterBase : public StringDataAdapter
{
  Q_OBJECT
public:
  SelectRTSourceStringDataAdapterBase(PatientModelServicePtr patientModelService);
  virtual ~SelectRTSourceStringDataAdapterBase();

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

protected:
  PatientModelServicePtr mPatientModelService;
};
typedef boost::shared_ptr<class SelectRTSourceStringDataAdapterBase> SelectRTSourceStringDataAdapterBasePtr;

/**
 * \brief Base class for all DataAdapters that selects a tool.
 */
class cxGui_EXPORT SelectToolStringDataAdapterBase : public StringDataAdapter
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
class cxGui_EXPORT SelectCoordinateSystemStringDataAdapterBase : public StringDataAdapter
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
class cxGui_EXPORT SelectRTSourceStringDataAdapter : public SelectRTSourceStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectRTSourceStringDataAdapterPtr New(PatientModelServicePtr patientModelService) { return SelectRTSourceStringDataAdapterPtr(new SelectRTSourceStringDataAdapter(patientModelService)); }
  SelectRTSourceStringDataAdapter(PatientModelServicePtr patientModelService);
  virtual ~SelectRTSourceStringDataAdapter() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  VideoSourcePtr getRTSource();
  void setValueName(const QString name);

private slots:
  void setDefaultSlot();

private:
  VideoSourcePtr mRTSource;
  QString mValueName;
};

typedef boost::shared_ptr<class ActiveVideoSourceStringDataAdapter> ActiveVideoSourceStringDataAdapterPtr;
/**
 * \brief Adapter for controlling the active video source in cx::VideoServiceOld
 */
class cxGui_EXPORT ActiveVideoSourceStringDataAdapter : public StringDataAdapter
{
  Q_OBJECT
public:
  static ActiveVideoSourceStringDataAdapterPtr New() { return ActiveVideoSourceStringDataAdapterPtr(new ActiveVideoSourceStringDataAdapter()); }
  ActiveVideoSourceStringDataAdapter();
  virtual ~ActiveVideoSourceStringDataAdapter() {}

public: // basic methods
  virtual QString getDisplayName() const;
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
class cxGui_EXPORT SelectCoordinateSystemStringDataAdapter : public SelectCoordinateSystemStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectCoordinateSystemStringDataAdapterPtr New() { return SelectCoordinateSystemStringDataAdapterPtr(new SelectCoordinateSystemStringDataAdapter()); }
  SelectCoordinateSystemStringDataAdapter();
  virtual ~SelectCoordinateSystemStringDataAdapter() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  COORDINATE_SYSTEM getCoordinateSystem();

private slots: //interface extension
  void setDefaultSlot();

private:
  COORDINATE_SYSTEM mCoordinateSystem;
  QString mValueName;
};

typedef boost::shared_ptr<class SelectToolStringDataAdapter> SelectToolStringDataAdapterPtr;

/**
 * \brief Adapter that selects and stores a tool.
 * The tool is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 *
 */
class cxGui_EXPORT SelectToolStringDataAdapter : public SelectToolStringDataAdapterBase
{
  Q_OBJECT
public:
  static SelectToolStringDataAdapterPtr New() { return SelectToolStringDataAdapterPtr(new SelectToolStringDataAdapter()); }
  SelectToolStringDataAdapter();
  virtual ~SelectToolStringDataAdapter() {}

  void setHelp(QString help);
  void setValueName(QString name);

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  ToolPtr getTool() const;

private:
  QString mValueName;
  QString mHelp;
  ToolPtr mTool;
};


typedef boost::shared_ptr<class ParentFrameStringDataAdapter> ParentFrameStringDataAdapterPtr;

/**
 * \brief Adapter that selects the parent frame of the given Data.
 */
class cxGui_EXPORT ParentFrameStringDataAdapter : public StringDataAdapter
{
  Q_OBJECT
public:
  static ParentFrameStringDataAdapterPtr New(PatientModelServicePtr patientModelService) { return ParentFrameStringDataAdapterPtr(new ParentFrameStringDataAdapter(patientModelService)); }
  ParentFrameStringDataAdapter(PatientModelServicePtr patientModelService);
  void setData(DataPtr data);
  virtual ~ParentFrameStringDataAdapter();

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

protected:
  DataPtr mData;
  PatientModelServicePtr mPatientModelService;
};

typedef boost::shared_ptr<class SetParentFrameStringDataAdapter> SetParentFrameStringDataAdapterPtr;

/**
 * \brief Adapter that force sets the parent frame of the given Data,
 * i.e. overwriting history. Nice for initial definition
 */
class cxGui_EXPORT SetParentFrameStringDataAdapter : public ParentFrameStringDataAdapter
{
  Q_OBJECT
public:
  static SetParentFrameStringDataAdapterPtr New(PatientModelServicePtr patientModelService) { return SetParentFrameStringDataAdapterPtr(new SetParentFrameStringDataAdapter(patientModelService)); }
  SetParentFrameStringDataAdapter(PatientModelServicePtr patientModelService);
  virtual ~SetParentFrameStringDataAdapter() {}
public: // basic methods
  virtual bool setValue(const QString& value);
};


typedef boost::shared_ptr<class DataNameEditableStringDataAdapter> DataNameEditableStringDataAdapterPtr;

/**
 * \brief Adapter for displaying and changing name of a Data.
 */
class cxGui_EXPORT DataNameEditableStringDataAdapter : public StringDataAdapter
{
  Q_OBJECT
public:
  static DataNameEditableStringDataAdapterPtr New() { return DataNameEditableStringDataAdapterPtr(new DataNameEditableStringDataAdapter()); }
  DataNameEditableStringDataAdapter();
  virtual void setData(DataPtr data);
  virtual ~DataNameEditableStringDataAdapter() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

	virtual bool isReadOnly() const { return false; }
	virtual bool getAllowOnlyValuesInRange() const { return false; }


private:
  DataPtr mData;
};

typedef boost::shared_ptr<class DataUidEditableStringDataAdapter> DataUidEditableStringDataAdapterPtr;

/**
 * \brief Adapter for displaying and changing name of a Data.
 */
class cxGui_EXPORT DataUidEditableStringDataAdapter : public StringDataAdapter
{
  Q_OBJECT
public:
  static DataUidEditableStringDataAdapterPtr New() { return DataUidEditableStringDataAdapterPtr(new DataUidEditableStringDataAdapter()); }
  DataUidEditableStringDataAdapter();
  virtual void setData(DataPtr data);
  virtual ~DataUidEditableStringDataAdapter() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

	virtual bool isReadOnly() const { return true; }
	virtual bool getAllowOnlyValuesInRange() const { return false; }

private:
  DataPtr mData;
};
typedef boost::shared_ptr<class DataModalityStringDataAdapter> DataModalityStringDataAdapterPtr;

/**
 * \brief Adapter that selects the modality of the given Data.
 */
class cxGui_EXPORT DataModalityStringDataAdapter : public StringDataAdapter
{
  Q_OBJECT
public:
  static DataModalityStringDataAdapterPtr New(PatientModelServicePtr patientModelService) { return DataModalityStringDataAdapterPtr(new DataModalityStringDataAdapter(patientModelService)); }
  DataModalityStringDataAdapter(PatientModelServicePtr patientModelService);
  virtual ~DataModalityStringDataAdapter();
  void setData(ImagePtr data);

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;

protected:
  ImagePtr mData;
  PatientModelServicePtr mPatientModelService;
};

typedef boost::shared_ptr<class ImageTypeStringDataAdapter> ImageTypeStringDataAdapterPtr;

/**
 * \brief Adapter that selects the image type of the given Data.
 */
class cxGui_EXPORT ImageTypeStringDataAdapter : public StringDataAdapter
{
  Q_OBJECT
public:
  static ImageTypeStringDataAdapterPtr New(PatientModelServicePtr patientModelService) { return ImageTypeStringDataAdapterPtr(new ImageTypeStringDataAdapter(patientModelService)); }
  ImageTypeStringDataAdapter(PatientModelServicePtr patientModelService);
  void setData(ImagePtr data);
  virtual ~ImageTypeStringDataAdapter();

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;

protected:
  ImagePtr mData;
  PatientModelServicePtr mPatientModelService;
};

} // namespace cx


/**
 * @}
 */

#endif /* CXDATAINTERFACE_H_ */
