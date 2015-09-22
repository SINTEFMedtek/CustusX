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

#include "cxActiveData.h"
#include "cxImage.h"
#include "cxTrackedStream.h"

namespace cx
{

ActiveData::ActiveData()
{
}

QList<DataPtr> ActiveData::getActiveDataHistory() const
{
	return mActiveData;
}

DataPtr ActiveData::getActiveData() const
{
	if(mActiveData.isEmpty())
		return DataPtr();
	return mActiveData.last();
}

DataPtr ActiveData::getActiveData(QString typeRegexp) const
{
	QList<DataPtr> activeDatas = getActiveDataHistory(typeRegexp);

	DataPtr activeData;
	if(!activeDatas.isEmpty())
		activeData = activeDatas.last();

	return activeData;
}

ImagePtr ActiveData::getDerivedActiveImage() const
{
	DataPtr activeData = this->getActiveData("image|trackedStream");
	ImagePtr retval;
	TrackedStreamPtr stream = boost::dynamic_pointer_cast<TrackedStream>(activeData);
	if(stream)
		retval = stream->getChangingImage();
	else
		retval = boost::dynamic_pointer_cast<Image>(activeData);
	return retval;
}

QList<DataPtr> ActiveData::getActiveDataHistory(QString typeRegexp) const
{
	QRegExp reg(typeRegexp);
	QList<DataPtr> activeDatas = this->getActiveDataHistory();

	for(int i = 0; i < activeDatas.size(); )
	{
		int current = i++;
		if(!activeDatas.at(current)->getType().contains(reg))
			activeDatas.removeAt(current);
	}

	return activeDatas;
}

void ActiveData::setActiveData(DataPtr activeData)
{
	if (!activeData)
		return;

	mActiveData.removeAll(activeData);
	mActiveData.append(activeData);

	this->emitSignals(activeData);
}

void ActiveData::emitSignals(DataPtr activeData)
{
	this->emitActiveDataChanged();
	if(activeData && activeData->getType() == "image")
		this->emitActiveImageChanged();
}

void ActiveData::emitActiveImageChanged()
{
	DataPtr activeData = ActiveData::getActiveData<Image>();
	QString uid = getChangedUid(activeData);
	emit activeImageChanged(uid);
}

void ActiveData::emitActiveDataChanged()
{
	DataPtr activeData = this->getActiveData();
	QString uid = getChangedUid(activeData);
	emit activeDataChanged(uid);
}

QString ActiveData::getChangedUid(DataPtr activeData) const
{
	QString uid = "";
	if(activeData)
		uid = activeData->getUid();
	return uid;
}



}//cx
