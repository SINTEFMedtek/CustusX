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
#include "cxPipeline.h"
#include "cxTypeConversions.h"

#include <QtCore>
#include "boost/bind.hpp"
#include "libQtSignalAdapters/Qt2Func.h"
#include "libQtSignalAdapters/ConnectionFactories.h"


#include "cxLogger.h"
#include "cxBoolPropertyBase.h"
#include "cxDoublePropertyBase.h"
#include "cxColorPropertyBase.h"
#include "cxStringPropertyBase.h"
#include "cxCompositeTimedAlgorithm.h"
#include "cxFilterTimedAlgorithm.h"

namespace cx
{


FusedInputOutputSelectDataStringDataAdapterPtr FusedInputOutputSelectDataStringDataAdapter::create(PatientModelServicePtr patientModelService,
																								   SelectDataStringDataAdapterBasePtr base,
																								   SelectDataStringDataAdapterBasePtr input)
{
	FusedInputOutputSelectDataStringDataAdapterPtr retval(new FusedInputOutputSelectDataStringDataAdapter(patientModelService, base, input));
	return retval;
}

FusedInputOutputSelectDataStringDataAdapter::FusedInputOutputSelectDataStringDataAdapter(PatientModelServicePtr patientModelService,
																						 SelectDataStringDataAdapterBasePtr base,
																						 SelectDataStringDataAdapterBasePtr input) :
	SelectDataStringDataAdapterBase(patientModelService)
{
	mBase = base;
	mInput = input;
	connect(mInput.get(), SIGNAL(changed()), this, SLOT(inputDataChangedSlot()));
	//    connect(mInput.get(), SIGNAL(dataChanged(QString)), this, SLOT(inputDataChangedSlot()));
	connect(mBase.get(), SIGNAL(dataChanged(QString)), this, SIGNAL(dataChanged(QString)));
	connect(mBase.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
	connect(mBase.get(), SIGNAL(changed()), this, SIGNAL(changed()));
}


bool FusedInputOutputSelectDataStringDataAdapter::setValue(const QString& value)
{
	return mBase->setValue(value);
}

QString FusedInputOutputSelectDataStringDataAdapter::getValue() const
{
	return mBase->getValue();
}

QString FusedInputOutputSelectDataStringDataAdapter::getDisplayName() const
{
	if (mValueName.isEmpty())
		return mBase->getDisplayName();
	return mValueName;
}

QStringList FusedInputOutputSelectDataStringDataAdapter::getValueRange() const
{
	return mBase->getValueRange();
}

QString FusedInputOutputSelectDataStringDataAdapter::convertInternal2Display(QString internal)
{
	return mBase->convertInternal2Display(internal);
}

QString FusedInputOutputSelectDataStringDataAdapter::getHelp() const
{
//	return mBase->getHelp();
	return QString("<html>"
	               "<h4>%1</h4> <p>%2</p>"
	               "<h4>%3</h4> <p>%4</p>"
	               "</html>")
	        .arg("Input").arg(mInput->getHelp())
	        .arg("Output").arg(mBase->getHelp());
}

DataPtr FusedInputOutputSelectDataStringDataAdapter::getData() const
{
	return mBase->getData();
}

void FusedInputOutputSelectDataStringDataAdapter::setValueName(const QString name)
{
	if (name==mValueName)
		return;
	//    mBase->setValueName(name);
	mValueName = name;
	emit changed();
}

void FusedInputOutputSelectDataStringDataAdapter::setHelp(QString text)
{
	mBase->setHelp(text);
}

void FusedInputOutputSelectDataStringDataAdapter::changedSlot()
{
	// this sync helps the pipeline behaving when the output is changed.
	this->blockSignals(true);
	mInput->setValue(mBase->getValue());
	this->blockSignals(false);
}

void FusedInputOutputSelectDataStringDataAdapter::inputDataChangedSlot()
{
	// the entire point of the class: update mBase when mInput is changed:
	mBase->setValue(mInput->getValue());
}



///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


Pipeline::Pipeline(PatientModelServicePtr patientModelService, QObject *parent) :
		QObject(parent),
		mPatientModelService(patientModelService)
{
	mCompositeTimedAlgorithm.reset(new CompositeSerialTimedAlgorithm("Pipeline"));
}

void Pipeline::initialize(FilterGroupPtr filters)
{
	mFilters = filters;

	for (unsigned i=0; i<mFilters->size(); ++i)
	{
		FilterPtr filter = mFilters->get(i);
		filter->getInputTypes();
		filter->getOutputTypes();
		filter->getOptions();
	}

	this->getNodes();

	for (unsigned i=0; i<mFilters->size(); ++i)
	{
		FilterPtr current = mFilters->get(i);
		mTimedAlgorithm[current->getUid()].reset(new FilterTimedAlgorithm(current));
	}
}
FilterGroupPtr Pipeline::getFilters() const
{
	return mFilters;
}

/** Set all options with the named valueName to the value,
  * given that they are of the correct type.
  *
  * Supported types: bool, double, QString, QColor
  */
void Pipeline::setOption(QString valueName, QVariant value)
{
	for (unsigned i=0; i<mFilters->size(); ++i)
	{
		FilterPtr filter = mFilters->get(i);
		std::vector<DataAdapterPtr> options = filter->getOptions();

		for (unsigned j=0; j<options.size(); ++j)
		{
			if (options[j]->getDisplayName()==valueName)
				this->setOption(options[j], value);
		}
	}
}

void Pipeline::setOption(DataAdapterPtr adapter, QVariant value)
{
	if (value.canConvert<bool>())
	{
		BoolDataAdapterPtr specific = boost::dynamic_pointer_cast<BoolDataAdapter>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<bool>(value));
	}
	else if (value.canConvert<double>())
	{
		DoubleDataAdapterPtr specific = boost::dynamic_pointer_cast<DoubleDataAdapter>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<double>(value));
	}
	else if (value.canConvert<QColor>())
	{
		ColorDataAdapterPtr specific = boost::dynamic_pointer_cast<ColorDataAdapter>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<QColor>(value));
	}
	else if (value.canConvert<QString>())
	{
		StringDataAdapterPtr specific = boost::dynamic_pointer_cast<StringDataAdapter>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<QString>(value));
	}
	else
	{
		reportWarning(QString("Attempt to set option of type %2 is not supported").arg(value.typeName()));
	}
}


