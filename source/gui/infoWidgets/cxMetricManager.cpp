/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxMetricManager.h"
#include "cxManualTool.h"
#include "cxViewGroupData.h"
#include "cxTrackingService.h"
#include <QFile>
#include <QTextStream>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <vtkMNITagPointReader.h>
#include <vtkStringArray.h>
#include "vtkForwardDeclarations.h"
#include "cxDataReaderWriter.h"
#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxFrameMetric.h"
#include "cxToolMetric.h"
#include "cxPlaneMetric.h"
#include "cxShapedMetric.h"
#include "cxCustomMetric.h"
#include "cxAngleMetric.h"
#include "cxSphereMetric.h"
#include "cxRegionOfInterestMetric.h"
#include "cxDataFactory.h"
#include "cxSpaceProvider.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxOrderedQDomDocument.h"
#include "cxXmlFileHandler.h"
#include "cxTime.h"
#include "cxErrorObserver.h"
#include "cxHelperWidgets.h"
#include "cxFileHelpers.h"
#include "cxSpaceProperty.h"
#include "cxSpaceEditWidget.h"

namespace cx
{

MetricManager::MetricManager(ViewServicePtr viewService, PatientModelServicePtr patientModelService, TrackingServicePtr trackingService, SpaceProviderPtr spaceProvider) :
	QObject(NULL),
	mViewService(viewService),
	mPatientModelService(patientModelService),
	mTrackingService(trackingService),
	mSpaceProvider(spaceProvider)
{
	connect(trackingService.get(), &TrackingService::stateChanged, this, &MetricManager::metricsChanged);
	connect(patientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(metricsChanged()));
}

DataMetricPtr MetricManager::getMetric(QString uid)
{
	DataPtr data = mPatientModelService->getData(uid);
	DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(data);
	return metric;
}

int MetricManager::getNumberOfMetrics() const
{
	return this->getAllMetrics().size();
}

std::vector<DataMetricPtr> MetricManager::getAllMetrics() const
{
	std::vector<DataMetricPtr> retval;
	std::map<QString, DataPtr> all = mPatientModelService->getDatas();
	for (std::map<QString, DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
	{
		DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(iter->second);
		if (metric)
			retval.push_back(metric);
	}
	return retval;
}

void MetricManager::setSelection(std::set<QString> selection)
{
	mSelection = selection;
}

void MetricManager::setActiveUid(QString uid)
{
	mActiveLandmark = uid;
	emit activeMetricChanged();
}

void MetricManager::moveToMetric(QString uid)
{
	  DataMetricPtr metric = this->getMetric(uid);
	  if (!metric)
		  return;
	  Vector3D p_r = metric->getRefCoord();;
	  this->setManualToolPosition(p_r);
}

void MetricManager::setManualToolPosition(Vector3D p_r)
{
	Transform3D rMpr = mPatientModelService->get_rMpr();
	Vector3D p_pr = rMpr.inv().coord(p_r);

	// set the picked point as offset tip
	ToolPtr tool = mTrackingService->getManualTool();
	Vector3D offset = tool->get_prMt().vector(Vector3D(0, 0, tool->getTooltipOffset()));
	p_pr -= offset;
	p_r = rMpr.coord(p_pr);

	// TODO set center here will not do: must handle
	mPatientModelService->setCenter(p_r);
	Vector3D p0_pr = tool->get_prMt().coord(Vector3D(0, 0, 0));
	tool->set_prMt(createTransformTranslate(p_pr - p0_pr) * tool->get_prMt());
}

PointMetricPtr MetricManager::addPoint(Vector3D point, CoordinateSystem space, QString uid, QColor color)
{
	PointMetricPtr p1 =	mPatientModelService->createSpecificData<PointMetric>(uid);
	p1->get_rMd_History()->setParentSpace("reference");
	p1->setSpace(space);
	p1->setCoordinate(point);
    p1->setColor(color);
	mPatientModelService->insertData(p1);

	mViewService->getGroup(0)->addData(p1->getUid());
	this->setActiveUid(p1->getUid());

	return p1;
}

DistanceMetricPtr MetricManager::addDistance(QString uid)
{
	DistanceMetricPtr d0 = mPatientModelService->createSpecificData<DistanceMetric>(uid);
	d0->get_rMd_History()->setParentSpace("reference");

	std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
	for (unsigned i=0; i<args.size(); ++i)
		d0->getArguments()->set(i, args[i]);

	this->installNewMetric(d0);

	return d0;
}

void MetricManager::addPointButtonClickedSlot()
{
	this->addPointInDefaultPosition();
}

PointMetricPtr MetricManager::addPointInDefaultPosition()
{
	CoordinateSystem ref = CoordinateSystem::reference();
    QColor color = QColor(240, 170, 255, 255);
	Vector3D p_ref = mSpaceProvider->getActiveToolTipPoint(ref, true);

	DataPtr data = mPatientModelService->getData(mActiveLandmark);
    if(!data)

        return this->addPoint(p_ref, ref,"point%1", color);

    PointMetricPtr pointMetric = boost::dynamic_pointer_cast<PointMetric>(data);
    if(pointMetric)
    {
        ref = pointMetric->getSpace();
		p_ref = mSpaceProvider->getActiveToolTipPoint(ref, true);
    }

    DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(data);
    if(metric)
        color = metric->getColor();



    return this->addPoint(p_ref, ref,"point%1", color);
}

void MetricManager::addFrameButtonClickedSlot()
{
	FrameMetricPtr frame = mPatientModelService->createSpecificData<FrameMetric>("frame%1");
  frame->get_rMd_History()->setParentSpace("reference");

  CoordinateSystem ref = CoordinateSystem::reference();
  Transform3D rMt = mSpaceProvider->getActiveToolTipTransform(ref, true);

  frame->setSpace(ref);
  frame->setFrame(rMt);

  this->installNewMetric(frame);
}

void MetricManager::addToolButtonClickedSlot()
{
	ToolMetricPtr frame = mPatientModelService->createSpecificData<ToolMetric>("tool%1");
  frame->get_rMd_History()->setParentSpace("reference");

  CoordinateSystem ref = CoordinateSystem::reference();
  Transform3D rMt = mSpaceProvider->getActiveToolTipTransform(ref, true);

  frame->setSpace(ref);
  frame->setFrame(rMt);
  frame->setToolName(mTrackingService->getActiveTool()->getName());
  frame->setToolOffset(mTrackingService->getActiveTool()->getTooltipOffset());

  this->installNewMetric(frame);
}

void MetricManager::addPlaneButtonClickedSlot()
{
  PlaneMetricPtr p1 = mPatientModelService->createSpecificData<PlaneMetric>("plane%1");
  p1->get_rMd_History()->setParentSpace("reference");

  std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(p1->getArguments());
  for (unsigned i=0; i<args.size(); ++i)
	p1->getArguments()->set(i, args[i]);

  this->installNewMetric(p1);
}

/**Starting with a selection of allowed arguments for a new metric,
 * refine them by removing nonselected items, and adding more point
 * metrics if there are too few arguments.
 */
std::vector<DataPtr> MetricManager::refinePointArguments(std::vector<DataPtr> args, unsigned argNo)
{
  // erase non-selected arguments if we have more than enough
  //std::set<QString> selectedUids = this->getSelectedUids();
  for (unsigned i=0; i<args.size();)
  {
	if (args.size() <= argNo)
		break;
	if (!mSelection.count(args[i]->getUid()))
		args.erase(args.begin()+i);
	else
		++i;
  }

  while (args.size() > argNo)
	args.erase(args.begin());

  while (args.size() < argNo)
  {
	  PointMetricPtr p0 = this->addPointInDefaultPosition();
	  args.push_back(p0);
  }

  return args;
}

void MetricManager::addROIButtonClickedSlot()
{
	RegionOfInterestMetricPtr d0 = mPatientModelService->createSpecificData<RegionOfInterestMetric>("roi%1");
	d0->get_rMd_History()->setParentSpace("reference");
	this->installNewMetric(d0);
}

void MetricManager::addDistanceButtonClickedSlot()
{
	this->addDistance();
}

void MetricManager::addAngleButtonClickedSlot()
{
	AngleMetricPtr d0 = mPatientModelService->createSpecificData<AngleMetric>("angle%1");
  d0->get_rMd_History()->setParentSpace("reference");

  std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments(), 3);
  d0->getArguments()->set(0, args[0]);
  d0->getArguments()->set(1, args[1]);
  d0->getArguments()->set(2, args[1]);
  d0->getArguments()->set(3, args[2]);

  this->installNewMetric(d0);
}

std::vector<DataPtr> MetricManager::getSpecifiedNumberOfValidArguments(MetricReferenceArgumentListPtr arguments, int numberOfRequiredArguments)
{
	// first try to reuse existing points as distance arguments, otherwise create new ones.

	std::vector<DataMetricPtr> metrics = this->getAllMetrics();

	std::vector<DataPtr> args;
	for (unsigned i=0; i<metrics.size(); ++i)
	{
	  if (arguments->validArgument(metrics[i]))
		  args.push_back(metrics[i]);
	}

	if (numberOfRequiredArguments<0)
		numberOfRequiredArguments = arguments->getCount();
	args = this->refinePointArguments(args, numberOfRequiredArguments);

	return args;
}

void MetricManager::addSphereButtonClickedSlot()
{
	SphereMetricPtr d0 = mPatientModelService->createSpecificData<SphereMetric>("sphere%1");
	d0->get_rMd_History()->setParentSpace("reference");
	std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
	d0->getArguments()->set(0, args[0]);

	this->installNewMetric(d0);
}

void MetricManager::addDonutButtonClickedSlot()
{
	DonutMetricPtr d0 = mPatientModelService->createSpecificData<DonutMetric>("donut%1");
	d0->get_rMd_History()->setParentSpace("reference");
	std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
	d0->getArguments()->set(0, args[0]);
	d0->getArguments()->set(1, args[1]);

	this->installNewMetric(d0);
}

void MetricManager::addCustomButtonClickedSlot()
{
	CustomMetricPtr d0 = mPatientModelService->createSpecificData<CustomMetric>("Custom%1");
    d0->get_rMd_History()->setParentSpace("reference");
    std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
    d0->getArguments()->set(0, args[0]);
    d0->getArguments()->set(1, args[1]);

    this->installNewMetric(d0);
}

void MetricManager::installNewMetric(DataMetricPtr metric)
{
    DataMetricPtr prevMetric = this->getMetric(mActiveLandmark);
    if(prevMetric)
    {
        metric->setColor(prevMetric->getColor());
    }

	mPatientModelService->insertData(metric);
	this->setActiveUid(metric->getUid());
	mViewService->getGroup(0)->addData(metric->getUid());
}

void MetricManager::loadReferencePointsSlot()
{
  ToolPtr refTool = mTrackingService->getReferenceTool();
  if(!refTool) // we only load reference points from reference tools
  {
	reportDebug("No reference tool, cannot load reference points into the pointsampler");
	return;
  }

  std::map<int, Vector3D> referencePoints_s = refTool->getReferencePoints();
  if(referencePoints_s.empty())
  {
	reportWarning("No referenceppoints in reference tool "+refTool->getName());
	return;
  }

  CoordinateSystem ref = CoordinateSystem::reference();
  CoordinateSystem sensor = mSpaceProvider->getS(refTool);

  std::map<int, Vector3D>::iterator it = referencePoints_s.begin();
  for(; it != referencePoints_s.end(); ++it)
  {
	Vector3D P_ref = mSpaceProvider->get_toMfrom(sensor, ref).coord(it->second);
	this->addPoint(P_ref, CoordinateSystem(csREF), "ref%1");
  }
}

void MetricManager::exportMetricsToXMLFile(QString& filename)
{
	OrderedQDomDocument orderedDoc;
	QDomDocument& doc = orderedDoc.doc();
	doc.appendChild(doc.createProcessingInstruction("xml version =", "'1.0'"));
	QDomElement patientNode = doc.createElement("patient");
	doc.appendChild(patientNode);
	QDomElement managersNode = doc.createElement("managers");
	patientNode.appendChild(managersNode);
	QDomElement datamanagerNode = doc.createElement("datamanager");
	managersNode.appendChild(datamanagerNode);
	std::map<QString, DataPtr> dataMap = mPatientModelService->getDatas();

	std::map<QString, DataPtr>::iterator iter;
	for (iter = dataMap.begin(); iter != dataMap.end(); ++iter)
	{
		DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(iter->second);
		if(metric)
		{
			QDomElement dataNode = doc.createElement("data");
			metric->addXml(dataNode);
			datamanagerNode.appendChild(dataNode);
		}
	}

	XmlFileHandler::writeXmlFile(doc, filename);
}

void MetricManager::resolveUnknownParentSpacesForPointMetrics(QDomNode dataNode, std::map<QString, QString> mapping_of_unknown_to_known_spaces, DataPtr data)
{
	QString uid = data->getUid();
	PointMetricPtr point_metric = boost::static_pointer_cast<PointMetric>(data);
	if(!point_metric)
		return;

	QString string_space = dataNode.toElement().attribute("space");
	CoordinateSystem parentSpace = CoordinateSystem::fromString(string_space);
	bool need_parent = parentSpace.isValid() && (parentSpace.mId == csDATA);
	bool parent_found = mPatientModelService->getData(parentSpace.mRefObject) != DataPtr();
	if(need_parent && !parent_found)
	{
		if(mapping_of_unknown_to_known_spaces.find(string_space) == mapping_of_unknown_to_known_spaces.end())
		{
			SpacePropertyPtr space_property;
			space_property = SpaceProperty::initialize("selectSpace",
													  "Space",
													  "Select parent coordinate system of metric with uid: "+uid);
			space_property->setSpaceProvider(mSpaceProvider);
			QWidget* widget = new QWidget;
			widget->setFocusPolicy(Qt::StrongFocus); // needed for help system: focus is used to display help text
			QVBoxLayout *layout = new QVBoxLayout();
			layout->addWidget(new QLabel("Select parent space for Point metric: "+uid+"."));
			layout->addWidget(new SpaceEditWidget(widget, space_property));
			QDialog dialog;
			dialog.setWindowTitle("Space "+string_space+" does not exist.");
			dialog.setLayout(layout);
			QPushButton *okButton = new QPushButton(tr("Ok"));
			layout->addWidget(okButton);
			connect(okButton, &QAbstractButton::clicked, &dialog, &QWidget::close);
			dialog.exec();
			CX_LOG_DEBUG() << "New space is now: " << space_property->getValue().mId << " " << space_property->getValue().mRefObject;
			CoordinateSystem new_parentspace = space_property->getValue();
			mapping_of_unknown_to_known_spaces[string_space] = new_parentspace.toString();
		}
		parentSpace = CoordinateSystem::fromString(mapping_of_unknown_to_known_spaces[string_space]);
		point_metric->setSpace(parentSpace);
		point_metric->setCoordinate(Vector3D::fromString(dataNode.toElement().attribute("coord")));
		dataNode.toElement().setAttribute("space", parentSpace.toString());
	}
}

void MetricManager::importMetricsFromXMLFile(QString& filename)
{
	QDomDocument xml = XmlFileHandler::readXmlFile(filename);
	QDomElement patientNode = xml.documentElement();

	std::map<DataPtr, QDomNode> datanodes;

	QDomNode managersNode = patientNode.firstChildElement("managers");
	QDomNode datamanagerNode = managersNode.firstChildElement("datamanager");
	QDomNode dataNode = datamanagerNode.firstChildElement("data");


	std::map<QString, QString> mapping_of_unknown_to_known_spaces;

	for (; !dataNode.isNull(); dataNode = dataNode.nextSibling())
	{
		QDomNamedNodeMap attributes = dataNode.attributes();
		QDomNode typeAttribute = attributes.namedItem("type");
		bool isMetric = false;
		if(typeAttribute.isAttr())
		{
			isMetric = typeAttribute.toAttr().value().contains("Metric");
		}

		if (dataNode.nodeName() == "data" && isMetric)
		{

			QString uid = dataNode.toElement().attribute("uid");
			if(mPatientModelService->getData(uid))
			{
				QString name = dataNode.toElement().attribute("name");
				reportWarning("Metric: " + name + ", is already in the model with Uid: " + uid + ". Import skipped.");
				continue;
			}

			DataPtr data = this->loadDataFromXMLNode(dataNode.toElement());
			if (data)
				datanodes[data] = dataNode.toElement();

			//If point metrics space is uknown to the system, user needs to select a new parent -> POPUP DIALOG
			this->resolveUnknownParentSpacesForPointMetrics(dataNode, mapping_of_unknown_to_known_spaces, data);
		}
	}	

	// parse xml data separately: we want to first load all data
	// because there might be interdependencies (cx::DistanceMetric)
	for (std::map<DataPtr, QDomNode>::iterator iter = datanodes.begin(); iter != datanodes.end(); ++iter)
	{
		iter->first->parseXml(iter->second);
	}
}

QColor MetricManager::getRandomColor()
{
	QStringList colorNames = QColor::colorNames();
	int random_int = rand() % colorNames.size();
	QColor color(colorNames[random_int]);
	if(color == QColor("black"))
		color = getRandomColor();

	return color;
}

std::vector<QString> MetricManager::dialogForSelectingVolumesForImportedMNITagFile( int number_of_volumes, QString description)
{
	std::vector<QString> data_uid;

	QDialog selectVolumeDialog;
	selectVolumeDialog.setWindowTitle("Select volume(s) related to points in MNI Tag Point file.");

	QVBoxLayout *layout = new QVBoxLayout();
	QLabel *description_label = new QLabel(description);
	layout->addWidget(description_label);

	std::map<int, StringPropertySelectImagePtr> selectedImageProperties;
	for(int i=0; i < number_of_volumes; ++i)
	{
		StringPropertySelectImagePtr image_property = StringPropertySelectImage::New(mPatientModelService);
		QWidget *widget = createDataWidget(mViewService, mPatientModelService, NULL, image_property);
		layout->addWidget(widget);
		selectedImageProperties[i] = image_property;
	}

	QPushButton *okButton = new QPushButton(tr("Ok"));
	layout->addWidget(okButton);
	connect(okButton, &QAbstractButton::clicked, &selectVolumeDialog, &QWidget::close);
	selectVolumeDialog.setLayout(layout);
	selectVolumeDialog.exec();
	for(int i=0; i < number_of_volumes; ++i)
	{
		StringPropertySelectImagePtr image_property = selectedImageProperties[i];
		data_uid.push_back(image_property->getValue());
	}
	return data_uid;
}

void MetricManager::importMetricsFromMNITagFile(QString &filename, bool testmode)
{
	//--- HACK to be able to read *.tag files with missing newline before eof
	forceNewlineBeforeEof(filename);

	//--- Reader for MNI Tag Point files
	vtkMNITagPointReaderPtr reader = vtkMNITagPointReader::New();
	reader->SetFileName(filename.toStdString().c_str());
	reader->Update();
	if (!ErrorObserver::checkedRead(reader, filename))
		CX_LOG_ERROR() << "Error reading MNI Tag Point file.";


	//--- Prompt user to select the volume(s) that is(are) related to the points in the file
	int number_of_volumes = reader->GetNumberOfVolumes();
	QString description(reader->GetComments());
	std::vector<QString> data_uid;
	data_uid.push_back("");
	data_uid.push_back("");
	if(!testmode)
		data_uid = dialogForSelectingVolumesForImportedMNITagFile(number_of_volumes, description);

	//--- Create the point metrics
	QString type = "pointMetric";
	QString uid = "";
	QString name = "";
	vtkStringArray *labels = reader->GetLabelText();

	for(int i=0; i< number_of_volumes; ++i)
	{
		QColor color = getRandomColor();

		vtkPoints *points = reader->GetPoints(i);
		if(points != NULL)
		{
			unsigned int number_of_points = points->GetNumberOfPoints();
			//CX_LOG_DEBUG() << "Number of points: " << number_of_points;

			for(int j=0; j < number_of_points; ++j)
			{
				vtkStdString label = labels->GetValue(j);
				name = QString(*label); //NB: name never used, using j+1 as name to be able to correlate two sets of points from MNI import
				uid = QDateTime::currentDateTime().toString(timestampMilliSecondsFormat()) + "_" + QString::number(i)+ QString::number(j);

				double *point = points->GetPoint(j);
				DataPtr data = this->createData(type, uid, QString::number(j+1));
				PointMetricPtr point_metric = boost::static_pointer_cast<PointMetric>(data);

				CoordinateSystem space(csDATA, data_uid[i]);
				Vector3D vector_ras(point[0], point[1], point[2]);
				//CX_LOG_DEBUG() << "POINTS: " << vector_ras;

				//Convert from RAS (MINC) to LPS (CX)
				Transform3D sMr = createTransformFromReferenceToExternal(pcsRAS);
				Vector3D vector_lps = sMr.inv() * vector_ras;

				point_metric->setCoordinate(vector_lps);
				point_metric->setSpace(space);
				point_metric->setColor(color);
			}
		}

	}
}

DataPtr MetricManager::loadDataFromXMLNode(QDomElement node)
{
	QString uid = node.toElement().attribute("uid");
	QString name = node.toElement().attribute("name");
	QString type = node.toElement().attribute("type");

	return this->createData(type, uid, name);

}

DataPtr MetricManager::createData(QString type, QString uid, QString name)
{
	DataPtr data = mPatientModelService->createData(type, uid, name);
	if (!data)
	{
		reportError(QString("Not able to create metric with name: %1, uid: %2, type: %3").arg(name).arg(uid).arg(type));
		return DataPtr();
	}

	if (!name.isEmpty())
		data->setName(name);

	mPatientModelService->insertData(data);

	return data;

}


} // namespace cx


