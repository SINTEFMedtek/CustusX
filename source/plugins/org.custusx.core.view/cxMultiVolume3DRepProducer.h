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
#ifndef CXMULTIVOLUME3DREPPRODUCER_H
#define CXMULTIVOLUME3DREPPRODUCER_H

#include "org_custusx_core_view_Export.h"

#include <QObject>
#include <QPointer>
#include <boost/shared_ptr.hpp>
#include "cxImage.h"
#include "cxRep.h"

namespace cx
{

typedef boost::shared_ptr<class MultiVolume3DRepProducer> MultiVolume3DVisualizerPtr;

/** 
 *
 *
 * \ingroup org_custusx_core_view
 * \date 4 Sep 2013
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_core_view_EXPORT MultiVolume3DRepProducer : public QObject
{
	Q_OBJECT
public:
	MultiVolume3DRepProducer();
	~MultiVolume3DRepProducer();

	void setView(ViewPtr view);
	void setMaxRenderSize(int voxels);
	int getMaxRenderSize() const;
	void setVisualizerType(QString type);
	void addImage(ImagePtr image);
	void removeImage(QString uid);
	std::vector<RepPtr> getAllReps();
//	static QStringList getAvailableVisualizers();
//	static std::map<QString, QString> getAvailableVisualizerDisplayNames();
	void removeRepsFromView();

signals:
	void imagesChanged();

private slots:
	void clearReps();

private:
	QString mVisualizerType;
	std::vector<ImagePtr> m2DImages;
	std::vector<ImagePtr> m3DImages;
	std::vector<RepPtr> mReps;
	int mMaxRenderSize;
	ViewPtr mView;

	void updateRepsInView();
	void fillReps();
	bool contains(ImagePtr image) const;

	void rebuildReps();
	void rebuild2DReps();
	void rebuild3DReps();

	void addRepsToView();

	ImagePtr removeImageFromVector(QString uid, std::vector<ImagePtr> &images);

	void buildVtkOpenGLGPUMultiVolumeRayCastMapper();
	void buildVtkVolumeTextureMapper3D(ImagePtr image);
	void buildVtkGPUVolumeRayCastMapper(ImagePtr image);
	void buildSscImage2DRep3D(ImagePtr image);

	void buildSingleVolumeRenderer(ImagePtr image);
	bool isSingleVolumeRenderer() const;

};

} // namespace cx



#endif // CXMULTIVOLUME3DREPPRODUCER_H
