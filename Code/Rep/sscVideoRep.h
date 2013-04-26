// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscRT2DRep.h
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */

#ifndef SSCRTSTREAMREP_H_
#define SSCRTSTREAMREP_H_

#include "sscRepImpl.h"
#include "sscVideoSource.h"
#include "sscVtkHelperClasses.h"
#include "sscForwardDeclarations.h"
#include "sscProbeSector.h"
#include "sscViewportListener.h"

typedef vtkSmartPointer<class vtkPlaneSource> vtkPlaneSourcePtr;
typedef vtkSmartPointer<class vtkTexture> vtkTexturePtr;
typedef vtkSmartPointer<class vtkTransformTextureCoords> vtkTransformTextureCoordsPtr;
typedef vtkSmartPointer<class vtkDataSetMapper> vtkDataSetMapperPtr;
typedef vtkSmartPointer<class vtkActor> vtkActorPtr;
typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkTextureMapToPlane> vtkTextureMapToPlanePtr;
typedef vtkSmartPointer<class vtkImageMask> vtkImageMaskPtr;

typedef vtkSmartPointer<class UltrasoundSectorSource> UltrasoundSectorSourcePtr;

namespace ssc
{

typedef boost::shared_ptr<class VideoGraphics> VideoGraphicsPtr;


/** \brief Wrap vtkActor displaying a video image, possibly clipped by a sector.
 *
 * Create a pipeline from a video vtkImageData to a vtkActor.
 *
 * \ingroup sscProxy
 */
class VideoGraphicsPipeline
{
public:
	VideoGraphicsPipeline();
	virtual ~VideoGraphicsPipeline();

	vtkActorPtr getActor();
	void setActorUserMatrix(vtkMatrix4x4Ptr rMu);
	void setVisibility(bool visible);

	/** Set a mask for the video image.
	  * Only one of clear and clip can be active at a time.
	  * The mask must be of the same size as the video.
	  * Zeros in the mask will be set to transparent.
	  */
	void setMask(vtkImageDataPtr mask);

	/** Set a clip polygon for the video image.
	  * Only one of clear and clip can be active at a time.
	  * The mask must be of the same size as the video.
	  * Zeros in the mask will be set to transparent.
	  */
	void setClip(vtkPolyDataPtr sector);

	/** Set the imagedata pointing to the video.
	  */
	void setInputVideo(vtkImageDataPtr video);

	/** One of the previously set inputs have been modified. Update the pipeline.
	  */
	void update();

private:
	void setLookupTable();

	void setupPipeline();
	void connectVideoImageToPipeline();
	void updatePlaneSourceBounds();
	void updateLUT();
	bool inputImageIsEmpty();

	vtkImageDataPtr mInputMask;
	vtkPolyDataPtr mInputSector;
	vtkImageDataPtr mInputVideo;

	vtkLookupTablePtr mLUT;
	vtkImageChangeInformationPtr mDataRedirecter;
	vtkActorPtr mPlaneActor;
	vtkPlaneSourcePtr mPlaneSource;
	vtkTexturePtr mTexture;
	UltrasoundSectorSourcePtr mUSSource;
	vtkDataSetMapperPtr mDataSetMapper;
	vtkTransformTextureCoordsPtr mTransformTextureCoords;
	vtkTextureMapToPlanePtr mTextureMapToPlane;

	vtkImageThresholdPtr mMapZeroToOne;
	vtkImageMaskPtr mMaskFilter;
};
typedef boost::shared_ptr<VideoGraphicsPipeline> VideoGraphicsPipelinePtr;

/**\brief Helper class for displaying a VideoSource.
 *
 * Used for Video display in VideoFixedPlaneRep and ToolRep3D.
 *
 *  Used by CustusX.
 *
 * \ingroup sscProxy
 */
class VideoGraphics : public QObject
{
	Q_OBJECT
public:
	VideoGraphics(bool useMaskFilter=false);
	virtual ~VideoGraphics();

	void setRealtimeStream(VideoSourcePtr data);
	void setTool(ToolPtr tool);
	ToolPtr getTool();
	ssc::ProbeSector getProbeData();
	/** Turn sector clipping on/off.
	 *  If on, only the area inside the probe sector is shown.
	 */
	void setClipToSector(bool on);
	void setShowInToolSpace(bool on);
	vtkActorPtr getActor();

signals:
	void newData();

private slots:
	void newDataSlot();
	void receiveTransforms(Transform3D matrix, double timestamp);
	void receiveVisible(bool visible);
	void probeSectorChanged();

private:
	bool mClipToSector;
	VideoGraphicsPipelinePtr mPipeline;
	bool mShowInToolSpace;
	ToolPtr mTool;
	ssc::ProbeSector mProbeData;
	VideoSourcePtr mData;
	ImagePtr mImage;//Can be used instead of mTexture. This allows visualization of rt 3D
};

typedef boost::shared_ptr<class VideoFixedPlaneRep> VideoFixedPlaneRepPtr;


/** \brief Display a VideoSource in a View.
 *
 * A rep visualizing a VideoSource directly into the view plane.
 * It does not follow the tool, but controls the camera in order to
 * fill the entire View.
 *
 * Used by CustusX.
 *
 * \ingroup sscRep
 * \ingroup sscRepVideo
 */
class VideoFixedPlaneRep : public ssc::RepImpl
{
	Q_OBJECT
public:
	VideoFixedPlaneRep(const QString& uid, const QString& name="");
	virtual ~VideoFixedPlaneRep();
	virtual QString getType() const { return "ssc::RealTimeStreamFixedPlaneRep"; }
	void setRealtimeStream(VideoSourcePtr data);
	void setTool(ToolPtr tool);

	void setShowSector(bool on);
	bool getShowSector() const;

protected:
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);
	private slots:
	void newDataSlot();

private:
	void setCamera();
	void updateSector();

	VideoGraphicsPtr mRTGraphics;
	bool mShowSector;

	ToolPtr mTool;
	ssc::ProbeSector mProbeData;
	VideoSourcePtr mData;

	ssc::TextDisplayPtr mStatusText;
	ssc::TextDisplayPtr mInfoText;

	GraphicalPolyData3DPtr mProbeSector;
	GraphicalPolyData3DPtr mProbeOrigin;
	GraphicalPolyData3DPtr mProbeClipRect;

	vtkRendererPtr mRenderer;
	View* mView;
	ViewportListenerPtr mViewportListener;
};


} // namespace ssc

#endif /* SSCRTSTREAMREP_H_ */
