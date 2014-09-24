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
 * \ingroup cx_resource_visualization
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
	virtual void connectToView(View *theView) = 0;

	/**
	 * Do not use this method! It is only used by View to tear down an
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

} // namespace cx

#endif /*CXREP_H_*/
