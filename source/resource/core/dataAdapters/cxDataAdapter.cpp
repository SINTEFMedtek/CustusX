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


#include "cxDataAdapter.h"

namespace cx
{

DataAdapter::DataAdapter() :
		QObject(), mEnabled(true), mAdvanced(false), mGroup("")
{}


DataAdapterPtr DataAdapter::findAdapter(std::vector<DataAdapterPtr> adapters, QString id)
{
	for(int i=0; i<adapters.size(); ++i)
	{
		DataAdapterPtr adapter = adapters[i];
		if(QString::compare(adapter->getUid(), id) == 0)
		{
			return adapter;
		}
	}
	return DataAdapterPtr();
}

bool DataAdapter::getEnabled() const
{
	return mEnabled;
}

bool DataAdapter::getAdvanced() const
{
	return mAdvanced;
}

QString DataAdapter::getGroup() const
{
	return mGroup;
}

bool DataAdapter::setEnabled(bool enabling)
{

	if(mEnabled == enabling)
		return false;

	mEnabled = enabling;
	emit changed();

	return true;
}

bool DataAdapter::setAdvanced(bool advanced)
{
	if(advanced == mAdvanced)
		return false;

	mAdvanced = advanced;
	emit changed();

	return true;
}

bool DataAdapter::setGroup(QString name)
{
	if(name == mGroup)
		return false;

	mGroup = name;
	emit changed();

	return true;
}


} //namespace cx
