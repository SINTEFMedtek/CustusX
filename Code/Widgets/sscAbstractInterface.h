#ifndef SSCABSTRACTINTERFACE_H_
#define SSCABSTRACTINTERFACE_H_

#include <boost/shared_ptr.hpp>
#include <QApplication>

/// This follows the proposed DICOM standard for hosting services, see supplement 118
enum appStates
{
	APP_IDLE =		0,
	APP_INPROGRESS =	1,
	APP_SUSPENDED =		2,
	APP_COMPLETED =		3,
	APP_CANCELED =		4,
	APP_EXIT =		5
};

/** Abstract interface for interaction with internal double-valued data */
class DoubleDataInterface : public QObject
{
	Q_OBJECT

public:
	virtual ~DoubleDataInterface() {}
	virtual QString getValueID() const = 0;
	virtual QString getValueName() const = 0;
	virtual bool setValue(double value) = 0; ///< implemented by subclasses to set value for this entity
	virtual double getValue() const = 0; ///< implemented by subclasses to get value for this entity
	virtual DoubleRange getValueRange() const { return DoubleRange(0,1,0.01); } /// range of value
	virtual double convertInternal2Display(double internal) { return internal*100; } ///< conversion from internal value to 
display value
	virtual int getValueDecimals() const { return 0; } ///< number of relevant decimals in value
	virtual double legendStep() const {return 0.2;} ///< step between each pos on the legend
	virtual void connectValueSignals(bool on) = 0; ///< set object to emit changed() when applicable
signals:
	void changed();
};
typedef boost::shared_ptr<DoubleDataInterface> DoubleDataInterfacePtr;

/** Dummy implementation */
class DoubleDataInterfaceNull : public DoubleDataInterface
{
	Q_OBJECT

public:
	virtual ~DoubleDataInterfaceNull() {}
	virtual QString getValueID() const { return ""; }
	virtual QString getValueName() const { return "dummy"; }
	virtual bool setValue(double value) { return false; }
	virtual double getValue() const { return 0; }
	virtual void connectValueSignals(bool on) {}
};

#endif
