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

#ifndef CXDOMINANTTOOLPROXY_H_
#define CXDOMINANTTOOLPROXY_H_

#include "sscTool.h"

namespace cx
{
using ssc::Transform3D;
// because of signaling

/**
 * \file
 * \addtogroup cxServiceTracking
 * @{
 */

typedef boost::shared_ptr<class DominantToolProxy> DominantToolProxyPtr;

/**
 * \brief Helper class for connecting to the dominant tool.
 * \ingroup cxServiceTracking
 *
 * By listening to this class, you will always listen
 * to the dominant tool.
 *
 *  \date Sep 9, 2011
 *  \author christiana
 */
class DominantToolProxy: public QObject
{
Q_OBJECT

public:
	static DominantToolProxyPtr New()
	{
		return DominantToolProxyPtr(new DominantToolProxy());
	}
	DominantToolProxy();

signals:
	// the original tool change signal
	void dominantToolChanged(const QString&);

	// forwarding of dominant tool signals
	void toolTransformAndTimestamp(Transform3D matrix, double timestamp);
	void toolVisible(bool visible);
	void tooltipOffset(double offset);
	void toolProbeSector();
	void tps(int);


private slots:
	void dominantToolChangedSlot(const QString&);
private:
	ssc::ToolPtr mTool;
};

/**
 * @}
 */
}

#endif /* CXDOMINANTTOOLPROXY_H_ */
