/**
  * @file dataconnector.cpp implements the DataConnector class
  * @author Paul Ahokas 283302 paul.ahokas@tuni.fi
  * @date 15.4.2021
  */

#include "dataconnector.h"
#include "cachefilehandler.h"

DataConnector::DataConnector(QObject* parent)
    : QObject(parent)
{
    dataImporters_ = {new DataImporting::FmiDataImporter,
                      new DataImporting::FingridDataImporter};

    for(auto dataImporter : dataImporters_)
    {
        connect(dataImporter, &DataImporting::DataImporter::dataFetched,
                this, &DataConnector::save_data);
    }

    // Set default time period to be the from past week
    startDateTime_ = QDateTime::currentDateTime().addDays(-7);
    endDateTime_ = QDateTime::currentDateTime();
}

DataConnector::~DataConnector()
{
}

std::vector<DataSourceDetails> DataConnector::getAllDataSourceDetails()
{
    std::vector<DataSourceDetails> dataSD;
    int dataSourceIndex = 0;

    for(DataImporting::DataImporter* dataImporter : dataImporters_)
    {
        std::vector<std::string> dataLocations = dataImporter->getAvailableLocations();

        for(DataImporting::ApiDataType dataType: dataImporter->getAvailableDataTypes())
        {
            for(std::string dataLocation: dataLocations)
            {
                DataSourceDetails dsd = {dataImporter->getSourceName(),
                                         dataLocation,
                                         DataImporting::getDataTypeName(dataType)
                                         + ", " + dataLocation,
                                         dataSourceIndex,
                                         dataType};
                dataSD.push_back(dsd);
            }
        }
        dataSourceIndex++;
    }
    return dataSD;
}

std::pair<QDateTime, std::map<std::string, DataSeries>> DataConnector::getData()
{
    std::pair<QDateTime, std::map<std::string, DataSeries>> weatherData = {startDateTime_, data_};
    return weatherData;
}

void DataConnector::addActiveDataSource(DataSourceDetails dataSource)
{
    DataImporting::ApiDataType DataType = dataSource.dataType;
    auto it = allData_.find(dataSource);

    // Some data of this data source exists
    if(it != allData_.end())
    {
        activeDataSources_.push_back(dataSource);
        reAddActiveDataSource(dataSource);
    }
    else
    {
        for(auto dataImporter : dataImporters_)
        {
            if(dataImporter->canFetchDataType(DataType))
            {
                activeDataSources_.push_back(dataSource);
                dataImporter->fetchData(DataType,
                                        startDateTime_,
                                        endDateTime_,
                                        dataSource.dataLocationName);
            }
        }
    }
}

void DataConnector::reAddActiveDataSource(DataSourceDetails dataSource)
{
    auto it = allData_.find(dataSource);
    auto importer = it->second.first.importer;
    auto old_data = it->second.second;

    // Start and end dates of old data's time interval
    QDateTime startDate = it->second.first.startDateTime;
    QDateTime endDate = it->second.first.endDateTime;

    // If time interval changed completely, erase previous data
    if(startDateTime_ >= endDate || endDateTime_ <= startDate)
    {
        allData_.erase(it);
        auto DataSource = std::find(activeDataSources_.begin(), activeDataSources_.end(), dataSource);
        activeDataSources_.erase(DataSource);

        addActiveDataSource(dataSource);
    }
    // Get more data to the heads of previous data if needed
    else
    {
        // Fetch new data from the front and end of old data
        if(startDate > startDateTime_ && endDate < endDateTime_)
        {
            importer->fetchData(dataSource.dataType,
                                startDateTime_,
                                startDate,
                                dataSource.dataLocationName);
            importer->fetchData(dataSource.dataType,
                                endDate,
                                endDateTime_,
                                dataSource.dataLocationName);
        }
        // Fetch new data from the front of old data
        else if(startDate > startDateTime_)
        {
            importer->fetchData(dataSource.dataType,
                                startDateTime_,
                                startDate,
                                dataSource.dataLocationName);
        }
        // Fetch new data from the end of old data
         else if(endDate < endDateTime_)
        {
            importer->fetchData(dataSource.dataType,
                                endDate,
                                endDateTime_,
                                dataSource.dataLocationName);
        }
        // Data from new boundaries already exist
        else
        {
            float maxY = it->second.first.maxValue;
            float minY = it->second.first.minValue;
            QSplineSeries* series = makeSeries(old_data, maxY, minY);

            float magnitude = calcMagnitude(series);
            DataSeries dataSeries = {it->second.first.unitOfMeasurement,
                                     series,
                                     nullptr,
                                     maxY,
                                     minY,
                                     magnitude};
            data_.insert({it->first.graphName, dataSeries});
            emit data_saved();
        }
    }
}



