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
class QDomElement;

namespace cx
{

typedef boost::shared_ptr<class Filter> FilterPtr;

/** Base class for CustusX filters/algorithms
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
    /** Definition of one Filter input or output argument.
      */
    class ArgumentType
    {
    public:
        explicit ArgumentType(QString dataType) : mDataType(dataType) {}
        /**
          * mesh, image, data (corresponding to ssc::Data::getType())
          */
        QString mDataType;
        QString mName;
        QString mHelp;
    };

public:
    explicit Filter();
    virtual ~Filter() {}
    

    /**
      *  Return a unique string for this algorithm.
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
    virtual std::vector<ArgumentType> getInputTypes() = 0;
    /**
      *  Return a help text describing algorithm usage.
      */
    virtual std::vector<ArgumentType> getOutputTypes() = 0;

//    - setting of data must be in preprocessing pga threading issues
//    - update() cannot use vtk stuff in a nonconst way.
//    - postprocess saves to data manager.
//    - in priciple all this is impossible - vtk cannot be part of a chain if also available in another thread.

    /**
      * Set input parameters for algorithm and perform preprocessing tasks.
      * Must be called from the main thread.
      *
      * \param input List of data inputs corresponding to getInputTypes()
      * \param settings Reference to settings file containing algorithm-specific settings
      * \param outputPath is path to data files for current patient.
      * \return success.
      */
    virtual bool preProcess(std::vector<ssc::DataPtr> input, QDomElement options, QString outputPath) = 0;
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
      * Perform postprocessing tasks and return result from calculation.
      * Must be called from the main thread.
      * Assumes execute() has been called.
      *
      * \return List of data outputs corresponding to getOutputTypes()
      */
    virtual std::vector<ssc::DataPtr> postProcess() = 0;

    
};

} // namespace cx

#endif // CXFILTER_H
