/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSTRINGPROPERTYSELECTCOORDINATESYSTEM_H
#define CXSTRINGPROPERTYSELECTCOORDINATESYSTEM_H

#include "cxResourceExport.h"

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

/** Base class for all Properties that selects a coordinatesystem.
 */
class cxResource_EXPORT StringPropertySelectCoordinateSystemBase : public StringPropertyBase
{
  Q_OBJECT
public:
  StringPropertySelectCoordinateSystemBase();
  virtual ~StringPropertySelectCoordinateSystemBase() {}

public: // basic methods

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};
typedef boost::shared_ptr<class StringPropertySelectCoordinateSystemBase> StringPropertySelectCoordinateSystemBasePtr;


typedef boost::shared_ptr<class StringPropertySelectCoordinateSystem> StringPropertySelectCoordinateSystemPtr;

/**
 * \brief Adapter that selects and stores a coordinate systems.
 * The coordinatesystem is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 *
 * Class reacts to toolmanagers configurerd signal and automatically sets patientref as default
 */
class cxResource_EXPORT StringPropertySelectCoordinateSystem : public StringPropertySelectCoordinateSystemBase
{
  Q_OBJECT
public:
  static StringPropertySelectCoordinateSystemPtr New(TrackingServicePtr trackingService) { return StringPropertySelectCoordinateSystemPtr(new StringPropertySelectCoordinateSystem(trackingService)); }
  StringPropertySelectCoordinateSystem(TrackingServicePtr trackingService);
  virtual ~StringPropertySelectCoordinateSystem() {}

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
  TrackingServicePtr mTrackingService;
};


} // namespace cx


/**
 * @}
 */

#endif // CXSTRINGPROPERTYSELECTCOORDINATESYSTEM_H
