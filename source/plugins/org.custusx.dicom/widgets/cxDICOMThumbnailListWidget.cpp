/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt include
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QMetaType>
#include <QPersistentModelIndex>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>

// ctk includes
#include "ctkLogger.h"

// ctkWidgets includes
#include "ctkFlowLayout.h"
#include "ctkThumbnailListWidget_p.h"
//#include "ui_ctkThumbnailListWidget.h"

//ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMFilterProxyModel.h"
#include "cxDICOMModel.h"

// ctkDICOMWidgets includes
#include "cxDICOMThumbnailListWidget.h"
#include "ctkThumbnailLabel.h"

// STD includes
#include <iostream>

// DCMTK includes
#include <dcmimage.h>

static ctkLogger logger("org.commontk.DICOM.Widgets.DICOMThumbnailListWidget");

Q_DECLARE_METATYPE(QPersistentModelIndex);

namespace cx
{
typedef DICOMModel ctkDICOMModel;

//----------------------------------------------------------------------------
class DICOMThumbnailListWidgetPrivate : ctkThumbnailListWidgetPrivate
{
  Q_DECLARE_PUBLIC(DICOMThumbnailListWidget);
public:
  typedef ctkThumbnailListWidgetPrivate Superclass;

  DICOMThumbnailListWidgetPrivate(DICOMThumbnailListWidget* parent);

  QSharedPointer<ctkDICOMDatabase> Database;
  QString DatabaseDirectory;
  QModelIndex CurrentSelectedModel;

//  void addThumbnailWidget(const QModelIndex &imageIndex, const QModelIndex& sourceIndex, const QString& text);

  void addPatientThumbnails(const QModelIndex& patientIndex);
  void addStudyThumbnails(const QModelIndex& studyIndex);
  void addSeriesThumbnails(const QModelIndex& seriesIndex);

