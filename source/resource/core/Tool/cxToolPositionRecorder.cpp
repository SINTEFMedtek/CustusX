/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxToolPositionRecorder.h"

#include <QStringList>
#include <QTextStream>
#include "cxTime.h"
#include "cxTrackingService.h"
#include "cxLogger.h"

namespace cx
{

void ToolPositionStatistics::addPosition(const Vector3D& position, double timestamp_ms, bool warning)
{
	if (positions == 0)
	{
		firstTimestamp_ms = timestamp_ms;
	}
	lastTimestamp_ms = timestamp_ms;
	positions++;
	positionsWithWarning += warning ? 1 : 0;
	Vector3D delta = position - mean;
	mean += delta / positions;
	sumSquaredDeviations += delta.cwiseProduct(position - mean);
}

Vector3D ToolPositionStatistics::getStandardDeviation() const
{
	Vector3D retval = Vector3D::Zero();
	if (positions > 1)
	{
		retval = (sumSquaredDeviations / (positions - 1)).cwiseSqrt();
	}
	return retval;
}

double ToolPositionStatistics::getRate_Hz() const
{
	double duration_ms = lastTimestamp_ms - firstTimestamp_ms;
	return (positions > 1 && duration_ms > 0.0) ? 1000.0 * (positions - 1) / duration_ms : 0.0;
}

QString ToolPositionStatistics::toString() const
{
	Vector3D sd = this->getStandardDeviation();
	return QString("%1 positions, %2 Hz, %3 with warning, %4 visibility changes, mean (%5, %6, %7) mm, sd (%8, %9, %10) mm")
			.arg(positions).arg(this->getRate_Hz(), 0, 'f', 1).arg(positionsWithWarning).arg(visibilityChanges)
			.arg(mean[0], 0, 'f', 2).arg(mean[1], 0, 'f', 2).arg(mean[2], 0, 'f', 2)
			.arg(sd[0], 0, 'f', 3).arg(sd[1], 0, 'f', 3).arg(sd[2], 0, 'f', 3);
}

ToolPositionRecorder::ToolPositionRecorder(TrackingServicePtr trackingService) :
	mTrackingService(trackingService),
	mRowCount(0)
{
}

ToolPositionRecorder::~ToolPositionRecorder()
{
	this->stop();
}

void ToolPositionRecorder::setTools(std::vector<ToolPtr> tools)
{
	mTools = tools;
	if (this->isRecording())
	{
		this->connectToTools();
	}
}

bool ToolPositionRecorder::start(QString filename, QString label)
{
	this->stop();
	mFile.setFileName(filename);
	bool success = mFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
	if (success)
	{
		mLabel = label;
		mRowCount = 0;
		mStatistics.clear();
		mFile.write((getHeader() + "\n").toUtf8());
		this->connectToTools();
		if (mTrackingService)
		{
			mStateConnection = connect(mTrackingService.get(), &TrackingService::stateChanged, this, &ToolPositionRecorder::connectToTools);
		}
	}
	else
	{
		reportError(QString("Could not open %1 for recording tool positions: %2").arg(filename).arg(mFile.errorString()));
	}
	return success;
}

void ToolPositionRecorder::stop()
{
	disconnect(mStateConnection);
	this->disconnectFromTools();
	if (mFile.isOpen())
	{
		mFile.close();
	}
}

bool ToolPositionRecorder::isRecording() const
{
	return mFile.isOpen();
}

QString ToolPositionRecorder::getFilename() const
{
	return mFile.fileName();
}

int ToolPositionRecorder::getRowCount() const
{
	return mRowCount;
}

std::map<QString, ToolPositionStatistics> ToolPositionRecorder::getStatistics() const
{
	return mStatistics;
}

QString ToolPositionRecorder::getSummary() const
{
	QStringList lines;
	for (const std::pair<const QString, ToolPositionStatistics>& entry : mStatistics)
	{
		lines << QString("%1 %2: %3").arg(mLabel).arg(entry.first).arg(entry.second.toString());
	}
	return lines.join("\n");
}

QString ToolPositionRecorder::getHeader()
{
	return "label,tool,event,timestamp_ms,received_ms,visible,x,y,z,r00,r01,r02,r10,r11,r12,r20,r21,r22,warning,metadata";
}

QString ToolPositionRecorder::formatRow(QString label, QString toolUid, QString event, double timestamp_ms, double received_ms,
										bool visible, const Transform3D& prMt, QString warning, QString metadata)
{
	QStringList fields;
	fields << quoteCsv(label) << quoteCsv(toolUid) << event
		   << QString::number(timestamp_ms, 'f', 3) << QString::number(received_ms, 'f', 3) << QString::number(visible ? 1 : 0);
	Vector3D position = prMt.translation();
	for (int i = 0; i < 3; ++i)
	{
		fields << QString::number(position[i], 'f', 4);
	}
	for (int row = 0; row < 3; ++row)
	{
		for (int col = 0; col < 3; ++col)
		{
			fields << QString::number(prMt(row, col), 'f', 6);
		}
	}
	fields << quoteCsv(warning) << quoteCsv(metadata);
	return fields.join(",");
}

QString ToolPositionRecorder::quoteCsv(QString text)
{
	QString retval = text;
	if (text.contains(',') || text.contains('"') || text.contains('\n'))
	{
		retval = "\"" + QString(text).replace("\"", "\"\"") + "\"";
	}
	return retval;
}

std::vector<ToolPtr> ToolPositionRecorder::getToolsToRecord() const
{
	std::vector<ToolPtr> retval = mTools;
	if (retval.empty() && mTrackingService)
	{
		for (const std::pair<const QString, ToolPtr>& entry : mTrackingService->getTools())
		{
			if (!entry.second->hasType(Tool::TOOL_MANUAL) && entry.second != mTrackingService->getManualTool())
			{
				retval.push_back(entry.second);
			}
		}
	}
	return retval;
}

void ToolPositionRecorder::connectToTools()
{
	this->disconnectFromTools();
	for (ToolPtr tool : this->getToolsToRecord())
	{
		mConnections.push_back(connect(tool.get(), &Tool::toolTransformAndTimestamp, this,
									   [this, tool](Transform3D prMt, double timestamp) { this->onTransform(tool, prMt, timestamp); }));
		mConnections.push_back(connect(tool.get(), &Tool::toolVisible, this,
									   [this, tool](bool visible) { this->onEvent(tool, visible ? "visible" : "hidden"); }));
		mConnections.push_back(connect(tool.get(), &Tool::toolPositionWarning, this,
									   [this, tool](QString) { this->onEvent(tool, "warning"); }));
	}
}

void ToolPositionRecorder::disconnectFromTools()
{
	for (QMetaObject::Connection& connection : mConnections)
	{
		disconnect(connection);
	}
	mConnections.clear();
}

void ToolPositionRecorder::onTransform(ToolPtr tool, const Transform3D& prMt, double timestamp_ms)
{
	QString warning = tool->getPositionWarning();
	mStatistics[tool->getUid()].addPosition(prMt.translation(), timestamp_ms, !warning.isEmpty());
	this->writeRow(formatRow(mLabel, tool->getUid(), "position", timestamp_ms, getMilliSecondsSinceEpoch(),
							 tool->getVisible(), prMt, warning, tool->getMetadata().mData));
}

void ToolPositionRecorder::onEvent(ToolPtr tool, QString event)
{
	if (event != "warning")
	{
		mStatistics[tool->getUid()].visibilityChanges++;
	}
	this->writeRow(formatRow(mLabel, tool->getUid(), event, tool->getTimestamp(), getMilliSecondsSinceEpoch(),
							 tool->getVisible(), tool->get_prMt(), tool->getPositionWarning(), QString()));
}

void ToolPositionRecorder::writeRow(const QString& row)
{
	if (mFile.isOpen())
	{
		mFile.write((row + "\n").toUtf8());
		mRowCount++;
		emit rowCountChanged(mRowCount);
	}
}

} // namespace cx
