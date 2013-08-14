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

#ifndef SSCREPIMPL_H_
#define SSCREPIMPL_H_

#include <set>
#include "sscRep.h"
#include "sscIndent.h"

namespace ssc
{

/**\brief Default implementation of Rep.
 *
 * Subclass from here to get a concrete class.
 *
 * The connection to View is handled here, but subclasses must implement
 * at least:
 * - getType()
 * - addRepActorsToViewRenderer()
 * - removeRepActorsFromViewRenderer().
 *
 * \ingroup sscRep
 */
class RepImpl : public Rep
{
public:
	RepImpl(const QString& uid, const QString& name="");
	virtual ~RepImpl();
	virtual QString getType() const = 0;
	virtual void connectToView(View *theView);
	virtual void disconnectFromView(View *theView);
	virtual bool isConnectedToView(View *theView) const;
	void setName(QString name);
	QString getName() const; ///< \return a reps name
	QString getUid() const; ///< \return a reps unique id
	virtual void printSelf(std::ostream & os, Indent indent);

protected:
	std::set<View *> mViews;
	QString mName;
	QString mUid;
	RepWeakPtr mSelf;

	virtual void addRepActorsToViewRenderer(View *view) = 0;
	virtual void removeRepActorsFromViewRenderer(View *view) = 0;

private:
	RepImpl(); ///< not implemented
};

} // namespace ssc

#endif /*SSCREPIMPL_H_*/
