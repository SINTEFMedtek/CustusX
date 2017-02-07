#ifndef METRICFILEREADER_H
#define METRICFILEREADER_H

#include <QFile>


namespace cx
{

/**
 * @brief The MetricFileReader class
 *
 * Class which reads the exported metrics file
 */
class MetricFileReader
{
public:
	MetricFileReader();

    std::vector<QStringList> readMetricFile(QFile &metricsFile) const;
    QStringList handleStringsInReadLine(QString& stringWithStrings) const;
    QStringList mergeAnyStringsInList(QStringList& list) const;
};

} // namespace cx

#endif // METRICFILEREADER_H
