/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXCUSTOMMETRIC_H
#define CXCUSTOMMETRIC_H

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxDataMetric.h"
#include "cxPointMetric.h"
#include "cxMetricReferenceArgumentList.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_data
 * @{
 */

typedef boost::shared_ptr<class CustomMetric> CustomMetricPtr;

/** \brief Data class that represents a custom.
 *
 * The custom can be used as a targeting device, for example using needle injection.
 * Use a point metric and a custom metric in a line, and target by visually placing
 * the point inside the custom to aim along the line.
 *
 * \date 2014-02-11
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT CustomMetric: public DataMetric
{
Q_OBJECT
public:
    virtual ~CustomMetric();
    static CustomMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	virtual bool isValid() const;
    virtual QIcon getIcon() {return QIcon(":/icons/metric_custom.png");}

    QString getDefineVectorUpMethod() const;
    void setDefineVectorUpMethod(QString defineVectorUpMethod);
	void setModelUid(QString val);
	QString getModelUid() const;
	DataPtr getModel() const;

	void setScaleToP1(bool val);
	bool getScaleToP1() const;
	void setOffsetFromP0(double val);
	double getOffsetFromP0() const;
	void setOffsetFromP1(double val);
	double getOffsetFromP1() const;
	void setRepeatDistance(double val);
	double getRepeatDistance() const;
	void setTranslationOnly(bool val);
	bool getTranslationOnly() const;
	void setTextureFollowTool(bool val);
	bool getTextureFollowTool() const;

	MetricReferenceArgumentListPtr getArguments() { return mArguments; }
	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual DoubleBoundingBox3D boundingBox() const;
	virtual Vector3D getRefCoord() const;
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
        return "CustomMetric";
	}
	virtual QString getValueAsString() const { return ""; }
	virtual bool showValueInGraphics() const { return false; }

private:
	std::vector<Vector3D> getPositions() const;
	Vector3D getDirection() const;
	Vector3D getVectorUp() const;
	Vector3D getScale() const;

	struct cxResource_EXPORT DefineVectorUpMethods
    {
        DefineVectorUpMethods()
        {
            table = "tableDefinesUp";
            connectedFrameInP1 = "connectedFrameDefinesUp";
			tool = "toolDefinesUp";
        }
        QString table;
        QString connectedFrameInP1;
		QString tool;
		QStringList getAvailableDefineVectorUpMethods() const;
        std::map<QString, QString> getAvailableDefineVectorUpMethodsDisplayNames() const;
    };

    CustomMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	MetricReferenceArgumentListPtr mArguments;
    QString mDefineVectorUpMethod;
	QString mModelUid;
    DefineVectorUpMethods mDefineVectorUpMethods;
	bool mScaleToP1;
	double mOffsetFromP0;
	double mOffsetFromP1;
	double mRepeatDistance;
	bool mShowDistanceMarkers;
	double mDistanceMarkerVisibility;
	bool mTranslationOnly;
	bool mTextureFollowTool;
	SpaceListenerPtr mToolListener;

	Transform3D calculateOrientation(Vector3D pos, Vector3D dir, Vector3D vup, Vector3D scale) const;
	Transform3D calculateRotation(Vector3D dir, Vector3D vup) const;
	Transform3D calculateTransformTo2DImageCenter() const;
	void onPropertiesChanged();
	bool needForToolListenerHasChanged() const;
	void createOrDestroyToolListener();

public:
	CustomMetric::DefineVectorUpMethods getDefineVectorUpMethods() const;
	std::vector<Transform3D> calculateOrientations() const;
	int getRepeatCount() const;
	std::vector<Vector3D> getPointCloud() const;
	bool modelIsImage() const;
	void setShowDistanceMarkers(bool show);
	bool getShowDistanceMarkers() const;
	Vector3D getZeroPosition() const;
	void setDistanceMarkerVisibility(double val);
	double getDistanceMarkerVisibility() const;
	void updateTexture(MeshPtr model, Transform3D rMrr);
};

/**
 * @}
 */
} // namespace cx

#endif // CXSHAPEDMETRIC_H
