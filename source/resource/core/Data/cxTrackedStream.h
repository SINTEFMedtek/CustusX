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
	virtual bool load(QString path, FileManagerServicePtr filemanager) { return true;} ///< Not used
	virtual void save(const QString& basePath, FileManagerServicePtr filemanager) {} ///< Not used

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
