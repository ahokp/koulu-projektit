/**
  * @file dataconnector.h declares the DataConnector class, which is used for making
  * data fetching calls, handling data and distributing it for the program.
  * @author Paul Ahokas 283302 paul.ahokas@tuni.fi
  * @date 10.4.2021
  */

#ifndef DATACONNECTOR_H
#define DATACONNECTOR_H

#include "DataImporting/fmidataimporter.hh"
#include "DataImporting/fingriddataimporter.hh"
#include "weathergraph.hh"
#include "weatherpie.hh"
#include "weatherbar.hh"
#include <QObject>
#include <iostream>
#include <string>
#include <map>
#include <QtCharts>
#include <algorithm>
#include <tuple>

/**
 * @brief The DataSourceDetails struct contains information of data type
 */
struct DataSourceDetails
{
    std::string dataSourceName;
    std::string dataLocationName;
    std::string graphName;
    int dataSourceIndex;
    DataImporting::ApiDataType dataType;
};

/**
 * @brief The DataSet struct contains a set of data that can be used for a setup.
 */
struct DataSet{
    int hide;
    DataSourceDetails dataSource;
    DataSeries series;
};

// Defining == for DataSourceDetails so that it can be compared with others
inline bool operator==(DataSourceDetails d1, DataSourceDetails d2) {
    return d1.dataSourceName == d2.dataSourceName && d1.graphName == d2.graphName; }
inline bool operator!=(DataSourceDetails d1, DataSourceDetails d2) { return !(d1==d2); }

// Defining < for DataSourceDetails so it can be used as a key for std::map
inline bool operator<(DataSourceDetails d1, DataSourceDetails d2)
{
    return(std::tie(d2.graphName,d2.dataSourceName) < std::tie(d1.graphName,d1.dataSourceName));
}

/**
 * @brief The DataConnector class handles data, it gets data from DataImporter classes and relays
 * information to the rest of the program. It can also use data given to it(from a loaded file).
 */
class DataConnector : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief DataConnector
     * @param parent
     */
    explicit DataConnector(QObject *parent = nullptr);

    /**
     * @brief ~DataConnector
     */
    virtual ~DataConnector();

    /**
     * @brief getAllDataSourceDetails gets details of all available data sources so the program
     * knows what kind of weather/electricity information can be fetched
     * @return a vector containing information of each kind of weather/electricity data type
     */
    std::vector<DataSourceDetails> getAllDataSourceDetails();

    /**
     * @brief getData gives fetched data of each requested data type
     * @return a pair containing start DateTime of data and a map containing data type and a series
     * of data points
     */
    std::pair<QDateTime, std::map<std::string, DataSeries>> getData();

    /**
     * @brief addActiveDataSource adds requested data type to be activated
     * @param dataSource contains details of data type to be activated
     */
    void addActiveDataSource(DataSourceDetails dataSource);

    /**
     * @brief removeActiveDataSource deactivates requested data type
     * @param dataSource contains details of data type to be deactivated
     */
    void removeActiveDataSource(DataSourceDetails dataSource);

    /**
     * @brief getStartDate returns currently set starting DateTime
     * @return starting DateTime of current time interval
     */
    QDateTime getStartDate();

    /**
     * @brief getEndDate returns currently set end DateTime
     * @return end DateTime of current time interval
     */
    QDateTime getEndDate();

    /**
     * @brief setBoundaryDates sets new DateTime boundaries and reuses/fetches more data for each
     * active data type depending on if enough data is already available. If DateTime boundaries
     * change completely, delete old data and fetch data from new time interval.
     * @param newStartDate is the new starting DateTime.
     * @param newEndDate is the new end DateTime.
     */
    void setBoundaryDates(QDateTime newStartDate, QDateTime newEndDate);

    /**
     * @brief calcMagnitude calculates the magnitude of given series
     * @param series contains points from which magnitude is calculated
     * @return magnitude of the series
     */
    float calcMagnitude(QSplineSeries* series);

    /**
     * @brief saveCurrentDataSets saves current data into DataSets.
     * @param fileName is the name of the file in which DataSets will be saved.
     */
    void saveCurrentDataSets(QString fileName);

    /**
     * @brief loadDataSets loads DataSets of given file.
     * @param fileName is the file from which DataSets are loaded.
     */
    void loadDataSets(QString fileName);

    /**
     * @brief savePreference saves current data sources as a preference.
     * @param fileName is the name of the file in which preference will be saved.
     */
    void savePreference(QString fileName);

    /**
     * @brief loadPreference loads a saved preference.
     * @param fileName is the name of the file which will be loaded.
     */
    void loadPreference(QString fileName);

    /**
     * @brief getActiveDataSources returns currently active data sources.
     * @return currently active data sources.
     */
    std::vector<DataSourceDetails> getActiveDataSources();

    /**
     * @brief clearAllActiveData clears all currently active data.
     */
    void clearAllActiveData();

