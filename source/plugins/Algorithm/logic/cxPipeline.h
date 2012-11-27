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
#ifndef CXPIPELINE_H
#define CXPIPELINE_H

#include <QObject>
#include "cxFilter.h"
#include "sscXmlOptionItem.h"

namespace cx
{

typedef boost::shared_ptr<class FusedInputOutputSelectDataStringDataAdapter> FusedInputOutputSelectDataStringDataAdapterPtr;

/** Data-DataAdapter that fuses two Data-DataAdapters.
  *
  * There are two arguments: input and output.
  * The output is wrapped by this adapter,
  * changes in the input will set the output.
  *
  *
  */
class FusedInputOutputSelectDataStringDataAdapter : public SelectDataStringDataAdapterBase
{
  Q_OBJECT
public:
    virtual ~FusedInputOutputSelectDataStringDataAdapter() {}
    static FusedInputOutputSelectDataStringDataAdapterPtr create(SelectDataStringDataAdapterBasePtr base, SelectDataStringDataAdapterBasePtr input);

public: // basic methods
    virtual QString getValueName() const;
    virtual bool setValue(const QString& value);
    virtual QString getValue() const;

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
  virtual QString getHelp() const;

public: // interface extension
  virtual ssc::DataPtr getData() const;
  virtual void setValueName(const QString name);
  virtual void setHelp(QString text);

protected slots:
    void inputDataChangedSlot();
protected:
    FusedInputOutputSelectDataStringDataAdapter(SelectDataStringDataAdapterBasePtr base, SelectDataStringDataAdapterBasePtr input);
    SelectDataStringDataAdapterBasePtr mInput;
    SelectDataStringDataAdapterBasePtr mBase;
};



/** Sequential execution of Filters.
 *
 * \ingroup cxPluginAlgorithms
 * \date Nov 22, 2012
 * \author christiana
 * \author Janne Beate Bakeng, SINTEF
 */
class Pipeline : public QObject
{
    Q_OBJECT
public:
    explicit Pipeline(QObject *parent = 0);
    /**
      * Initialize pipeline. Do once before use.
      */
    void initialize(FilterGroupPtr filter);
    /**
      * Get all filters in pipeline
      */
    FilterGroupPtr getFilters() const;
    /**
      * Get all nodes. If there are N filters, there are N+1 nodes.
      * Node N are input to filter N, and have node N+1 as output.
      *
      * Nodes are a fusion of output/input of filters in the pipeline.
      * Setting of an output will autoset the input of the next filter
      * through this mechanism.
      */
    std::vector<SelectDataStringDataAdapterBasePtr> getNodes();
    /**
      * Execute the filter at filterIndex. Recursively execute
      * all filters earlier in the pipeline if they dont have
      * an output value.
      */
    void execute(int filterIndex);

signals:
    
public slots:
    
private:
    FilterGroupPtr mFilters;

};
typedef boost::shared_ptr<Pipeline> PipelinePtr;

} // namespace cx

#endif // CXPIPELINE_H
