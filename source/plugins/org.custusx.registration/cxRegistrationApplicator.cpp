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

#include "cxRegistrationApplicator.h"

#include "cxData.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"

#include "cxRegistrationTransform.h"
#include "cxFrameForest.h"



namespace cx
{

RegistrationApplicator::RegistrationApplicator(const std::map<QString, DataPtr> &source) :
	mSource(source)
{

}

RegistrationApplicator::~RegistrationApplicator()
{

}

/** Update the registration for data and all data connected to its space.
 *
 * Registration is done relative to masterFrame, i.e. data is moved relative to the masterFrame.
 *
 */
void RegistrationApplicator::updateRegistration(QDateTime oldTime, RegistrationTransform delta_pre_rMd, DataPtr movingData, bool silent)
{
  FrameForest forest(mSource);
  QDomNode moving = forest.getNode(movingData->getUid());
  QDomNode fixed = forest.getNode(delta_pre_rMd.mFixed);

  // if no parent, assume this is an operation on the moving image, thus set fixed to its parent.
  if (delta_pre_rMd.mFixed == "")
  {
	  fixed = forest.getNode(movingData->getParentSpace());
  }
  QDomNode movingBase = forest.getOldestAncestorNotCommonToRef(moving, fixed);

  std::vector<DataPtr> allMovingData = forest.getDataFromDescendantsAndSelf(movingBase);

  if(!silent)
	  report(QString(""
					 "Update Registration using\n"
					 "\tFixed:\t%1\n"
					 "\tMoving:\t%2\n"
					 "\tDelta matrix (rMd'=Delta*rMd)\n"
					 "%3")
			 .arg(delta_pre_rMd.mFixed)
			 .arg(movingData->getUid())
			 .arg(qstring_cast(delta_pre_rMd.mValue)));

  this->updateTransform(oldTime, allMovingData, delta_pre_rMd, silent);

  // reconnect only if master and target are unconnected, i.e. doesnt share a common ancestor.
  // If we are registrating inside an already connected tree we only want to change transforms,
  // not change the topology of the tree.
  if (forest.getOldestAncestor(moving) != forest.getOldestAncestor(fixed))
  {
	// connect the target to the master's ancestor, i.e. replace targetBase with masterAncestor:

	QDomNode fixedAncestor = forest.getOldestAncestor(fixed);
	QString fixedAncestorUid = fixedAncestor.toElement().tagName();

	QString newFixedSpace = fixedAncestorUid;

	// if fixedAncestor is a data, insert a pure space above it
	if (mSource.count(fixedAncestorUid) && mSource[fixedAncestorUid]->getParentSpace()=="")
	{
		newFixedSpace = this->generateNewSpaceUid();
		ParentSpace newParentSpace(newFixedSpace, delta_pre_rMd.mTimestamp, delta_pre_rMd.mType);
		this->changeParentSpace(oldTime, mSource[fixedAncestorUid], newParentSpace);
	}

	QString movingBaseUid = movingBase.toElement().tagName();
	// if movingBaseUid is a data, then move the space above it
	if (mSource.count(movingBaseUid))
	{
		movingBaseUid = mSource[movingBaseUid]->getParentSpace();
	}

	// change parent space of all moving spaces connected to base
	ParentSpace newParentSpace(newFixedSpace, delta_pre_rMd.mTimestamp, delta_pre_rMd.mType);
	this->changeParentSpace(oldTime, allMovingData, movingBaseUid, newParentSpace);
  }
}

QString RegistrationApplicator::generateNewSpaceUid() const
{
	int max = 0;
	std::map<QString, DataPtr>::const_iterator iter;
	for (iter = mSource.begin(); iter != mSource.end(); ++iter)
	{
		QStringList parentList = qstring_cast(iter->second->getParentSpace()).split("_");
		if (parentList.size() < 2)
			continue;
		max = std::max(max, parentList[1].toInt());
	}
	QString parentFrame = "frame_" + qstring_cast(max + 1);
	return parentFrame;
}

void RegistrationApplicator::updateTransform(QDateTime oldTime, std::vector<DataPtr> data, RegistrationTransform delta_pre_rMd, bool silent)
{
	// update the transform on all target data:
	for (unsigned i=0; i<data.size(); ++i)
	{
	  RegistrationTransform newTransform = delta_pre_rMd;
	  newTransform.mValue = delta_pre_rMd.mValue * data[i]->get_rMd();
	  data[i]->get_rMd_History()->updateRegistration(oldTime, newTransform);

	  if(!silent)
		  report("Updated registration of data " + data[i]->getName());
	}
}

void RegistrationApplicator::changeParentSpace(QDateTime oldTime, std::vector<DataPtr> data, QString oldParentSpace, ParentSpace newParentSpace)
{
	for (unsigned i=0; i<data.size(); ++i)
	{
		if (data[i]->getParentSpace() != oldParentSpace)
			continue;
		this->changeParentSpace(oldTime, data[i], newParentSpace);
	}
}

void RegistrationApplicator::changeParentSpace(QDateTime oldTime, DataPtr data, ParentSpace newParentSpace)
{
	report(QString("Reset parent frame of %1 from [%2] to [%3].")
		   .arg(data->getName())
		   .arg(data->getParentSpace())
		   .arg(newParentSpace.mValue));

	data->get_rMd_History()->updateParentSpace(oldTime, newParentSpace);
}

} // namespace cx
