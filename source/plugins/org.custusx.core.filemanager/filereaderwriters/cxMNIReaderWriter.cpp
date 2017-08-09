#include "cxMNIReaderWriter.h"

#include <QDialog>
#include <QColor>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "vtkMNITagPointReader.h"
#include "vtkStringArray.h"
#include <ctkPluginContext.h>
#include "cxSelectDataStringProperty.h"
#include "cxErrorObserver.h"
#include "cxLogger.h"
#include "cxFileHelpers.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxPointMetric.h"
#include "cxPatientModelServiceProxy.h"
#include "cxViewServiceProxy.h"
#include "cxHelperWidgets.h"
#include "cxSpaceProviderImpl.h"
#include "cxTrackingServiceProxy.h"
#include "cxTime.h"

namespace cx
{


MNIReaderWriter::MNIReaderWriter(ctkPluginContext *context) :
	FileReaderWriterImplService("MNIReaderWriter", "pointMetric", "", "tag", context)
{
	mViewService = ViewServiceProxy::create(context);
	mPatientModelServicePrivate = PatientModelServiceProxy::create(context);

	std::vector<QString> uids;
	uids.push_back("dummyParent1");
	uids.push_back("dummyParent2");
	this->setVolumeUidsRelatedToPointsInMNIPointFile(uids);
}

bool MNIReaderWriter::isNull()
{
	return false;
}

QString MNIReaderWriter::canReadDataType() const
{
	return "pointMetric";
}

bool MNIReaderWriter::canRead(const QString &type, const QString &filename)
{
	return this->canReadInternal(type, filename);
}

//TODO change retval for this function signature?
DataPtr MNIReaderWriter::read(const QString &uid, const QString &filename)
{
	PointMetricPtr pointMetric;
	this->readInto(pointMetric, filename);
	return pointMetric;
}

std::vector<DataPtr> MNIReaderWriter::read(const QString &filename)
{
	std::vector<DataPtr> retval;

	//TODO this needs to be implemented properly
	bool testmode = false;

	//--- HACK to be able to read *.tag files with missing newline before eof
	forceNewlineBeforeEof(filename);

	//TODO
	//std::vector<DataPtr> retval;
	//DataPtr retval;


	//--- Reader for MNI Tag Point files
	vtkMNITagPointReaderPtr reader = vtkMNITagPointReader::New();
	reader->SetFileName(filename.toStdString().c_str());
	reader->Update();
	if (!ErrorObserver::checkedRead(reader, filename))
		CX_LOG_ERROR() << "Error reading MNI Tag Point file.";

	int number_of_volumes = reader->GetNumberOfVolumes();
	QString description(reader->GetComments());
	/*
	//--- Prompt user to select the volume(s) that is(are) related to the points in the file
	bool knownUidAreValid = this->validateKnownVolumeUids(number_of_volumes);
	if(!knownUidAreValid)
	{
		mVolumeUids = dialogForSelectingVolumesForImportedMNITagFile(number_of_volumes, description);
	}
	*/


	//--- Create the point metrics
	QString type = "pointMetric";
	//QString uid = "";
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
				//TODO is this still needed?
				//QString uid = QDateTime::currentDateTime().toString(timestampMilliSecondsFormat()) + "_" + QString::number(i)+ QString::number(j);

				double *point = points->GetPoint(j);
				//DataPtr data = this->createData(type, uid, QString::number(j+1));
				DataPtr data_point_metric = this->createData(type, filename, QString::number(j+1));
				PointMetricPtr point_metric = boost::static_pointer_cast<PointMetric>(data_point_metric);

				CoordinateSystem space(csDATA, mVolumeUids[i]);
				Vector3D vector_ras(point[0], point[1], point[2]);
				//CX_LOG_DEBUG() << "POINTS: " << vector_ras;

				//Convert from RAS (MINC) to LPS (CX)
				Transform3D sMr = createTransformFromReferenceToExternal(pcsRAS);
				Vector3D vector_lps = sMr.inv() * vector_ras;

				point_metric->setCoordinate(vector_lps);
				point_metric->setSpace(space);
				point_metric->setColor(color);

				retval.push_back(data_point_metric);
			}
		}
	}
	mVolumeUids.clear();

	return retval;
}