void DataConnector::removeActiveDataSource(DataSourceDetails dataSource)
{
    auto DataSource = std::find(activeDataSources_.begin(), activeDataSources_.end(), dataSource);
    activeDataSources_.erase(DataSource);

    auto dataIter = data_.find(dataSource.graphName);
    data_.erase(dataIter);
}

QDateTime DataConnector::getStartDate()
{
    return startDateTime_;
}

QDateTime DataConnector::getEndDate()
{
    return endDateTime_;
}

void DataConnector::setBoundaryDates(QDateTime newStartDate, QDateTime newEndDate)
{
    QDateTime oldEndDate = endDateTime_;
    QDateTime oldStartDate = startDateTime_;

    endDateTime_ = newEndDate;
    startDateTime_ = newStartDate;

    std::vector<DataSourceDetails>::iterator activeDS_iter = activeDataSources_.begin();
    while(activeDS_iter != activeDataSources_.end())
    {
        auto it = allData_.find(*activeDS_iter);

        if(it == allData_.end()){
            break;
        }

        // Delete old data if new time boundary isn't within the old one
        if(newStartDate >= oldEndDate || newEndDate <= oldStartDate)
        {
            DataSourceDetails dSource = it->first;
            allData_.erase(it);
            removeActiveDataSource(dSource);

            emit reAddSeries(dSource);
            addActiveDataSource(dSource);
        }
        // Modify the heads of current old series
        else
        {
            auto importer = it->second.first.importer;
            auto old_data = it->second.second;

            auto oldDataIter = data_.find(it->first.graphName);



            // Start and end dates of old data's time interval
            QDateTime startDate = it->second.first.startDateTime;
            QDateTime endDate = it->second.first.endDateTime;

            // Remove points from the active series if needed
            if(oldEndDate > newEndDate || oldStartDate < newStartDate)
            {
                float newMaxY = it->second.first.maxValue;
                float newMinY = it->second.first.minValue;
                QSplineSeries* new_ser = makeSeries(old_data, newMaxY, newMinY);

                removeFromActiveSeries(oldDataIter->second.splineSeries, new_ser,
                                       it->first.graphName);

                data_.erase(oldDataIter);
                float newMagnitude = calcMagnitude(new_ser);
                DataSeries dataSeries = {it->second.first.unitOfMeasurement,
                                         new_ser,
                                         nullptr,
                                         newMaxY,
                                         newMinY,
                                         newMagnitude};
                data_.insert({it->first.graphName, dataSeries});

                emit updateCharts(activeDS_iter->graphName, newMagnitude, newMaxY, newMinY);
            }
            // Time boundary widened from start
            if(oldStartDate > newStartDate)
            {
                // Add data to the start of active series if it already exists
                if(oldStartDate > startDate)
                {
                    std::pair<QSplineSeries*, std::pair<float, float>> start_SPser =
                            makeSnippetSeries(old_data, oldStartDate, true);
                    float magnitude = calcMagnitude(start_SPser.first);
                    DataSeries start_ser = {it->second.first.unitOfMeasurement,
                                            start_SPser.first,
                                            nullptr,
                                            start_SPser.second.first,
                                            start_SPser.second.second,
                                            magnitude};

                    addToActiveSeries(it->first.graphName, start_ser, true);
                }
                // Fetch new data from the front of old data if needed
                if(startDate > newStartDate)
                {
                    importer->fetchData(activeDS_iter->dataType,
                                        newStartDate,
                                        startDate,
                                        activeDS_iter->dataLocationName);
                }
            }
            // Time boundary widened from end
            if(oldEndDate < newEndDate)
            {
                // Add data to the end of active series of it already exists
                if(oldEndDate < endDate)
                {
                    std::pair<QSplineSeries*, std::pair<float, float>> end_SPser =
                            makeSnippetSeries(old_data, oldEndDate, false);
                    float magnitude = calcMagnitude(end_SPser.first);
                    DataSeries end_ser = {it->second.first.unitOfMeasurement,
                                          end_SPser.first,
                                          nullptr,
                                          end_SPser.second.first,
                                          end_SPser.second.second,
                                          magnitude};

                    addToActiveSeries(it->first.graphName, end_ser, false);
                }
                // Fetch new data from the end of old data if needed
                if(endDate < newEndDate){
                    importer->fetchData(activeDS_iter->dataType,
                                        endDate,
                                        newEndDate,
                                        activeDS_iter->dataLocationName);
                }
            }
            activeDS_iter++;
        }
    }
}

