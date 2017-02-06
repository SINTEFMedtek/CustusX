#ifndef METRICFILEREADER_H
#define METRICFILEREADER_H

#include <QFile>


namespace cx
{

class MetricFileReader
{
public:
	MetricFileReader();
    void importMetrics(QFile &metricsFile) const;
};

} // namespace cx

#endif // METRICFILEREADER_H
