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

#ifndef CXFILTER_H
#define CXFILTER_H

#include <vector>
#include <QObject>
#include "sscData.h"
#include "sscDataAdapter.h"
#include "cxDataInterface.h"
#include "sscXmlOptionItem.h"
class QDomElement;

namespace cx
{

typedef boost::shared_ptr<class Filter> FilterPtr;

/** Base class for CustusX filters/algorithms
 *
 * Intended for use in a multithreaded environment.
 *
 * The methods getHelp(), getName(), getInputTypes(), getOutputTypes(),
 * getOptions() can be used outside of actual filter execution for
 * initialization and gui interaction.
 *
 * preProcess(), execute() and postProcess() must be called in sequence
 * and together executes the algorithm. They work on a copy of the input
 * data (the input volumes themselved are not copied, only pointers and options).
 *
 *
 * \ingroup cxResourceAlgorithms
 * \date Nov 16, 2012
 * \author christiana
 */
class Filter : public QObject
{
    Q_OBJECT

public:
    explicit Filter();
    virtual ~Filter() {}
    

    /**
      *  Return a unique string for this algorithm.
      */
    virtual QString getUid() const  = 0;
    virtual void setUid(QString uid) = 0;
    /**
      *  Return the type of this algorithm.
      */
    virtual QString getType() const  = 0;
    /**
      *  Return a descriptive name for this algorithm.
      */
    virtual QString getName() const = 0;
    /**
      *  Return a help text describing algorithm usage.
      */
    virtual QString getHelp() const = 0;
    /**
      *  Fill options for this algorithm.
      *  Input is the root node for this algo, filled with stored settings (if any).
      *  On completion, the root is filled with default values for settings.
      */
    virtual std::vector<DataAdapterPtr> getOptions(QDomElement root) = 0;
    /**
      *  List of input arguments.
      */
    virtual std::vector<SelectDataStringDataAdapterBasePtr> getInputTypes() = 0;
    /**
      *  Return a help text describing algorithm usage.
      */
    virtual std::vector<SelectDataStringDataAdapterBasePtr> getOutputTypes() = 0;
    /**
      * Set Active state.
      * Active filters are used by the ui (or similar) and can interact
      * with the system, for example by showing a preview.
      *
      * Inactive filters should no interact with the system.
      */
    virtual void setActive(bool on) = 0;
    /**
      * Perform main thread preprocessing. Copies input data from options and
      * the input adapters into thread-safe storage.
      * Assumes getOptions(), getInputTypes(), getOutputTypes()
      * has been called (this initializes options)
      *
      * \param outputPath is path to data files for current patient.
      * \return success.
      */
    virtual bool preProcess() = 0;
    /**
      * Execute filter with the given inputs and produce an output.
      * This method is threadable - can be executed in a secondary thread.
      * Assumes preprocess has been called.
      *
      * \param input List of data inputs corresponding to getInputTypes()
      * \param settings Reference to settings file containing algorithm-specific settings
      * \return success.
      */
    virtual bool execute() = 0;
    /**
      * Perform postprocessing tasks and set result from calculation
      * into the output adapters.
      *
      * Must be called from the main thread.
      * Assumes execute() has been called.
      *
      */
    virtual void postProcess() = 0;

    
};


/** Collection of filters.
  *
  * Connects them by giving them unique id's.
  *
  */
class FilterGroup
{
public:
    FilterGroup(ssc::XmlOptionFile options);
    /**
      * Get the option node for this pipeline
      */
    ssc::XmlOptionFile getOptions();
    /**
      * Get all filters in pipeline
      */
    std::vector<FilterPtr> getFilters() const;
    /**
      * Append a filter to group
      */
    void append(FilterPtr filter);

    size_t size() const { return mFilters.size(); }
    bool empty() const { return mFilters.empty(); }
//    FilterPtr& operator[](size_t index) { return mFilters[index]; }
//    const FilterPtr& operator[](size_t index) const { return mFilters[index]; }
    FilterPtr get(int index) { return mFilters[index]; }
    FilterPtr get(QString uid)
    {
        for (unsigned i=0; i<mFilters.size(); ++i)
            if (mFilters[i]->getUid()==uid)
                return mFilters[i];
        return FilterPtr();
    }

private:
    std::vector<FilterPtr> mFilters;
    ssc::XmlOptionFile mOptions;
};
typedef boost::shared_ptr<FilterGroup> FilterGroupPtr;


} // namespace cx

#endif // CXFILTER_H
