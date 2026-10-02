/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNDITOOL_H_
#define CXNDITOOL_H_

#include "org_custusx_core_tracking_system_ndi_Export.h"

#include <atomic>
#include <QObject>
#include <boost/shared_ptr.hpp>

#include "cxTransform3D.h"
#include "cxToolFileParser.h"
#include "cxTool.h"
#include "cxNdiDevice.h"
#include "cxNdiPositionQuality.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_core_tracking_system_ndi
 * @{
 */

class NdiTool;
typedef boost::shared_ptr<NdiTool> NdiToolPtr;
typedef boost::weak_ptr<NdiTool> NdiToolWeakPtr;

/**
 * \brief Represents one tool tracked by the NDI CAPI.
 * \ingroup org_custusx_core_tracking_system_ndi
 *
 * Stores the tool's XML configuration and current tracking state.
 * Emits signals when the transform is updated by NdiTrackerManager.
 */
class org_custusx_core_tracking_system_ndi_EXPORT NdiTool : public QObject
{
	Q_OBJECT

public:
	explicit NdiTool(ToolFileParser::ToolInternalStructurePtr internalStructure);
	virtual ~NdiTool();

	ToolFileParser::ToolInternalStructurePtr getInternalStructure() const;
	QString getUid() const;

	bool isValid() const;
	bool isVisible() const;
	bool isInitialized() const;
	bool isReference() const;

	int getPortHandle() const;
	void setPortHandle(int handle);

    /**
     * Called by NdiTrackerManager when a new transform is available.
     * Emits toolTransformAndTimestamp and toolVisible signals as needed.
     */
    void updateTransform(const Transform3D& transform, double timestamp, bool visible, const NdiToolSample& sample = NdiToolSample());

	QString updateQuality(const NdiToolSample& sample); ///< Add a sample to the position quality, return the tool's own warning
	void setPositionWarning(QString warning); ///< Emits positionWarning if it changed
	void setMaxErrorRMS(double maxErrorRMS_mm);

    /**
     * Called by NdiTrackerManager when the tool becomes initialized (enabled on device).
     */
    void setInitialized(bool on);
	void setInvisible(); ///< Hide the tool now, without the miss tolerance (tracking stopped, device lost)

signals:
	void toolTransformAndTimestamp(Transform3D matrix, double timestamp, ToolPositionMetadata metadata);
	void toolVisible(bool visible);
	void positionWarning(QString warning); ///< Why the position may be inaccurate, empty if it is OK

public:
	static const int kMissTolerance = 2; ///< Frames a tool may be absent before going invisible
private:

	ToolFileParser::ToolInternalStructurePtr mInternalStructure;
	int mPortHandle;
	bool mValid;
	std::atomic<bool> mVisible;     ///< Written by the tracker thread, read by the main thread
	std::atomic<bool> mInitialized; ///< Written by the tracker thread, read by the main thread
	int mMissedFrames;
	NdiPositionQuality mQuality; ///< Used by the tracker thread only
	QString mPositionWarning;   ///< Used by the tracker thread only
};

/**
 * @}
 */
} // namespace cx

#endif /* CXNDITOOL_H_ */