std::vector<SelectDataStringDataAdapterBasePtr> Pipeline::getNodes()
{
	// TODO: create getMainXXType() in filters instead of using zero.

	if (mNodes.empty())
		mNodes = this->createNodes();
	return mNodes;
}

std::vector<SelectDataStringDataAdapterBasePtr> Pipeline::createNodes()
{
	// TODO: create fused nodes: input+output
	// TODO: create getMainXXType() in filters instead of using zero.

	std::vector<SelectDataStringDataAdapterBasePtr> retval;

	if (mFilters->empty())
		return retval;

	// first node is the input of the first algo
	retval.push_back(mFilters->get(0)->getInputTypes()[0]);

	// intermediate nodes are fusions between output and input
	for (unsigned i=1; i<mFilters->size(); ++i)
	{
		SelectDataStringDataAdapterBasePtr output = mFilters->get(i-1)->getOutputTypes()[0];
		SelectDataStringDataAdapterBasePtr base  = mFilters->get(i)->getInputTypes()[0];
		FusedInputOutputSelectDataStringDataAdapterPtr node;
		node = FusedInputOutputSelectDataStringDataAdapter::create(mPatientModelService, base, output);
		node->setValueName(QString("Node %1").arg(i));
		retval.push_back(node);
	}

	// last node is the output of the last algo
	retval.push_back(mFilters->get(mFilters->size()-1)->getOutputTypes()[0]);

	for (unsigned i=0; i<retval.size(); ++i)
		QtSignalAdapters::connect1<void(QString)>(retval[i].get(), SIGNAL(dataChanged(QString)),
		                                          boost::bind(&Pipeline::nodeValueChanged, this, _1, i));

	return retval;
}


