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
#ifndef CXPIPELINE_H
#define CXPIPELINE_H

#include "cxResourceFilterExport.h"

#include "cxFilter.h"
#include "cxFilterGroup.h"
#include "cxXmlOptionItem.h"
#include "cxSelectDataStringProperty.h"

namespace cx
{
typedef boost::shared_ptr<class TimedBaseAlgorithm> TimedAlgorithmPtr;
typedef boost::shared_ptr<class CompositeTimedAlgorithm> CompositeTimedAlgorithmPtr;

typedef boost::shared_ptr<class StringPropertyFusedInputOutputSelectData> StringPropertyFusedInputOutputSelectDataPtr;

/** Data-Property that fuses two Data-Properties.
  *
  * There are two arguments: input and output.
  * The output is wrapped by this adapter,
  * changes in the input will set the output.
  *
  *
  */
class cxResourceFilter_EXPORT StringPropertyFusedInputOutputSelectData : public SelectDataStringPropertyBase
{
	Q_OBJECT
public:
	virtual ~StringPropertyFusedInputOutputSelectData() {}
	static StringPropertyFusedInputOutputSelectDataPtr create(PatientModelServicePtr patientModelService, SelectDataStringPropertyBasePtr base, SelectDataStringPropertyBasePtr input);

public: // basic methods
	virtual QString getDisplayName() const;
	virtual bool setValue(const QString& value);
	virtual QString getValue() const;

public: // optional methods
	virtual QStringList getValueRange() const;
	virtual QString convertInternal2Display(QString internal);
	virtual QString getHelp() const;

public: // interface extension
	virtual DataPtr getData() const;
	virtual void setValueName(const QString name);
	virtual void setHelp(QString text);

protected slots:
	void inputDataChangedSlot();
	void changedSlot();

protected:
	StringPropertyFusedInputOutputSelectData(PatientModelServicePtr patientModelService, SelectDataStringPropertyBasePtr base, SelectDataStringPropertyBasePtr input);
	SelectDataStringPropertyBasePtr mInput;
	SelectDataStringPropertyBasePtr mBase;
	QString mValueName;
};



/** Sequential execution of Filters.
 *
 * \ingroup cxPluginAlgorithms
 * \date Nov 22, 2012
 * \author christiana
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResourceFilter_EXPORT Pipeline : public QObject
{
	Q_OBJECT
public:
	explicit Pipeline(PatientModelServicePtr patientModelService, QObject *parent = 0);
	/**
	  * Initialize pipeline. Do once before use.
	  */
	void initialize(FilterGroupPtr filter);
	/**
	  * Get all filters in pipeline
	  */
	FilterGroupPtr getFilters() const;
	/**
	  * Set all options with the named valueName to the value,
	  * given that they are of the correct type.
	  *
	  * Supported types: bool, double, QString, QColor
	  */
	void setOption(QString valueName, QVariant value);
	/**
	  * Get all nodes. If there are N filters, there are N+1 nodes.
	  * Node N are input to filter N, and have node N+1 as output.
	  *
	  * Nodes are a fusion of output/input of filters in the pipeline.
	  * Setting of an output will autoset the input of the next filter
	  * through this mechanism.
	  */
	std::vector<SelectDataStringPropertyBasePtr> getNodes();
	/**
	  * Get the TimedAlgorithm for a given filter.
	  * Do not run this directly: Rather use the execute() method
	  * to run all filters serially.
	  */
	TimedAlgorithmPtr getTimedAlgorithm(QString uid);
	/**
	  * Get the TimedAlgorithm for the entire pipeline. This is
	  * a composition of all the individual filters.
	  * Do not run this directly: Rather use the execute() method,
	  * use this for extra stuff like checking for finished.
	  */
	TimedAlgorithmPtr getPipelineTimedAlgorithm();
	/**
	  * Execute the filter at filterIndex. Recursively execute
	  * all filters earlier in the pipeline if they dont have
	  * an output value.
	  *
	  * Empty input tries to update the pipeline output, i.e. execute
	  * all filters required to generate the final output, or none if already
	  * filled.
	  */
	void execute(QString uid = "");

signals:

public slots:
	void nodeValueChanged(QString uid, int index);

private:
	void setOption(PropertyPtr adapter, QVariant value);
	std::vector<SelectDataStringPropertyBasePtr> createNodes();

	FilterGroupPtr mFilters;
	std::vector<SelectDataStringPropertyBasePtr> mNodes;
	std::map<QString, TimedAlgorithmPtr> mTimedAlgorithm;
	CompositeTimedAlgorithmPtr mCompositeTimedAlgorithm;
	PatientModelServicePtr mPatientModelService;
};
typedef boost::shared_ptr<Pipeline> PipelinePtr;

} // namespace cx

#endif // CXPIPELINE_H