  void addThumbnailWidget(QString filename, const QString &text);
  void addThumbnailWidget(QString studyUid, QString seriesUid, QString imageUid, QString caption);
  QStringList getFilesForImage(QString studyUid, QString seriesUid, QString imageUid);
  QStringList getThumbnailsForSeries(QString studyUid, QString seriesUid);

private:
  Q_DISABLE_COPY( DICOMThumbnailListWidgetPrivate );
};

//----------------------------------------------------------------------------
// DICOMThumbnailListWidgetPrivate methods

//----------------------------------------------------------------------------
DICOMThumbnailListWidgetPrivate
::DICOMThumbnailListWidgetPrivate(DICOMThumbnailListWidget* parent)
  : Superclass(parent)
{

}

//----------------------------------------------------------------------------
void DICOMThumbnailListWidgetPrivate
::addPatientThumbnails(const QModelIndex &index)
{
  QModelIndex patientIndex = index;

  ctkDICOMModel* model = const_cast<ctkDICOMModel*>(
	qobject_cast<const ctkDICOMModel*>(index.model()));

  if(model)
    {
    model->fetchMore(patientIndex);
	const int studyCount = model->rowCount(patientIndex);

    for(int i=0; i<studyCount; i++)
      {
	  QModelIndex studyIndex = patientIndex.child(i, 0);

	  this->addStudyThumbnails(studyIndex);
	  }
    }
}

//----------------------------------------------------------------------------
void DICOMThumbnailListWidgetPrivate::addStudyThumbnails(const QModelIndex &index)
{
	QModelIndex studyIndex = index;

	ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

	if (!model)
	{
		return;
	}
	model->fetchMore(studyIndex);

	const int seriesCount = model->rowCount(studyIndex);

	for(int i=0; i<seriesCount; i++)
	{
		QModelIndex seriesIndex = studyIndex.child(i, 0);
		model->fetchMore(seriesIndex);

		QString studyUid = model->data(studyIndex ,ctkDICOMModel::UIDRole).toString();
		QString seriesUid = model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString();

		QString caption = model->data(seriesIndex, Qt::DisplayRole).toString();

		QStringList thumbnails = this->getThumbnailsForSeries(studyUid, seriesUid);

		if (thumbnails.empty())
			continue;
		QString file = thumbnails[thumbnails.size()/2];
		this->addThumbnailWidget(file, caption);
	}

}

void DICOMThumbnailListWidgetPrivate::addSeriesThumbnails(const QModelIndex &index)
{
  QModelIndex studyIndex = index.parent();
  QModelIndex seriesIndex = index;

  ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

  if (!model)
	{
	return;
	}
  model->fetchMore(seriesIndex);

  QString studyUid = model->data(studyIndex ,ctkDICOMModel::UIDRole).toString();
  QString seriesUid = model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString();

  QStringList thumbnails = this->getThumbnailsForSeries(studyUid, seriesUid);

  for (int i=0; i<thumbnails.size(); ++i)
  {
	  QString caption = QString("Image %1").arg(i);
	  this->addThumbnailWidget(thumbnails[i], caption);
  }

//  for (int i=0; i<files.size(); ++i)
//  {
//	  QString imageUid = Database->instanceForFile(files[i]);
//	  QString caption = QString("Image %1").arg(i);
//	  this->addThumbnailWidget(studyUid, seriesUid, imageUid, caption);
//  }
}

//----------------------------------------------------------------------------

QStringList DICOMThumbnailListWidgetPrivate::getThumbnailsForSeries(QString studyUid, QString seriesUid)
{
	QStringList files = Database->filesForSeries(seriesUid);

	QStringList thumbnails;
	for (int i=0; i<files.size(); ++i)
	{
		QString imageUid = Database->instanceForFile(files[i]);
		QStringList filenames = this->getFilesForImage(studyUid, seriesUid, imageUid);
		thumbnails << filenames;
	}
	return thumbnails;
}

void DICOMThumbnailListWidgetPrivate::addThumbnailWidget(QString studyUid, QString seriesUid, QString imageUid, QString caption)
{
	QStringList filenames = this->getFilesForImage(studyUid, seriesUid, imageUid);

	for (int i=0; i<filenames.size(); ++i)
	{
		this->addThumbnailWidget(filenames[0], QString("%1-%2").arg(caption).arg(i));
	}
}

/** Match filenames generated by class cx::DICOMThumbnailGenerator
  *
  */
QStringList DICOMThumbnailListWidgetPrivate::getFilesForImage(QString studyUid, QString seriesUid, QString imageUid)
{
	QString baseFilename = QString("%1/thumbs/%2/%3/%4.png")
			.arg(this->DatabaseDirectory)
			.arg(studyUid)
			.arg(seriesUid)
			.arg(imageUid);

	if(QFileInfo(baseFilename).exists())
	{
		QStringList() << baseFilename;
	}

	QStringList splitPath = baseFilename.split(".");
	QStringList retval;

	for (int i=0; true; ++i)
	{
		splitPath.back() = QString("frame%1.png").arg(i);
		QString frameFilename = splitPath.join(".");
		retval << frameFilename;

		if(!QFileInfo(frameFilename).exists())
			break;
	}
	return retval;
}

void DICOMThumbnailListWidgetPrivate::addThumbnailWidget(QString filename, const QString &text)
{
  if(!QFileInfo(filename).exists())
	{
	  qDebug() << "Thumbnail not found: " << filename;
	  return;
	}

  ctkThumbnailLabel* widget = new ctkThumbnailLabel(this->ScrollAreaContentWidget);

  QString widgetLabel = text;
  widget->setText( widgetLabel );
  QPixmap pix(filename);
  logger.debug("Setting pixmap to " + filename);
  if(this->ThumbnailSize.isValid())
	{
	widget->setFixedSize(this->ThumbnailSize);
	}
  widget->setPixmap(pix);

//  QVariant var;
//  var.setValue(QPersistentModelIndex(sourceIndex));
//  widget->setProperty("sourceIndex", var);

  this->addThumbnail(widget);
}

//void DICOMThumbnailListWidgetPrivate
//::addThumbnailWidget(const QModelIndex& imageIndex,
//                     const QModelIndex& sourceIndex, const QString &text)
//{
//  ctkDICOMModel* model = const_cast<ctkDICOMModel*>(
//    qobject_cast<const ctkDICOMModel*>(imageIndex.model()));

//  if(!model)
//    {
//    return;
//    }
//  QModelIndex seriesIndex = imageIndex.parent();
//  QModelIndex studyIndex = seriesIndex.parent();

//  QString thumbnailPath = this->DatabaseDirectory +
//						  "/thumbs/" + model->data(studyIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
//                          model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString() + "/" +
//                          model->data(imageIndex, ctkDICOMModel::UIDRole).toString() + ".png";
//  if(!QFileInfo(thumbnailPath).exists())
//    {
//    return;
//    }
//  ctkThumbnailLabel* widget = new ctkThumbnailLabel(this->ScrollAreaContentWidget);

//  QString widgetLabel = text;
//  widget->setText( widgetLabel );
//  QPixmap pix(thumbnailPath);
//  logger.debug("Setting pixmap to " + thumbnailPath);
//  if(this->ThumbnailSize.isValid())
//    {
//    widget->setFixedSize(this->ThumbnailSize);
//    }
//  widget->setPixmap(pix);

//  QVariant var;
//  var.setValue(QPersistentModelIndex(sourceIndex));
//  widget->setProperty("sourceIndex", var);

//  this->addThumbnail(widget);
//}

//----------------------------------------------------------------------------
// DICOMThumbnailListWidget methods

//----------------------------------------------------------------------------
DICOMThumbnailListWidget::DICOMThumbnailListWidget(QWidget* _parent)
  : Superclass(new DICOMThumbnailListWidgetPrivate(this), _parent)
{

}

//----------------------------------------------------------------------------
DICOMThumbnailListWidget::~DICOMThumbnailListWidget()
{

}

//----------------------------------------------------------------------------
void DICOMThumbnailListWidget::setDatabaseDirectory(const QString &directory){
  Q_D(DICOMThumbnailListWidget);

  d->DatabaseDirectory = directory;
}

void DICOMThumbnailListWidget::setDatabase(QSharedPointer<ctkDICOMDatabase> database)
{
	Q_D(DICOMThumbnailListWidget);
	d->Database = database;
}


void DICOMThumbnailListWidget::selectThumbnailFromIndex(const QModelIndex &index){
  Q_D(DICOMThumbnailListWidget);
	qDebug() << "=====DICOMThumbnailListWidget::selectThumbnailFromIndex" ;

  if(!d->CurrentSelectedModel.isValid())
    {
    return;
    }
  if(index.parent() != d->CurrentSelectedModel)
    {
    return;
    }

  ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

  if(model)
    {
    int count = d->ScrollAreaContentWidget->layout()->count();

    for(int i=0; i<count; i++)
      {
      ctkThumbnailLabel* thumbnailWidget = qobject_cast<ctkThumbnailLabel*>(d->ScrollAreaContentWidget->layout()->itemAt(i)->widget());
      if(thumbnailWidget->property("sourceIndex").value<QPersistentModelIndex>() == index)
        {
        thumbnailWidget->setSelected(true);
        d->ScrollArea->ensureWidgetVisible(thumbnailWidget);
		qDebug() << "     DICOMThumbnailListWidget::selectThumbnailFromIndex successfully selected" ;
		}
      else
        {
        thumbnailWidget->setSelected(false);
        }
      }
    }
}

//----------------------------------------------------------------------------
void DICOMThumbnailListWidget::addThumbnails(const QModelIndex &index)
{
	qDebug() << "==DICOMThumbnailListWidget::addThumbnails" ;

  Q_D(DICOMThumbnailListWidget);

  this->clearThumbnails();

  ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

  if(model)
    {
    QModelIndex index0 = index.sibling(index.row(), 0);

    d->CurrentSelectedModel = index0;

    if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::PatientType) )
      {
	  d->addPatientThumbnails(index0);
	  }
    else if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::StudyType) )
      {
      d->addStudyThumbnails(index0);
      }
    else if ( model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::SeriesType) )
      {
      d->addSeriesThumbnails(index0);
      }
    }

  this->setCurrentThumbnail(0);
}

} // namespace cx

