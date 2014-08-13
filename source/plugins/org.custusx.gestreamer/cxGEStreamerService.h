// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXGESTREAMERSERVICE_H_
#define CXGESTREAMERSERVICE_H_

#include "cxStreamerService.h"
#include "org_custusx_gestreamer_Export.h"

#include <QMutex>
#include "cxGEStreamer.h"
#include "cxStringDataAdapterXml.h"
#include "cxBoolDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"

namespace cx
{

/**
 * Implementation of streamer service.
 *
 * \ingroup org_custusx_gestreamer
 *
 * \date 2014-08-07
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_gestreamer_EXPORT GEStreamerService : public StreamerService
{
	Q_OBJECT
	Q_INTERFACES(cx::StreamerService)

public:
	GEStreamerService();
	virtual ~GEStreamerService();

	virtual QString getName();
	virtual std::vector<DataAdapterPtr> getSettings(QDomElement root);
	virtual StreamerPtr createStreamer(QDomElement root);

private slots:
	void sendOptions();

private:
	GEStreamer::Options generateOptions();
	long translateToValue(QString value);
	StringDataAdapterXmlPtr getIPOption(QDomElement root);
	DoubleDataAdapterXmlPtr getStreamPortOption(QDomElement root);
	BoolDataAdapterXmlPtr	getUseAutoImageSizeOption(QDomElement root);
	DoubleDataAdapterXmlPtr getBufferSizeOption(QDomElement root);
	StringDataAdapterXmlPtr getImageSizeOption(QDomElement root);
	BoolDataAdapterXmlPtr	getIsotropicOption(QDomElement root);
	StringDataAdapterXmlPtr	getTestModeOption(QDomElement root);
	BoolDataAdapterXmlPtr	getUseOpenCLOption(QDomElement root);
	BoolDataAdapterXmlPtr	getScanconvertedStreamOption(QDomElement root);
	BoolDataAdapterXmlPtr	getTissueStreamOption(QDomElement root);
	BoolDataAdapterXmlPtr	getBandwidthStreamOption(QDomElement root);
	BoolDataAdapterXmlPtr	getFrequencyStreamOption(QDomElement root);
	BoolDataAdapterXmlPtr	getVelocityStreamOption(QDomElement root);

	GEStreamerPtr mStreamer;

	StringDataAdapterXmlPtr mIPDataAdapter;
	DoubleDataAdapterXmlPtr mStreamPortDataAdapter;
	BoolDataAdapterXmlPtr	mUseAutoImageSizeDataAdapter;
	DoubleDataAdapterXmlPtr mBufferSizeDataAdapter;
	StringDataAdapterXmlPtr mImageSizeDataAdapter;
	BoolDataAdapterXmlPtr	mIsotropicDataAdapter;
	StringDataAdapterXmlPtr	mTestModeDataAdapter;
	BoolDataAdapterXmlPtr	mUseOpenCLDataAdapter;
	BoolDataAdapterXmlPtr	mScanconvertedStreamDataAdapter;
	BoolDataAdapterXmlPtr	mTissueStreamDataAdapter;
	BoolDataAdapterXmlPtr	mBandwidthStreamDataAdapter;
	BoolDataAdapterXmlPtr	mFrequencyStreamDataAdapter;
	BoolDataAdapterXmlPtr	mVelocityStreamDataAdapter;


	QDomElement mXmlSettings;
	QMutex mStreamerMutex;

};
typedef boost::shared_ptr<GEStreamerService> GEStreamerServicePtr;

} /* namespace cx */

#endif /* CXGESTREAMERSERVICE_H_ */