bool MNIReaderWriter::readInto(DataPtr data, QString path)
{
	CX_LOG_ERROR() << "Do not use readInto...";
	return false;

	/*

	//--- HACK to be able to read *.tag files with missing newline before eof
	forceNewlineBeforeEof(path);

	//TODO
	//std::vector<DataPtr> retval;
	//DataPtr retval;


	//--- Reader for MNI Tag Point files
	vtkMNITagPointReaderPtr reader = vtkMNITagPointReader::New();
	reader->SetFileName(path.toStdString().c_str());
	reader->Update();
	if (!ErrorObserver::checkedRead(reader, path))
		CX_LOG_ERROR() << "Error reading MNI Tag Point file.";


	//--- Prompt user to select the volume(s) that is(are) related to the points in the file
	int number_of_volumes = reader->GetNumberOfVolumes();
	QString description(reader->GetComments());
	bool knownUidAreValid = this->validateKnownVolumeUids(number_of_volumes);
	if(!knownUidAreValid)
	{
		mVolumeUids = dialogForSelectingVolumesForImportedMNITagFile(number_of_volumes, description);
	}

	//--- Create the point metrics
	QString type = "pointMetric";
	//QString uid = "";
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
				//TODO is this still needed?
				QString uid = QDateTime::currentDateTime().toString(timestampMilliSecondsFormat()) + "_" + QString::number(i)+ QString::number(j);

				double *point = points->GetPoint(j);
				//DataPtr data = this->createData(type, uid, QString::number(j+1));
				data = this->createData(type, path, QString::number(j+1));
				PointMetricPtr point_metric = boost::static_pointer_cast<PointMetric>(data);

				CoordinateSystem space(csDATA, mVolumeUids[i]);
				Vector3D vector_ras(point[0], point[1], point[2]);
				//CX_LOG_DEBUG() << "POINTS: " << vector_ras;

				//Convert from RAS (MINC) to LPS (CX)
				Transform3D sMr = createTransformFromReferenceToExternal(pcsRAS);
				Vector3D vector_lps = sMr.inv() * vector_ras;

				point_metric->setCoordinate(vector_lps);
				point_metric->setSpace(space);
				point_metric->setColor(color);

				//TODO make vector
				//retval = point_metric;
			}
		}
	}

	mVolumeUids.clear();

	return data != DataPtr();
	*/
}

QString MNIReaderWriter::canWriteDataType() const
{
	return "";
}

bool MNIReaderWriter::canWrite(const QString &type, const QString &filename) const
{
	return false;
}

void MNIReaderWriter::write(DataPtr data, const QString &filename)
{

}

void MNIReaderWriter::setVolumeUidsRelatedToPointsInMNIPointFile(std::vector<QString> volumeUids)
{
	mVolumeUids = volumeUids;
}

QColor MNIReaderWriter::getRandomColor()
{
	QStringList colorNames = QColor::colorNames();
	int random_int = rand() % colorNames.size();
	QColor color(colorNames[random_int]);
	if(color == QColor("black"))
		color = getRandomColor();

	return color;
}

std::vector<QString> MNIReaderWriter::dialogForSelectingVolumesForImportedMNITagFile( int number_of_volumes, QString description)
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
		StringPropertySelectImagePtr image_property = StringPropertySelectImage::New(mPatientModelServicePrivate);
		QWidget *widget = createDataWidget(mViewService, mPatientModelServicePrivate, NULL, image_property);
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

bool MNIReaderWriter::validateKnownVolumeUids(int numberOfVolumesInFile) const
{
	bool retval = true;
	retval &= numberOfVolumesInFile == mVolumeUids.size();
	for(int i=0; i<mVolumeUids.size(); ++i)
	{
		DataPtr data = mPatientModelServicePrivate->getData(mVolumeUids[i]);
		if(data)
			retval &= true;
	}
	return retval;
}


}
