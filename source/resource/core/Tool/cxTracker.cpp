#include "cxTracker.h"

#include "cxDefinitions.h"
#include "cxEnumConversion.h"

namespace cx
{

Tracker::Tracker(QObject *parent) :
    QObject(parent)
{
}

QStringList Tracker::getSupportedTrackingSystems()
{
    QStringList retval;
    retval << enum2string(tsPOLARIS);
    retval << enum2string(tsAURORA);
    return retval;
}
} //namespace cx