float DataConnector::calcMagnitude(QSplineSeries *series)
{
    float magnitude = 0;
    for(int i = 0; series->count() > i; i++){
        magnitude += series->at(i).y();
    }
    return magnitude;
}

void DataConnector::saveCurrentDataSets(QString fileName)
{

    std::vector<DataSet> dataSets = {};
    for(DataSourceDetails dsd : activeDataSources_)
    {
        auto it = data_.find(dsd.graphName);
        DataSet dSet = {0,
                        dsd,
                        it->second};
        dataSets.push_back(dSet);
    }
    CacheFileHandler::saveDataSet({{startDateTime_, endDateTime_},  dataSets}, fileName);
}

void DataConnector::loadDataSets(QString fileName)
{
    std::pair<std::pair<QDateTime, QDateTime>, std::vector<DataSet>> data =
                                    CacheFileHandler::readDataSet(fileName);

    std::vector<DataSet> dataSets = data.second;
    if(dataSets.size() != 0)
    {
        clearAllActiveData();
        startDateTime_ = data.first.first;
        endDateTime_ = data.first.second;

        for(DataSet dataSet : dataSets)
        {
            DataSourceDetails dataSource = dataSet.dataSource;
            DataSeries dataSeries = dataSet.series;

            auto it = allData_.find(dataSource);
            if(it != allData_.end()){
                allData_.erase(it);
            }
            float curMaxY = dataSet.series.maxY;
            float curMinY = dataSet.series.minY;
            std::vector<DataImporting::DataPoint> dataPoints = makeDataPoints(dataSet.series.splineSeries, curMaxY, curMinY);

            DataImporting::DataImporter* importer = dataImporters_.at(dataSource.dataSourceIndex);

            DataImporting::DataFetchDetails fetchDetails = {dataSource.dataType,
                                                            dataSource.dataLocationName,
                                                            dataSeries.unitOfMeasurement,
                                                            importer,
                                                            startDateTime_,
                                                            endDateTime_,
                                                            curMaxY,
                                                            curMinY};

            allData_.insert({dataSource, {fetchDetails, dataPoints}});
            emit addSourceWidget(dataSource);
        }
    }
}

void DataConnector::savePreference(QString fileName)
{
    CacheFileHandler::savePreference(activeDataSources_, fileName);
}

