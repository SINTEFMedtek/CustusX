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
typedef boost::shared_ptr<class ActiveToolProxy> ActiveToolProxyPtr;

/**
 * \brief Interface to the tool offset of the active tool
 */
class cxGui_EXPORT DoublePropertyActiveToolOffset : public DoublePropertyBase
{
  Q_OBJECT
public:
	static boost::shared_ptr<DoublePropertyActiveToolOffset> create()
	{
		return boost::shared_ptr<DoublePropertyActiveToolOffset>(new DoublePropertyActiveToolOffset());
	}

  DoublePropertyActiveToolOffset();
  virtual ~DoublePropertyActiveToolOffset() {}
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
  ActiveToolProxyPtr mActiveTool;
};

/**
 * \brief Superclass for all doubles interacting with the active image.
 */
class cxGui_EXPORT DoublePropertyActiveImageBase : public DoublePropertyBase
{
  Q_OBJECT
public:
  DoublePropertyActiveImageBase(PatientModelServicePtr patientModelService);
  virtual ~DoublePropertyActiveImageBase() {}
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
class cxGui_EXPORT DoubleProperty2DWindow : public DoublePropertyActiveImageBase
{
public:
  virtual ~DoubleProperty2DWindow() {}
  virtual QString getDisplayName() const { return "Window"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**
 * \brief DataInterface implementation for the LUT 2D level value
 */
class cxGui_EXPORT DoubleProperty2DLevel : public DoublePropertyActiveImageBase
{
  Q_OBJECT
public:
  virtual ~DoubleProperty2DLevel() {}
  virtual QString getDisplayName() const { return "Level"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**
 * \brief Base class for all Properties that selects a real time source.
 */
class cxGui_EXPORT StringPropertySelectRTSourceBase : public StringPropertyBase
{
  Q_OBJECT
public:
  StringPropertySelectRTSourceBase(PatientModelServicePtr patientModelService);
  virtual ~StringPropertySelectRTSourceBase();

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

protected:
  PatientModelServicePtr mPatientModelService;
};
typedef boost::shared_ptr<class StringPropertySelectRTSourceBase> StringPropertySelectRTSourceBasePtr;


typedef boost::shared_ptr<class StringPropertySelectRTSource> StringPropertySelectRTSourcePtr;

/**
 * \brief Adapter that selects and stores an rtsource.
 * The rtsource is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 */
class cxGui_EXPORT StringPropertySelectRTSource : public StringPropertySelectRTSourceBase
{
  Q_OBJECT
public:
  static StringPropertySelectRTSourceBasePtr New(PatientModelServicePtr patientModelService) { return StringPropertySelectRTSourceBasePtr(new StringPropertySelectRTSource(patientModelService)); }
  StringPropertySelectRTSource(PatientModelServicePtr patientModelService);
  virtual ~StringPropertySelectRTSource() {}

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

typedef boost::shared_ptr<class StringPropertyActiveVideoSource> StringPropertyActiveVideoSourcePtr;
/**
 * \brief Adapter for controlling the active video source in cx::VideoServiceOld
 */
class cxGui_EXPORT StringPropertyActiveVideoSource : public StringPropertyBase
{
  Q_OBJECT
public:
  static StringPropertyActiveVideoSourcePtr New() { return StringPropertyActiveVideoSourcePtr(new StringPropertyActiveVideoSource()); }
  StringPropertyActiveVideoSource();
  virtual ~StringPropertyActiveVideoSource() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;
  virtual QStringList getValueRange() const;
  virtual QString getHelp() const;
};


typedef boost::shared_ptr<class StringPropertyParentFrame> StringPropertyParentFramePtr;

/**
 * \brief Adapter that selects the parent frame of the given Data.
 */
class cxGui_EXPORT StringPropertyParentFrame : public StringPropertyBase
{
  Q_OBJECT
public:
  static StringPropertyParentFramePtr New(PatientModelServicePtr patientModelService) { return StringPropertyParentFramePtr(new StringPropertyParentFrame(patientModelService)); }
  StringPropertyParentFrame(PatientModelServicePtr patientModelService);
  void setData(DataPtr data);
  virtual ~StringPropertyParentFrame();

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

typedef boost::shared_ptr<class StringPropertySetParentFrame> StringPropertySetParentFramePtr;

/**
 * \brief Adapter that force sets the parent frame of the given Data,
 * i.e. overwriting history. Nice for initial definition
 */
class cxGui_EXPORT StringPropertySetParentFrame : public StringPropertyParentFrame
{
  Q_OBJECT
public:
  static StringPropertySetParentFramePtr New(PatientModelServicePtr patientModelService) { return StringPropertySetParentFramePtr(new StringPropertySetParentFrame(patientModelService)); }
  StringPropertySetParentFrame(PatientModelServicePtr patientModelService);
  virtual ~StringPropertySetParentFrame() {}
public: // basic methods
  virtual bool setValue(const QString& value);
};


typedef boost::shared_ptr<class StringPropertyDataNameEditable> StringPropertyDataNameEditablePtr;

/**
 * \brief Adapter for displaying and changing name of a Data.
 */
class cxGui_EXPORT StringPropertyDataNameEditable : public StringPropertyBase
{
  Q_OBJECT
public:
  static StringPropertyDataNameEditablePtr New() { return StringPropertyDataNameEditablePtr(new StringPropertyDataNameEditable()); }
  StringPropertyDataNameEditable();
  virtual void setData(DataPtr data);
  virtual ~StringPropertyDataNameEditable() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

	virtual bool isReadOnly() const { return false; }
	virtual bool getAllowOnlyValuesInRange() const { return false; }


private:
  DataPtr mData;
};

typedef boost::shared_ptr<class StringPropertyDataUidEditable> StringPropertyDataUidEditablePtr;

/**
 * \brief Adapter for displaying and changing name of a Data.
 */
class cxGui_EXPORT StringPropertyDataUidEditable : public StringPropertyBase
{
  Q_OBJECT
public:
  static StringPropertyDataUidEditablePtr New() { return StringPropertyDataUidEditablePtr(new StringPropertyDataUidEditable()); }
  StringPropertyDataUidEditable();
  virtual void setData(DataPtr data);
  virtual ~StringPropertyDataUidEditable() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

	virtual bool isReadOnly() const { return true; }
	virtual bool getAllowOnlyValuesInRange() const { return false; }

private:
  DataPtr mData;
};
typedef boost::shared_ptr<class StringPropertyDataModality> StringPropertyDataModalityPtr;

/**
 * \brief Adapter that selects the modality of the given Data.
 */
class cxGui_EXPORT StringPropertyDataModality : public StringPropertyBase
{
  Q_OBJECT
public:
  static StringPropertyDataModalityPtr New(PatientModelServicePtr patientModelService) { return StringPropertyDataModalityPtr(new StringPropertyDataModality(patientModelService)); }
  StringPropertyDataModality(PatientModelServicePtr patientModelService);
  virtual ~StringPropertyDataModality();
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

typedef boost::shared_ptr<class StringPropertyImageType> StringPropertyImageTypePtr;

/**
 * \brief Adapter that selects the image type of the given Data.
 */
class cxGui_EXPORT StringPropertyImageType : public StringPropertyBase
{
  Q_OBJECT
public:
  static StringPropertyImageTypePtr New(PatientModelServicePtr patientModelService) { return StringPropertyImageTypePtr(new StringPropertyImageType(patientModelService)); }
  StringPropertyImageType(PatientModelServicePtr patientModelService);
  void setData(ImagePtr data);
  virtual ~StringPropertyImageType();

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
typedef boost::shared_ptr<class StringPropertyGlyphOrientationArray> StringPropertyGlyphOrientationArrayPtr;

/**
 * \brief Adapter that selects the glyph orientation array.
 */
class cxGui_EXPORT StringPropertyGlyphOrientationArray : public StringPropertyBase
{
  Q_OBJECT
public:
  static StringPropertyGlyphOrientationArrayPtr New(PatientModelServicePtr patientModelService) { return StringPropertyGlyphOrientationArrayPtr(new StringPropertyGlyphOrientationArray(patientModelService)); }
  StringPropertyGlyphOrientationArray(PatientModelServicePtr patientModelService);
  virtual ~StringPropertyGlyphOrientationArray();
  void setData(MeshPtr data);

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;

protected:
  MeshPtr mData;
  PatientModelServicePtr mPatientModelService;
};




} // namespace cx


/**
 * @}
 */

#endif /* CXDATAINTERFACE_H_ */
