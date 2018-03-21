/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewRepCollection.h"

#include <vtkImageActor.h>
#include <vtkImageData.h>
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"

#include "cxRep.h"
#include "cxTypeConversions.h"

namespace cx
{


ViewRepCollection::ViewRepCollection(vtkRenderWindowPtr renderWindow, const QString& uid, const QString& name)
{
	QString myuid = uid;
	if (myuid.isEmpty())
	{
		myuid = QString::number(reinterpret_cast<long>(this));
	}
	mRenderWindow = renderWindow;
	mBackgroundColor = QColor("black");
	mUid = myuid;
	mName = name;
	mType = View::VIEW;

	this->clear();
}

ViewRepCollection::~ViewRepCollection()
{
	removeReps();

	if (mRenderer)
		mRenderWindow->RemoveRenderer(mRenderer);
}

QString ViewRepCollection::getTypeString() const
{
	switch (this->getType())
	{
	case View::VIEW:
		return "View";
	case View::VIEW_2D:
		return "View2D";
	case View::VIEW_3D:
		return "View3D";
	case View::VIEW_REAL_TIME:
		return "ViewRealTime";
	}
	return "";
}

QString ViewRepCollection::getUid()
{
	return mUid;
}

QString ViewRepCollection::getName()
{
	return mName;
}

vtkRendererPtr ViewRepCollection::getRenderer() const
{
	return mRenderer;
}

void ViewRepCollection::addRep(const RepPtr& rep)
{
	if (hasRep(rep))
	{
		return;
	}

	rep->connectToView(mSelf.lock());
	mReps.push_back(rep);
}

void ViewRepCollection::setBackgroundColor(QColor color)
{
	mBackgroundColor = color;
	if (mRenderer)
	{
		mRenderer->SetBackground(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF());
	}
}

/**clear all content of the view. This ensures that props added from
 * outside the rep system also is cleared, and data not cleared with
 * RemoveAllViewProps() (added to fix problem in snw ultrasound rep,
 * data was not cleared, dont know why).
 */
void ViewRepCollection::clear()
{
	removeReps();

	if (mRenderer)
		mRenderWindow->RemoveRenderer(mRenderer);

	mRenderer = vtkRendererPtr::New();
	mRenderer->SetBackground(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF());
	mRenderWindow->AddRenderer(mRenderer);
}

void ViewRepCollection::removeReps()
{
	for (RepsIter it = mReps.begin(); it != mReps.end(); ++it)
	{
		(*it)->disconnectFromView(mSelf.lock());
	}
	mReps.clear();
}

void ViewRepCollection::removeRep(const RepPtr& rep)
{
	RepsIter it = std::find(mReps.begin(), mReps.end(), rep);

	if (it == mReps.end())
	{
		return;
	}

	rep->disconnectFromView(mSelf.lock());
	mReps.erase(it);
}

std::vector<RepPtr> ViewRepCollection::getReps()
{
	return mReps;
}

bool ViewRepCollection::hasRep(const RepPtr& rep) const
{
	return std::count(mReps.begin(), mReps.end(), rep);
}

void ViewRepCollection::print(std::ostream& os)
{
	Indent ind;
	printSelf(os, ind);
}

void ViewRepCollection::printSelf(std::ostream & os, Indent indent)
{
	os << indent << "mUid: " << mUid << std::endl;
	os << indent << "mName: " << mName << std::endl;
	os << indent << "NumberOfReps: " << mReps.size() << std::endl;

	for (unsigned i = 0; i < mReps.size(); ++i)
	{
		os << indent << "<Rep child " << i << ">" << std::endl;
		mReps[i]->printSelf(os, indent.stepDown());
		os << indent << "</Rep child " << i << ">" << std::endl;
	}

	if (indent.includeDetails())
	{
		os << indent << "<RenderWindow>" << std::endl;
		mRenderWindow->PrintSelf(os, indent.getVtkIndent().GetNextIndent());
		os << indent << "</RenderWindow>" << std::endl;
		os << indent << "<Renderer>" << std::endl;
		mRenderer->PrintSelf(os, indent.getVtkIndent().GetNextIndent());
		os << indent << "</Renderer>" << std::endl;
		os << indent << "<Props>" << std::endl;
		vtkPropCollection* collection = mRenderer->GetViewProps();
		collection->InitTraversal();
		vtkProp* prop = collection->GetNextProp();
		while (prop)
		{
			os << indent << indent << "<Prop>" << std::endl;
			prop->PrintSelf(os, indent.getVtkIndent().GetNextIndent().GetNextIndent());
			os << indent << indent << "</Prop>" << std::endl;
			prop = collection->GetNextProp();
		}
		os << indent << "</Props>" << std::endl;
	}
}

void ViewRepCollection::setModified()
{
	this->getRenderer()->Modified();
	this->getRenderWindow()->Modified();
}

int ViewRepCollection::computeTotalMTime()
{
	unsigned long hash = 0;

	hash += this->getRenderer()->GetMTime();
	hash += this->getRenderWindow()->GetMTime();
	vtkPropCollection* props = this->getRenderer()->GetViewProps();
	props->InitTraversal();
	for (vtkProp* prop = props->GetNextProp(); prop != NULL; prop = props->GetNextProp())
	{
		vtkImageActor* imageActor = vtkImageActor::SafeDownCast(prop);
		if (imageActor && imageActor->GetInput())
		{
			hash += imageActor->GetInput()->GetMTime();
		}
		hash += prop->GetMTime();
		hash += prop->GetRedrawMTime();
	}

	return hash;
}

} // namespace cx
