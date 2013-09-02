// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef COORDINATESYSTEMLISTENER_H_
#define COORDINATESYSTEMLISTENER_H_

#include "sscCoordinateSystemHelpers.h"

namespace ssc {

/**\brief Class that listens to changes in a coordinate system,
 * and emits a signal if that system changes.
 *
 * \date Jul 4, 2011
 * \author Christian Askeland, SINTEF
 */
class CoordinateSystemListener: public QObject
{
Q_OBJECT

public:
	CoordinateSystemListener(ssc::CoordinateSystem space);
	virtual ~CoordinateSystemListener();
	void setSpace(ssc::CoordinateSystem space);
	ssc::CoordinateSystem getSpace() const;
signals:
	void changed();
private slots:
	void reconnect();
private:
	void doConnect();
	void doDisconnect();
	ssc::CoordinateSystem mSpace;
};
typedef boost::shared_ptr<CoordinateSystemListener> CoordinateSystemListenerPtr;

} //namespace ssc
#endif // COORDINATESYSTEMLISTENER_H_
