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
#ifndef CXMULTIVOLUME3DREPPRODUCER_H
#define CXMULTIVOLUME3DREPPRODUCER_H

#include <QObject>
#include <QPointer>
#include <boost/shared_ptr.hpp>
#include "sscImage.h"
#include "sscRep.h"

namespace cx
{

typedef boost::shared_ptr<class MultiVolume3DRepProducer> MultiVolume3DVisualizerPtr;

/** 
 *
 *
 * \ingroup cxServiceVisualization
 * \date 4 Sep 2013
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class MultiVolume3DRepProducer : public QObject
{
	Q_OBJECT
public:
	MultiVolume3DRepProducer();

	void setView(ssc::View* view);
	void setMaxRenderSize(int voxels);
	int getMaxRenderSize() const;
	void setVisualizerType(QString type);
	void addImage(ssc::ImagePtr image);
	void removeImage(QString uid);
	std::vector<ssc::RepPtr> getAllReps();
	static QStringList getAvailableVisualizers();
	static std::map<QString, QString> getAvailableVisualizerDisplayNames();

signals:
	void imagesChanged();

private:
	QString mVisualizerType;
	std::vector<ssc::ImagePtr> mImages;
	std::vector<ssc::RepPtr> mReps;
	int mMaxRenderSize;
	ssc::View* mView;

	void updateRepsInView();
	void clearReps();
	void fillReps();

	void rebuildReps();

	void removeRepsFromView();
	void addRepsToView();

	void buildSscGPURayCastMultiVolume();
	void buildVtkOpenGLGPUMultiVolumeRayCastMapper();
	void buildVtkVolumeTextureMapper3D(ssc::ImagePtr image);
	void buildVtkGPUVolumeRayCastMapper(ssc::ImagePtr image);
	void buildSscProgressiveLODVolumeTextureMapper3D(ssc::ImagePtr image);
	bool is2DImage(ssc::ImagePtr image) const;
	void buildSscImage2DRep3D(ssc::ImagePtr image);

	void buildSingleVolumeRenderer(ssc::ImagePtr image);
	bool isSingleVolumeRenderer() const;

};

} // namespace cx



#endif // CXMULTIVOLUME3DREPPRODUCER_H
