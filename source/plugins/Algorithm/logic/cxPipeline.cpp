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
#include "cxPipeline.h"
#include "sscTypeConversions.h"

#include <QtCore>
#include "boost/bind.hpp"
#include "libQtSignalAdapters/Qt2Func.h"
#include "libQtSignalAdapters/ConnectionFactories.h"

#include "sscMessageManager.h"

#include "sscBoolDataAdapter.h"
#include "sscDoubleDataAdapter.h"
#include "sscColorDataAdapter.h"
#include "sscStringDataAdapter.h"
#include "cxCompositeTimedAlgorithm.h"
#include "cxFilterTimedAlgorithm.h"

namespace cx
{


FusedInputOutputSelectDataStringDataAdapterPtr FusedInputOutputSelectDataStringDataAdapter::create(SelectDataStringDataAdapterBasePtr base, SelectDataStringDataAdapterBasePtr input)
{
	FusedInputOutputSelectDataStringDataAdapterPtr retval(new FusedInputOutputSelectDataStringDataAdapter(base, input));
	return retval;
}

FusedInputOutputSelectDataStringDataAdapter::FusedInputOutputSelectDataStringDataAdapter(SelectDataStringDataAdapterBasePtr base, SelectDataStringDataAdapterBasePtr input)
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

QString FusedInputOutputSelectDataStringDataAdapter::getValueName() const
{
	if (mValueName.isEmpty())
		return mBase->getValueName();
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

ssc::DataPtr FusedInputOutputSelectDataStringDataAdapter::getData() const
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


Pipeline::Pipeline(QObject *parent) :
    QObject(parent)
{
	mCompositeTimedAlgorithm.reset(new CompositeTimedAlgorithm("Pipeline"));
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
			if (options[j]->getValueName()==valueName)
				this->setOption(options[j], value);
		}
	}
}

void Pipeline::setOption(DataAdapterPtr adapter, QVariant value)
{
	if (value.canConvert<bool>())
	{
		ssc::BoolDataAdapterPtr specific = boost::dynamic_pointer_cast<ssc::BoolDataAdapter>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<bool>(value));
	}
	else if (value.canConvert<double>())
	{
		ssc::DoubleDataAdapterPtr specific = boost::dynamic_pointer_cast<ssc::DoubleDataAdapter>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<double>(value));
	}
	else if (value.canConvert<QColor>())
	{
		ssc::ColorDataAdapterPtr specific = boost::dynamic_pointer_cast<ssc::ColorDataAdapter>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<QColor>(value));
	}
	else if (value.canConvert<QString>())
	{
		ssc::StringDataAdapterPtr specific = boost::dynamic_pointer_cast<ssc::StringDataAdapter>(adapter);
		if (specific)
			specific->setValue(qvariant_cast<QString>(value));
	}
	else
	{
		ssc::messageManager()->sendWarning(QString("Attempt to set option of type %2 is not supported").arg(value.typeName()));
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
		node = FusedInputOutputSelectDataStringDataAdapter::create(base, output);
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
		ssc::messageManager()->sendWarning(QString("Cannot execute filter %1: No input data set").arg(uid));
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
//		ssc::messageManager()->sendWarning(QString("Cannot execute filter %1: No input data set").arg(uid));
//		return;
//	}

//	mCompositeTimedAlgorithm->clear();
//	for (unsigned i=startIndex; i<=endIndex; ++i)
//		mCompositeTimedAlgorithm->append(mTimedAlgorithm[mFilters->get(i)->getUid()]);

//	// run all filters
//	mCompositeTimedAlgorithm->execute();
//}


} // namespace cx