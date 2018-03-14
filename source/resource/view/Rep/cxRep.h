/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXREP_H_
#define CXREP_H_

#include "cxResourceVisualizationExport.h"

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <QObject>

namespace cx
{
typedef boost::shared_ptr<class Rep> RepPtr;
typedef boost::weak_ptr<class Rep> RepWeakPtr;

class Indent;
typedef boost::shared_ptr<class View> ViewPtr;

/**\brief Abstract interface for entities that can be added to a View.
 *
 * Rep (Representation) is the interface that all Representations in ssc must
 * implement.
 *
 * The default implementation can be found in RepImpl. Inherit from that when
 * implementing concrete classes.
 *
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT Rep :public QObject
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
	 * Do not use this method! It is only used by View to set up an
	 * internal connection between the two.
	 *
	 * \param theView The view to add this representation to.
	 * \return
	 */
	virtual void connectToView(ViewPtr theView) = 0;

	/**
	 * Do not use this method! It is only used by View to tear down an
	 * internal connection between the two.
	 *
	 * \param theView The view to disconnect from.
	 * \return
	 */
	virtual void disconnectFromView(ViewPtr theView) = 0;

	/**
	 * Traverse the vector of views to see if this Representation is connected to
	 * one of them.
	 *
	 * \param theView A view pointer.
	 * \return True if \a theView was found in the internal vector of \a View pointers.
	 * false otherwise.
	 */
	virtual bool isConnectedToView(ViewPtr theView) const = 0;
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

} // namespace cx

#endif /*CXREP_H_*/
