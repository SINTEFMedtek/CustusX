/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
 * \ingroup cx_resource_view_internal
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
