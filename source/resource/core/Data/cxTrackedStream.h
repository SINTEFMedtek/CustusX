/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRACKEDSTREAM_H
#define CXTRACKEDSTREAM_H

#include "cxImage.h"

namespace cx
{

/** \brief A data set for video streams (2D/3D).
 *
 * Allowing video stream as a data type
 *
 * \ingroup cx_resource_core_data
 *
 * \date jan 28, 2015
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT TrackedStream : public Data
{
	Q_OBJECT
public:
	static TrackedStreamPtr create(const QString& uid, const QString& name = "");
	TrackedStream(const QString &uid, const QString &name, const ToolPtr &probe, const VideoSourcePtr &videoSource);
	~TrackedStream();

	void setProbeTool(const ToolPtr &probeTool);
	ToolPtr getProbeTool();
	void setVideoSource(const VideoSourcePtr &videoSource);
	VideoSourcePtr getVideoSource();
	void setSpaceProvider(SpaceProviderPtr spaceProvider);

	virtual void addXml(QDomNode& dataNode);
	virtual void parseXml(QDomNode& dataNode);

	virtual DoubleBoundingBox3D boundingBox() const;
	virtual bool load(QString path) { return true;} ///< Not used
	virtual void save(const QString& basePath) {} ///< Not used

	virtual QString getType() const;
	static QString getTypeName();

	ImagePtr getChangingImage();
	bool is3D();
	bool is2D();
	bool hasVideo() const;
	bool isStreaming() const;
signals:
	void streamChanged(QString uid);
	void newTool(ToolPtr tool);
	void newVideoSource(VideoSourcePtr videoSource);
	void newFrame();
	void streaming(bool on); ///< emitted when streaming started/stopped
	void newPosition();

private slots:
	void newFrameSlot();
	void toolTransformAndTimestamp(Transform3D prMt, double timestamp);
private:
	ToolPtr mProbeTool;
	VideoSourcePtr mVideoSource;
	ImagePtr mImage;

	SpaceProviderPtr mSpaceProvider;
	Transform3D get_tMu();
};

typedef boost::shared_ptr<TrackedStream> TrackedStreamPtr;

} //cx

#endif // CXTRACKEDSTREAM_H
