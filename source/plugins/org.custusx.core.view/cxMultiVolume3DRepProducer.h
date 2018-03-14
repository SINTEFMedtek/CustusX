/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
