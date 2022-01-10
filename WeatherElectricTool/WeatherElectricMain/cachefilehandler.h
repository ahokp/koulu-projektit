/**
  * @file cachefilehandler.h declares the CacheFileHandler class, which is used for
  * saving and loading data sets and preferences from/to an xml file.
  * @author Paul Ahokas 283302 paul.ahokas@tuni.fi
  * @date 15.4.2021
  */

#ifndef CACHEFILEHANDLER_H
#define CACHEFILEHANDLER_H

#include "dataconnector.h"
#include <QXmlStreamWriter>
#include <QtXml>
#include <QFile>

/**
 * @brief The CacheFileHandler class saves/reads data sets and preferences to/from
 * xml files
 */
class CacheFileHandler
{
public:

    /**
     * @brief saveDataSet saves given data set into an xml file.
     * @param savedDataSet contains data of the data set.
     * @param fileName is the name of the file in which data will be saved.
     */
    static void saveDataSet(std::pair<std::pair<QDateTime, QDateTime>, std::vector<DataSet>> savedDataSet, QString fileName);

    /**
     * @brief readDataSet reads a data set and stores data so it can be used.
     * @param fileName is the name of the file from which data will be read.
     * @return the stored data set.
     */
    static std::pair<std::pair<QDateTime, QDateTime>, std::vector<DataSet>> readDataSet(QString fileName);

    /**
     * @brief savePreference saves given preference into an xml file.
     * @param savedPreference contains data of each given data source.
     * @param fileName is the name of the file in which data will be saved.
     */
    static void savePreference(std::vector<DataSourceDetails> savedPreference, QString fileName);

    /**
     * @brief loadPreference reads a preference and stores it so it can be used.
     * @param fileName is the name of the file from which data will be read.
     * @return the stored preference.
     */
    static std::vector<DataSourceDetails> loadPreference(QString fileName);

private:
    // Don't allow creating an instance of this object
    CacheFileHandler() {};
};

#endif // CACHEFILEHANDLER_H
