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


#ifndef CXREPIMPL_H_
#define CXREPIMPL_H_

#include "cxResourceVisualizationExport.h"

#include <set>
#include "cxRep.h"
#include "cxIndent.h"
#include <vtkSmartPointer.h>
class vtkObject;
typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkCallbackCommand> vtkCallbackCommandPtr;

namespace cx
{
typedef boost::weak_ptr<class View> ViewWeakPtr;

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
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT RepImpl : public Rep
{
	Q_OBJECT
public:
	explicit RepImpl(const QString& uid="", const QString& name="");
	virtual ~RepImpl();
	virtual QString getType() const = 0;
	virtual void connectToView(ViewPtr theView);
	virtual void disconnectFromView(ViewPtr theView);
	virtual bool isConnectedToView(ViewPtr theView) const;
	void setName(QString name);
	QString getName() const; ///< \return a reps name
	QString getUid() const; ///< \return a reps unique id
	virtual void printSelf(std::ostream & os, Indent indent);

	/** Usage:
	  * Define functions in each subclass with the signature:
	  *  static REPPtr New(QString uid="") { return wrap_new(new REP(), uid); }
	  */
	template<class REP>
	static boost::shared_ptr<REP> wrap_new(REP* object, QString uid)
	{
		boost::shared_ptr<REP> retval(object);
		if (uid.isEmpty())
			uid = retval->getType() + "_" + reinterpret_cast<long long>(retval.get());
		retval->mUid = uid;
		retval->mName = uid;
		retval->mSelf = retval;
		return retval;
	}

protected slots:
	void setModified(); // set flag to get onModifiedStartRender() called before next render

protected:
	ViewPtr getView() const;
	vtkRendererPtr getRenderer();
	RepPtr getSelf() { return mSelf.lock(); }


	virtual void onModifiedStartRender() {}
	virtual void addRepActorsToViewRenderer(ViewPtr view) = 0;
	virtual void removeRepActorsFromViewRenderer(ViewPtr view) = 0;
	virtual void onEveryRender() {}

private:
//	RepImpl(); ///< not implemented
	static void ProcessEvents(vtkObject* object,
										unsigned long event,
										void* clientdata,
										void* calldata);
	bool mModified;
	vtkCallbackCommandPtr mCallbackCommand;
	void onStartRenderPrivate();
	ViewWeakPtr mView;
	QString mName;
	QString mUid;
	RepWeakPtr mSelf;

};



} // namespace cx

#endif /*CXREPIMPL_H_*/
