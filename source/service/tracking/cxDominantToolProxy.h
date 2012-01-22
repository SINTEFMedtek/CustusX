/*
 * cxDominantToolProxy.h
 *
 *  Created on: Sep 9, 2011
 *      Author: christiana
 */

#ifndef CXDOMINANTTOOLPROXY_H_
#define CXDOMINANTTOOLPROXY_H_

#include "sscTool.h"

namespace cx
{
using ssc::Transform3D; // because of signaling

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
 */
class DominantToolProxy : public QObject
{
	Q_OBJECT

public:
	static DominantToolProxyPtr New() { return DominantToolProxyPtr(new DominantToolProxy()); }
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
	void probeChanged();

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
