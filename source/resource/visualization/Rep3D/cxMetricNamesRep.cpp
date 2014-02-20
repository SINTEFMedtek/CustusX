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
#include "cxMetricNamesRep.h"

#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>

#include "sscView.h"
#include "sscVtkHelperClasses.h"
#include "sscTypeConversions.h"
#include "sscDataMetric.h"
#include "sscLogger.h"

namespace cx
{

MetricNamesRep::MetricNamesRep(const QString& uid, const QString& name=""):
	RepImpl(uid,name)
{
	mFontSize = 20;
}

MetricNamesRep::~MetricNamesRep()
{
}

MetricNamesRepPtr MetricNamesRep::New(const QString& uid, const QString& name="")
{
	MetricNamesRepPtr retval(new MetricNamesRep(uid, name));
	retval->mSelf = retval;
	return retval;
}

void MetricNamesRep::addRepActorsToViewRenderer(View *view)
{
	for(unsigned i =0; i<mDisplayText.size(); ++i)
	{
		mDisplayText[i]->setRenderer(view->getRenderer());
	}
}

void MetricNamesRep::removeRepActorsFromViewRenderer(View *view)
{
	for(unsigned i =0; i<mDisplayText.size(); ++i)
	{
		mDisplayText[i]->setRenderer(NULL);
	}
}

void MetricNamesRep::setData(std::vector<DataPtr> data)
{
	std::vector<DataMetricPtr> metrics = this->convertToMetrics(data);

	if (this->equal(metrics, mMetrics))
		return;

	for (unsigned i=0; i<mMetrics.size(); ++i)
	{
		disconnect(mMetrics[i].get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
		disconnect(mMetrics[i].get(), SIGNAL(propertiesChanged()), this, SLOT(setModified()));
	}

	mMetrics = metrics;

	for (unsigned i=0; i<mMetrics.size(); ++i)
	{
		connect(mMetrics[i].get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
		connect(mMetrics[i].get(), SIGNAL(propertiesChanged()), this, SLOT(setModified()));
	}

	this->setColoredTextList(this->getAllMetricTexts(), Eigen::Array2d(0.98, 0.98));
}

std::vector<DataMetricPtr> MetricNamesRep::convertToMetrics(std::vector<DataPtr> data)
{
	std::vector<DataMetricPtr> metrics;
	for (unsigned i=0; i<data.size(); ++i)
	{
		DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(data[i]);
		if (metric)
			metrics.push_back(metric);
	}
	return metrics;
}

bool MetricNamesRep::equal(std::vector<DataMetricPtr> a, std::vector<DataMetricPtr> b) const
{
	if (a.size()!=b.size())
		return false;
	for (unsigned i=0; i<a.size(); ++i)
		if (a[i] != b[i])
			return false;
	return true;
}

void MetricNamesRep::onModifiedStartRender()
{
	std::vector<std::pair<QColor, QString> > text;
	text = this->getAllMetricTexts();
	SSC_ASSERT(text.size()==mDisplayText.size());

	for (unsigned i=0; i<mDisplayText.size(); ++i)
	{
		mDisplayText[i]->updateText(text[i].second);
		mDisplayText[i]->setColor(text[i].first);
	}

}

void MetricNamesRep::setColoredTextList(std::vector<std::pair<QColor, QString> > text, Eigen::Array2d pos, vtkViewport *vp)
{
	mDisplayText.clear();

	if (vp==0)
	{
		vp = this->getRenderer();
	}
//	if (vp==0)
//	{
//		return;
//	}

	DoubleBoundingBox3D bb;
	bb = this->findNormalizedBoundingBoxAroundText(text, pos, vp);
	double meanHeight = bb.range()[1]/text.size();

	for (unsigned i=0; i<text.size(); ++i)
	{
		TextDisplayPtr rep;
		rep.reset( new TextDisplay( text[i].second, text[i].first, 	mFontSize) );
		rep->textProperty()->SetJustificationToLeft();
		rep->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();

		Vector3D currentPos = bb.bottomLeft() + (meanHeight*i)*Eigen::Vector3d::UnitY();
		rep->setPosition(currentPos);
		rep->setRenderer(this->getRenderer());

		mDisplayText.push_back(rep);
	}
}

DoubleBoundingBox3D MetricNamesRep::findNormalizedBoundingBoxAroundText(std::vector<std::pair<QColor, QString> > text, Eigen::Array2d pos, vtkViewport *vp)
{
	QStringList fullText;
	for (unsigned i=0; i<text.size(); ++i)
		fullText << text[i].second;
	TextDisplay test(fullText.join("\n"), QColor(Qt::red), mFontSize);
	test.textProperty()->SetJustificationToLeft();
	double width_p = test.getMapper()->GetWidth(vp);
	double height_p = test.getMapper()->GetHeight(vp);
	height_p *= 1.1; // increase spacing

	Eigen::Array2i size_vp(vp->GetSize());
	Eigen::Array2d normsize_box(width_p/size_vp[0], height_p/size_vp[1]);

	// move box into viewport:
	double border = 0.02;
	DoubleBoundingBox3D bb_vp(border, 1.0-border, border, 1.0-border, 0, 0);
	DoubleBoundingBox3D bb_box(pos[0], pos[0]+normsize_box[0], pos[1], pos[1]+normsize_box[1], 0, 0);
	bb_box = this->moveBoxIntoAnother(bb_box, bb_vp);

	return bb_box;
}

DoubleBoundingBox3D MetricNamesRep::moveBoxIntoAnother(DoubleBoundingBox3D box, DoubleBoundingBox3D another)
{
	// move negative direction
	for (unsigned i=0; i<2; ++i)
	{
		double shift = another[2*i+1] - box[2*i+1];
		if (shift < 0)
		{
			box[2*i] += shift;
			box[2*i+1] += shift;
		}
	}

	// move positive direction
	for (unsigned i=0; i<3; ++i)
	{
		double shift = another[2*i] - box[2*i];
		if (shift > 0)
		{
			box[2*i] -= shift;
			box[2*i+1] -= shift;
		}
	}

	return box;
}

void MetricNamesRep::setFontSize(int size)
{
	mFontSize = size;
}

QString MetricNamesRep::getText(DataMetricPtr metric, bool showLabel) const
{
	QStringList text;
	if (showLabel)
		text << metric->getName();
	if (metric->showValueInGraphics())
		text << metric->getValueAsString();
	return text.join(" = ");
}

std::vector<std::pair<QColor, QString> > MetricNamesRep::getAllMetricTexts() const
{
	std::vector<std::pair<QColor, QString> > retval;

	for (unsigned i = 0; i < mMetrics.size(); ++i)
	{
		DataMetricPtr metric = mMetrics[i];
		QString line = metric->getName();

		if (!metric->showValueInGraphics())
			continue;

		QString text = this->getText(metric, true);
		retval.push_back(std::make_pair(metric->getColor(), text));
	}
	std::reverse(retval.begin(), retval.end());
	return retval;
}


} // namespace cx

