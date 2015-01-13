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

#ifndef CXVIEWREPCOLLECTION_H
#define CXVIEWREPCOLLECTION_H

#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxView.h"
#include <QColor>

namespace cx
{

typedef boost::shared_ptr<class ViewRepCollection> ViewRepCollectionPtr;

/**
 * \ingroup cx_resource_visualization_internal
 * \date 2014-09-26
 * \author Christian Askeland
 */
class ViewRepCollection : public View
{
Q_OBJECT

public:
	ViewRepCollection(vtkRenderWindowPtr renderWindow, const QString& uid, const QString& name = ""); ///< constructor
	virtual ~ViewRepCollection();

	void print(std::ostream& os);
	virtual void printSelf(std::ostream & os, Indent indent);
	virtual void clear(); ///< Removes everything in the view, inluding reps.

	// Implement pure virtuals in base class
	virtual vtkRenderWindowPtr getRenderWindow() const { return mRenderWindow; } ///< Get the vtkRenderWindow used by this \a View.

	virtual View::Type getType() const
	{
		return mType;
	}
	virtual void setType(View::Type type)
	{
		mType = type;
	}
	virtual QString getTypeString() const;
	virtual QString getUid(); ///< Get a views unique id
	virtual QString getName(); ///< Get a views name
	virtual vtkRendererPtr getRenderer() const; ///< Get the renderer used by this \a View.
	virtual void addRep(const RepPtr& rep); ///< Adds and connects a rep to the view
	virtual void removeRep(const RepPtr& rep); ///< Removes and disconnects the rep from the view
	virtual bool hasRep(const RepPtr& rep) const; ///< Checks if the view already have the rep
	virtual std::vector<RepPtr> getReps(); ///< Returns all reps in the view
	virtual void removeReps(); ///< Removes all reps in the view
	virtual void setBackgroundColor(QColor color);

	virtual void setModified();
	int computeTotalMTime();

	QColor mBackgroundColor;
	QString mUid; ///< The view's unique id
	QString mName; ///< The view's name
	vtkRenderWindowPtr mRenderWindow;
	vtkRendererPtr mRenderer;
	std::vector<RepPtr> mReps; ///< Storage for internal reps.
	typedef std::vector<RepPtr>::iterator RepsIter; ///< Iterator typedef for the internal rep vector.
	View::Type mType;
	boost::weak_ptr<class View> mSelf;
};

} // namespace cx

#endif // CXVIEWREPCOLLECTION_H
