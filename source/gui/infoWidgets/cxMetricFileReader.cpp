#include "cxMetricFileReader.h"

#include <QTextStream>
#include <iostream>


namespace cx
{

MetricFileReader::MetricFileReader()
{

}

void MetricFileReader::importMetrics(QFile& metricsFile) const
{
    QTextStream in(&metricsFile);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        QStringList metricProperties = line.split(" ");
        QString metricType = metricProperties.at(0);
        QString metricName = metricProperties.at(1);





        if(metricType == "pointMetric")
            std::cout << "found POINTMETRIC !!! \n";


    }

}

} // namespace cx
