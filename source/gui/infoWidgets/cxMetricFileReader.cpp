#include "cxMetricFileReader.h"
#include "cxStringHelpers.h"

#include <QTextStream>
#include <iostream>


namespace cx
{

MetricFileReader::MetricFileReader()
{

}

/**
 * @brief MetricFileReader::importMetrics
 * @param metricsFile
 *
 * Read a metrics file and create a vector with the parameters for each metric.
 */
std::vector<QStringList> MetricFileReader::readMetricFile(QFile& metricsFile) const
{
    QTextStream in(&metricsFile);
    std::vector<QStringList> metrics;

    while(!in.atEnd())
    {
        QString line = in.readLine();

        QStringList metricParameters = splitStringContaingQuotes(line);
		metrics.push_back(metricParameters);
    }

    return metrics;
}


} // namespace cx