signals:
    /**
     * @brief data_saved is a signal that is emitted when recieving data and saving it is complete
     */
    void data_saved();

    /**
     * @brief addToSeries is a signal to add more data points to an active series.
     * @param dataSeriesName is the name of the series which will recieve more data points.
     * @param addedSeries contains a series of points which will be added and information of them.
     * @param addBefore tells if series of points will be added to the front or back of current series.
     */
    void addToSeries(std::string dataSeriesName, DataSeries addedSeries, bool addBefore);

    /**
     * @brief delFromSeries is a signal to for deleting data points from an active series.
     * @param dataSeriesName is the name of the series from which data points will be deleted.
     * @param delIndex decides the index where point deletion starts/ends.
     * @param delBefore indicates if point deletion starts or ends at delIndex.
     */
    void delFromSeries(std::string dataSeriesName, int delIndex, bool delBefore);

    /**
     * @brief updateCharts is a signal for updating charts(pie and bar currently).
     * @param dataSeriesName is the name of the pie chart that need to be updated.
     * @param newMagnitude is the new magnitude.
     */
    void updateCharts(std::string dataSeriesName, float newMagnitude, float newMaxY, float newMinY);

    /**
     * @brief reAddSeries is a signal to remove a series so it can be added back afterwards.
     * @param dataSource contains information of data type which will be re-added.
     */
    void reAddSeries(DataSourceDetails dataSource);

    /**
     * @brief addSourceWidget is a signal for adding source widget boxes
     * @param dataSource is the data source of which box will be made
     */
    void addSourceWidget(DataSourceDetails dataSource);

private slots:
    /**
     * @brief save_data recieves fetched data and saves it into a map.
     * @param fetchDetails: The details of the data retrieved.
     * @param data: The data in a vector.
     */
    void save_data(DataImporting::DataFetchDetails fetchDetails,
                   std::shared_ptr<std::vector<DataImporting::DataPoint>> data);

private:

    /**
     * @brief reAddActiveDataSource adds requested data type to be activated. Re-uses data if possible
     * and fetches more data if needed. Removes old data if it's not usable.
     * @param dataSource contains information of data type which will be re-activated.
     */
    void reAddActiveDataSource(DataSourceDetails dataSource);

    /**
     * @brief makeDataPoints copies QSplineSeries values into a vector of DataPoints and
     * updates max and min values if needed.
     * @param series is the QSplineSeries which is copied.
     * @param maxY is the highest Y value which gets updated if needed.
     * @param minY is the smallest Y value which gets updated if needed.
     * @return the vector of DataPoints.
     */
    std::vector<DataImporting::DataPoint> makeDataPoints(QSplineSeries* series, float& maxY, float& minY);

    /**
     * @brief makeSeries makes a series from given data between current time interval and
     * updates max and min values if needed.
     * @param data_vec is a vector of data points which is converted into a series.
     * @param maxY is the highest Y value which gets updated if needed.
     * @param minY is the smallest Y value which gets updated if needed.
     * @return a series of data points from current time interval.
     */
    QSplineSeries* makeSeries(std::vector<DataImporting::DataPoint> data_vec, float& maxY, float& minY);

    /**
     * @brief makeSnippetSeries makes a series between given datetime and start/end datetime.
     * @param data_vec is a vector containing data points which are used to make a series.
     * @param oldDateTime is a date time which determines starting/ending date time for series creation.
     * @param isStartDate tells if oldDateTime is the starting/ending date time for series creation.
     * @return the created series, a pair containing the highest and lowest values of the series.
     */
    std::pair<QSplineSeries*, std::pair<float, float>> makeSnippetSeries
    (std::vector<DataImporting::DataPoint> data_vec, QDateTime oldDateTime, bool isStartDate);

    /**
     * @brief addToActiveSeries adds more data points to an active series.
     * @param seriesName is the data series which will recieve more data points.
     * @param dataSeries contains a series of data points and information of them.
     * @param addBefore tells if series of points will be added to the front or back of current series.
     * @return true if adding data was successful, false otherwise.
     */
    bool addToActiveSeries(std::string seriesName, DataSeries dataSeries, bool addBefore);

    /**
     * @brief removeFromActiveSeries determines the index for data point removal.
     * @param old_ser is the old series.
     * @param new_ser is the new series.
     * @param dataType tells the data type of both series'.
     */
    void removeFromActiveSeries(QSplineSeries *old_ser, QSplineSeries* new_ser, std::string dataType);

    // Stores instances of each DataImporter
    std::vector<DataImporting::DataImporter*> dataImporters_;

    // Keeps track of active data
    std::map<std::string, DataSeries> data_;

    // Keeps track of active data sources
    std::vector<DataSourceDetails> activeDataSources_;

    // Keeps track of data that is within or close to the current time interval.
    std::map<DataSourceDetails, std::pair<DataImporting::DataFetchDetails,
                                std::vector<DataImporting::DataPoint> >> allData_;

    // Current time interval.
    QDateTime startDateTime_;
    QDateTime endDateTime_;
};

#endif // DATACONNECTOR_H