void DataConnector::loadPreference(QString fileName)
{
    std::vector<DataSourceDetails> dataSources = CacheFileHandler::loadPreference(fileName);

    if(dataSources.size() != 0)
    {
        clearAllActiveData();
        for(DataSourceDetails dataSource : dataSources){
            emit addSourceWidget(dataSource);
        }
    }
}

std::vector<DataSourceDetails> DataConnector::getActiveDataSources()
{
    return activeDataSources_;
}

std::vector<DataImporting::DataPoint> DataConnector::makeDataPoints(QSplineSeries *series, float& maxY, float& minY)
{
    std::vector<DataImporting::DataPoint> dataPoints = {};

    for(int i = 0; series->count() > i; i++){
        float value = series->at(i).y();
        DataImporting::DataPoint dataPoint = {QDateTime::fromSecsSinceEpoch(series->at(i).x()), value};
        dataPoints.push_back(dataPoint);

        if(maxY < value){
            maxY = value;
        }
        if(minY > value){
            minY = value;
        }
    }
    return dataPoints;
}

QSplineSeries* DataConnector::makeSeries(std::vector<DataImporting::DataPoint> data_vec, float& maxY, float& minY)
{
    QSplineSeries* series = new QSplineSeries;

    // Transfer data to QSplineSeries
    for(auto dataPoint : data_vec)
    {
        // Data is within requested time interval
        if(dataPoint.dateTime > startDateTime_ && dataPoint.dateTime < endDateTime_)
        {
            series->append(QPointF(dataPoint.dateTime.toSecsSinceEpoch(), dataPoint.value));
            if(maxY < dataPoint.value){
                maxY = dataPoint.value;
            }
            if(minY > dataPoint.value){
                minY = dataPoint.value;
            }
        }
        if(dataPoint.dateTime > endDateTime_)
        {
            break;
        }
    }
    return series;
}

std::pair<QSplineSeries*, std::pair<float, float>>DataConnector::makeSnippetSeries
(std::vector<DataImporting::DataPoint> data_vec, QDateTime oldDateTime, bool isStartDate)
{
    QSplineSeries* series = new QSplineSeries;
    float maxVal = INT_MIN;
    float minVal = INT_MAX;
    if(isStartDate)
    {
        for(auto dataPoint : data_vec)
        {
            if(dataPoint.dateTime < oldDateTime && dataPoint.dateTime >= startDateTime_)
            {
                series->append(QPointF(dataPoint.dateTime.toSecsSinceEpoch(), dataPoint.value));
                if(maxVal < dataPoint.value){
                    maxVal = dataPoint.value;
                }
                if(minVal > dataPoint.value){
                    minVal = dataPoint.value;
                }
            }
            if(dataPoint.dateTime > oldDateTime)
            {
                break;
            }
        }
    }
    else
    {
        for(auto dataPoint : data_vec)
        {
            if(dataPoint.dateTime > endDateTime_){
                break;
            }
            if(dataPoint.dateTime > oldDateTime)
            {
                series->append(QPointF(dataPoint.dateTime.toSecsSinceEpoch(), dataPoint.value));
                if(maxVal < dataPoint.value){
                    maxVal = dataPoint.value;
                }
                if(minVal > dataPoint.value){
                    minVal = dataPoint.value;
                }
            }
        }
    }
    return {series, {maxVal, minVal}};
}

bool DataConnector::addToActiveSeries(std::string seriesName, DataSeries dataSeries, bool addBefore)
{
    auto data_iter = data_.find(seriesName);
    // Data type is active
    if(data_iter != data_.end())
    {
        emit addToSeries(seriesName, dataSeries, addBefore);
        return true;
    }
    return false;
}

