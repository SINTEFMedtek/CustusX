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
        //QStringList metricParameters = this->handleStringsInReadLine(line);
        metrics.push_back(metricParameters);
    }

    return metrics;
}

///**
// * @brief MetricFileReader::handleStringsInReadLine
// * @param stringWithStrings
// * @return A QStringList with the parameters of the metric on the read line.
// */
//QStringList MetricFileReader::handleStringsInReadLine(QString& stringWithStrings) const
//{
//    QStringList lineSplitAtSpace = stringWithStrings.split(" ");
//    QStringList mergedStrings = this->mergeAnyStringsInList(lineSplitAtSpace);

//    return mergedStrings;
//}

///**
// * @brief MetricFileReader::mergeStrings
// * @param list A list where the entries might start or end with a "
// * @return A QStringList where any strings in the input list have been merged
// *
// * The names of metrics and references can contain spaces and are therefore quoted as strings in the file.
// * Since the separator between fields is space we need to merge the strings back together after splitting
// * on space. E.g. "point 2" "reference 3" -> "point,2","reference,3" -> point 2,reference 3
// */
//QStringList MetricFileReader::mergeAnyStringsInList(QStringList& list) const
//{
//    QStringList finalList;

//    for(QStringList::Iterator it = list.begin(); it != list.end(); ++it)
//    {
//        QString current = *it;
//        if(current.at(0) == QString("\"")) //First part of a string
//        {
//            current.remove(0,1); //remove first " of the string
//            ++it;
//            while(it->at(it->size()-1) != QString("\"")) //while the words in the middle of the string
//            {
//                current += " ";      //add a space
//                current += *it;     //add the word
//                ++it;
//            }
//            current += " ";      //add a space
//            current += *it;      //add the final word of the string
//            current.remove(current.size()-1,1); //remove the ending " of the string

//        }
//        finalList.push_back(current);
//    }

//    return finalList;
//}

} // namespace cx
