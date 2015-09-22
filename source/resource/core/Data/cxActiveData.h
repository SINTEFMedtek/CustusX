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

#ifndef CXACTIVEDATA_H
#define CXACTIVEDATA_H

#include "cxResourceExport.h"
#include <QObject>
#include <QList>
#include "boost/shared_ptr.hpp"

namespace cx
{
typedef boost::shared_ptr<class ActiveData> ActiveDataPtr;
typedef boost::shared_ptr<class Data> DataPtr;
typedef boost::shared_ptr<class Image> ImagePtr;

/** \brief Provides the last active data of warious types
 *
 * \ingroup cx_resource_core_data
 *
 * \date seb 28, 2015
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT ActiveData : public QObject
{
	Q_OBJECT
public:
	ActiveData();
	virtual QList<DataPtr> getActiveDataHistory() const;
	DataPtr getActiveData() const;
	DataPtr getActiveData(QString typeRegexp) const;
	template <class DATA>
	boost::shared_ptr<DATA> getActiveData() const;
	ImagePtr getDerivedActiveImage() const;///< In addition to returning Image this also provides derived (changing) images from TrackedStream
	virtual void setActiveData(DataPtr activeData);
//	virtual void setActiveData(QString uid);

signals:
	void activeImageChanged(const QString& uId);
	void activeDataChanged(const QString& uId);

private:
	QList<DataPtr> mActiveData;

	void emitSignals(DataPtr activeData);
	void emitActiveDataChanged();
	void emitActiveImageChanged();
	QString getChangedUid(DataPtr activeData) const;
	QList<DataPtr> getActiveDataHistory(QString typeRegexp) const;
};

template <class DATA>
boost::shared_ptr<DATA> ActiveData::getActiveData() const
{
	boost::shared_ptr<DATA> retval;
	QList<DataPtr> activeDataList = this->getActiveDataHistory();
	for(int i = activeDataList.size() - 1; i >= 0; --i)
	{
		retval = boost::dynamic_pointer_cast<DATA>(activeDataList.at(i));
		if(retval)
			return retval;
	}
	return retval;
}


} //cx

#endif // CXACTIVEDATA_H