void DataConnector::removeFromActiveSeries(QSplineSeries *old_ser, QSplineSeries *new_ser, std::string dataType)
{
    int delBeforeIndex = 0;
    for(int i = 1; old_ser->count() > i; i++)
    {
        // Delete points from start to index
        if(old_ser->at(i) == new_ser->at(0))
        {
            delBeforeIndex = i;
            emit delFromSeries(dataType, delBeforeIndex, true);
        }
        // Delete points from index to end
        if(old_ser->at(i) == (new_ser->at(new_ser->count()-1)))
        {
            int delAfterIndex = i - delBeforeIndex;
            emit delFromSeries(dataType, delAfterIndex, false);
            break;
        }
    }
}

void DataConnector::clearAllActiveData()
{
    activeDataSources_.clear();
    data_.clear();
}

void DataConnector::save_data(DataImporting::DataFetchDetails fetchDetails,
                              std::shared_ptr<std::vector<DataImporting::DataPoint>> data)
{
    DataSourceDetails fetchedDSD = { fetchDetails.importer->getSourceName(),
                                     fetchDetails.dataLocation,
                                     DataImporting::getDataTypeName(
                                        fetchDetails.dataType) + ", "
                                     + fetchDetails.dataLocation, 0,
                                     fetchDetails.dataType };

    auto it = allData_.find(fetchedDSD);
    std::vector<DataImporting::DataPoint>& old_data = it->second.second;
    QSplineSeries* series = new QSplineSeries();
    float maxY = fetchDetails.maxValue;
    float minY = fetchDetails.minValue;

    bool addDataBeforeSeries = false;

    // No earlier data of this type exists
    if(it == allData_.end())
    {
        allData_.insert({fetchedDSD, {fetchDetails, *data}});
        series = makeSeries(*data, maxY, minY);

        startDateTime_ = fetchDetails.startDateTime;
        endDateTime_ = fetchDetails.endDateTime;
    }
    // New fetched data is added to the end of old data
    else if(it->second.first.endDateTime == fetchDetails.startDateTime)
    {
        old_data.insert(old_data.end(), data->begin(),data->end());
        it->second.first.endDateTime= fetchDetails.endDateTime;
        series = makeSeries(old_data, maxY, minY);

        addDataBeforeSeries = false;
    }
    // New fetched data is added to the front of old data
    else if(it->second.first.startDateTime == fetchDetails.endDateTime)
    {
        // Reversing vectors so data can be combined
        std::reverse(old_data.begin(), old_data.end());
        std::reverse(data->begin(), data->end());
        old_data.insert(old_data.end(), data->begin(),data->end());

        // Reverse to get values in back correct order
        std::reverse(old_data.begin(), old_data.end());
        std::reverse(data->begin(), data->end());

        it->second.first.startDateTime = fetchDetails.startDateTime;
        series = makeSeries(old_data, maxY, minY);

        addDataBeforeSeries = true;
    }

    QSplineSeries* addedSeries = makeSeries(*data, maxY, minY);
    float addedMagnitude = calcMagnitude(addedSeries);

    // DataSeries of data to be added (if needed) to active data
    DataSeries addedDataSeries = {fetchDetails.unitOfMeasurement,
                                  addedSeries,
                                  nullptr,
                                  maxY,
                                  minY,
                                  addedMagnitude};
    bool isSeriesModified = addToActiveSeries(fetchedDSD.graphName,
                                              addedDataSeries, addDataBeforeSeries);

    auto dataIter = data_.find(fetchedDSD.graphName);
    // Old data needs to be deleted from data_ if it exists
    if(dataIter != data_.end()){
        data_.erase(dataIter);
    }

    float magnitude = calcMagnitude(series);
    // DataSeries of data that will be activated
    DataSeries dataSeries = {fetchDetails.unitOfMeasurement,
                             series,
                             nullptr,
                             maxY,
                             minY,
                             magnitude};
    data_.insert({fetchedDSD.graphName, dataSeries});

    if(!isSeriesModified){
        emit data_saved();
    }
    else{
        emit updateCharts(fetchedDSD.graphName, magnitude, maxY, minY);
    }
}