void Pipeline::nodeValueChanged(QString uid, int index)
{
	//    std::cout << "Pipeline::nodeValueChanged(QString uid, int index) " << uid << " " << index << std::endl;

	// clear all nodes beyond the input:
	for (unsigned i=index+1; i<mNodes.size(); ++i)
		mNodes[i]->setValue("");
}

TimedAlgorithmPtr Pipeline::getTimedAlgorithm(QString uid)
{
	return mTimedAlgorithm[uid];
}

TimedAlgorithmPtr Pipeline::getPipelineTimedAlgorithm()
{
	return mCompositeTimedAlgorithm;
}

void Pipeline::execute(QString uid)
{
	// generate |startIndex, endIndex>, pointing to the filters to be executed

	int endIndex = -1;
	int startIndex = endIndex;

	if (uid.isEmpty()) // execute entire pipeline, if necessary
	{
		endIndex = mFilters->size();
		startIndex = endIndex;
	}
	else // execute at least one given filter, more if necessary
	{
		for (unsigned i=0; i<mFilters->size(); ++i)
			if (mFilters->get(i)->getUid()==uid)
				endIndex = i+1; // set end index to after filter to execute;
		startIndex = endIndex-1;
	}

	if (endIndex<0) // input filter not found: ignore
		return;

//	// filter i require node i as input

	//	int startIndex = endIndex;

	// index now counts filters <0...N-1>
	// nodes are <0...N>

	for ( ; startIndex>=-1; --startIndex)
	{
		if (startIndex<0)
			break;
		if (mNodes[startIndex]->getData()) // index output node for filter[startIndex]
			break; // found node with data: stop here
	}

	std::cout << "Pipeline::execute filter range s=|" << startIndex << "," << endIndex << ">" << std::endl;

	if (startIndex<0)
	{
		reportWarning(QString("Cannot execute filter %1: No input data set").arg(uid));
		return;
	}

	mCompositeTimedAlgorithm->clear();
	for (unsigned i=startIndex; i<endIndex; ++i)
		mCompositeTimedAlgorithm->append(mTimedAlgorithm[mFilters->get(i)->getUid()]);

	// run all filters
	mCompositeTimedAlgorithm->execute();
}

//void Pipeline::execute(QString uid)
//{
//	// no input uid: execute entire pipeline
//	if (uid.isEmpty())
//		uid = mFilters->get(mFilters->size()-1)->getUid();

//	int endIndex = -1;

//	for (unsigned i=0; i<mFilters->size(); ++i)
//		if (mFilters->get(i)->getUid()==uid)
//			endIndex = i;
//	if (endIndex<0)
//		return;

//	// filter i require node i as input
//	int startIndex = endIndex;

//	// index now counts filters <0...N-1>
//	// nodes are <0...N>

//	for ( ; startIndex>=-1; --startIndex)
//	{
//		if (startIndex<0)
//			break;
//		if (mNodes[startIndex+1]->getData()) // index output node for filter[startIndex]
//			break; // found node with data: stop here
//	}

//	std::cout << "Pipeline::execute3 s=" << startIndex << ", e=" << endIndex << std::endl;

//	if (startIndex<0)
//	{
//		reportWarning(QString("Cannot execute filter %1: No input data set").arg(uid));
//		return;
//	}

//	mCompositeTimedAlgorithm->clear();
//	for (unsigned i=startIndex; i<=endIndex; ++i)
//		mCompositeTimedAlgorithm->append(mTimedAlgorithm[mFilters->get(i)->getUid()]);

//	// run all filters
//	mCompositeTimedAlgorithm->execute();
//}


} // namespace cx
