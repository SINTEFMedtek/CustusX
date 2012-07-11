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

#ifndef SSCREP_H_
#define SSCREP_H_

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <QObject>

namespace ssc
{
typedef boost::shared_ptr<class Rep> RepPtr;
typedef boost::weak_ptr<class Rep> RepWeakPtr;

class Indent;
class View;
class ViewWidget;

/**\brief Abstract interface for entities that can be added to a View.
 *
 * Rep (Representation) is the interface that all Representations in ssc must
 * implement.
 *
 * The default implementation can be found in RepImpl. Inherit from that when
 * implementing concrete classes.
 *
 * \ingroup sscRep
 */
class Rep :public QObject
{
	Q_OBJECT
public:
	virtual ~Rep() {}

	/**\return a string identifying the Rep type. Each suclass implement
	 * this with its own class name. Subclasses from other namespaces
	 * (such as cx and snw) should add that namespace as a prefix.
	 */
	virtual QString getType() const = 0;

	/**
	 * Do not use this method! It is only used by ssc::View to set up an
	 * internal connection between the two.
	 *
	 * \param theView The view to add this representation to.
	 * \return
	 */
	virtual void connectToView(View *theView) = 0;

	/**
	 * Do not use this method! It is only used by ssc::View to tear down an
	 * internal connection between the two.
	 *
	 * \param theView The view to disconnect from.
	 * \return
	 */
	virtual void disconnectFromView(View *theView) = 0;

	/**
	 * Traverse the vector of views to see if this Representation is connected to
	 * one of them.
	 *
	 * \param theView A view pointer.
	 * \return True if \a theView was found in the internal vector of \a View pointers.
	 * false otherwise.
	 */
	virtual bool isConnectedToView(View *theView) const = 0;
	/** Set the name of this Rep. This is useful for context-menus in
	 * Views and other user interaction constructs.
	 *
	 * \param name The user-friendly name for this instance.
	 */
	virtual void setName(QString name) = 0;
	/**\return a user-friendly name for this instance
	 */
	virtual QString getName() const = 0;
	/**Return an unique id for this instance, i.e. unique
	 * over all Rep instances.
	 * \return the unique id.
	 */
	virtual QString getUid() const = 0;

	virtual void printSelf(std::ostream & os, Indent indent) = 0;

};

} // namespace ssc

#endif /*SSCREP_H_*/
