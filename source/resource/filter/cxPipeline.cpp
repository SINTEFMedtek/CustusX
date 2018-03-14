/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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


StringPropertyFusedInputOutputSelectDataPtr StringPropertyFusedInputOutputSelectData::create(PatientModelServicePtr patientModelService,
																								   SelectDataStringPropertyBasePtr base,
																								   SelectDataStringPropertyBasePtr input)
{
	StringPropertyFusedInputOutputSelectDataPtr retval(new StringPropertyFusedInputOutputSelectData(patientModelService, base, input));
	return retval;
}

StringPropertyFusedInputOutputSelectData::StringPropertyFusedInputOutputSelectData(PatientModelServicePtr patientModelService,
																						 SelectDataStringPropertyBasePtr base,
																						 SelectDataStringPropertyBasePtr input) :
	SelectDataStringPropertyBase(patientModelService)
{
	mBase = base;
	mInput = input;
	connect(mInput.get(), SIGNAL(changed()), this, SLOT(inputDataChangedSlot()));
	//    connect(mInput.get(), SIGNAL(dataChanged(QString)), this, SLOT(inputDataChangedSlot()));
	connect(mBase.get(), SIGNAL(dataChanged(QString)), this, SIGNAL(dataChanged(QString)));
	connect(mBase.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
	connect(mBase.get(), SIGNAL(changed()), this, SIGNAL(changed()));
}


bool StringPropertyFusedInputOutputSelectData::setValue(const QString& value)
{
	return mBase->setValue(value);
}

QString StringPropertyFusedInputOutputSelectData::getValue() const
{
	return mBase->getValue();
}

QString StringPropertyFusedInputOutputSelectData::getDisplayName() const
{
	if (mValueName.isEmpty())
		return mBase->getDisplayName();
	return mValueName;
}

QStringList StringPropertyFusedInputOutputSelectData::getValueRange() const
{
	return mBase->getValueRange();
}

QString StringPropertyFusedInputOutputSelectData::convertInternal2Display(QString internal)
{
	return mBase->convertInternal2Display(internal);
}

QString StringPropertyFusedInputOutputSelectData::getHelp() const
{
//	return mBase->getHelp();
	return QString("<html>"
	               "<h4>%1</h4> <p>%2</p>"
	               "<h4>%3</h4> <p>%4</p>"
	               "</html>")
	        .arg("Input").arg(mInput->getHelp())
	        .arg("Output").arg(mBase->getHelp());
}

DataPtr StringPropertyFusedInputOutputSelectData::getData() const
{
	return mBase->getData();
}

void StringPropertyFusedInputOutputSelectData::setValueName(const QString name)
{
	if (name==mValueName)
		return;
	//    mBase->setValueName(name);
	mValueName = name;
	emit changed();
}

void StringPropertyFusedInputOutputSelectData::setHelp(QString text)
{
	mBase->setHelp(text);
}

void StringPropertyFusedInputOutputSelectData::changedSlot()
{
	// this sync helps the pipeline behaving when the output is changed.
	this->blockSignals(true);
	mInput->setValue(mBase->getValue());
	this->blockSignals(false);
}

void StringPropertyFusedInputOutputSelectData::inputDataChangedSlot()
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
		std::vector<PropertyPtr> options = filter->getOptions();

		for (unsigned j=0; j<options.size(); ++j)
		{
			if (options[j]->getDisplayName()==valueName)
				this->setOption(options[j], value);
		}
	}
}

void Pipeline::setOption(PropertyPtr adapter, QVariant value)
{
	if (value.canConvert<bool>())
	{
		BoolPropertyBasePtr specific = boost::dynamic_pointer_cast<BoolPropertyBase>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<bool>(value));
	}
	else if (value.canConvert<double>())
	{
		DoublePropertyBasePtr specific = boost::dynamic_pointer_cast<DoublePropertyBase>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<double>(value));
	}
	else if (value.canConvert<QColor>())
	{
		ColorPropertyBasePtr specific = boost::dynamic_pointer_cast<ColorPropertyBase>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<QColor>(value));
	}
	else if (value.canConvert<QString>())
	{
		StringPropertyBasePtr specific = boost::dynamic_pointer_cast<StringPropertyBase>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<QString>(value));
	}
	else
	{
		reportWarning(QString("Attempt to set option of type %2 is not supported").arg(value.typeName()));
	}
}


std::vector<SelectDataStringPropertyBasePtr> Pipeline::getNodes()
{
	// TODO: create getMainXXType() in filters instead of using zero.

	if (mNodes.empty())
		mNodes = this->createNodes();
	return mNodes;
}

std::vector<SelectDataStringPropertyBasePtr> Pipeline::createNodes()
{
	// TODO: create fused nodes: input+output
	// TODO: create getMainXXType() in filters instead of using zero.

	std::vector<SelectDataStringPropertyBasePtr> retval;

	if (mFilters->empty())
		return retval;

	// first node is the input of the first algo
	retval.push_back(mFilters->get(0)->getInputTypes()[0]);

	// intermediate nodes are fusions between output and input
	for (unsigned i=1; i<mFilters->size(); ++i)
	{
		SelectDataStringPropertyBasePtr output = mFilters->get(i-1)->getOutputTypes()[0];
		SelectDataStringPropertyBasePtr base  = mFilters->get(i)->getInputTypes()[0];
		StringPropertyFusedInputOutputSelectDataPtr node;
		node = StringPropertyFusedInputOutputSelectData::create(mPatientModelService, base, output);
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
