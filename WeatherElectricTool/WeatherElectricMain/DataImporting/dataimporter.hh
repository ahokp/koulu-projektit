/**
  * @file dataimporter.hh declares the DataImporter class, the ApiDataType enum
  * and the DataPoint struct, which are used as core tools for handling API
  * data.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 10.4.2021
  */

#ifndef DATAIMPORTER_HH
#define DATAIMPORTER_HH

#include <QDateTime>
#include <QObject>

#include <memory>

namespace DataImporting
{

/**
 * @brief The ApiDataType enum is used to sort different data types retrieved
 * from REST APIs.
 */
enum ApiDataType : int
{
    // These are all powers of two so that in binary they correspond to only
    // one bit each on the underlying int and bitwise operators can be used on
    // them
    None = 0,
    Temperature = 1,
    WindSpeed = 2,
    CloudAmount = 4,
    PredictedTemperature = 8,
    PredictedWindSpeed = 16,
    PredictedCloudAmount = 32,
    AverageTemperature = 64,
    AverageMinTemperature = 128,
    AverageMaxTemperature = 256,
    ElectricityConsumption = 512,
    ElectricityProduction = 1024,
    NuclearPowerProduction = 2048,
    HydroPowerProduction = 4096,
    WindPowerProduction = 8192,
    PredictedElectricityConsumption = 16384,
    PredictedElectricityProduction = 32768,
    PredictedNuclearPowerProduction = 65536,
    PredictedHydroPowerProduction = 131072,
    PredictedWindPowerProduction = 262144
};

// Allow combining data type enums with the | operator (bitwise OR)
ApiDataType operator|(ApiDataType a, ApiDataType b);
// Allow combining data type enums with the & operator (bitwise AND)
ApiDataType operator&(ApiDataType a, ApiDataType b);
// Allow combining data type enums with the ~ operator (bitwise NOT)
ApiDataType operator~(ApiDataType a);

/**
 * @brief getDataTypeName returns a string of the full name of the given
 * ApiDataType.
 * @param dataType: The data type the name of which to return
 * @return The name of the data type
 */
std::string getDataTypeName(const ApiDataType& dataType);

/**
 * @brief getDataTypeFromName returns the ApiDataType that matches the
 * given name string.
 * @param dataTypeName: The name of the data type as a string
 * @return ApiDataType that matches the given name string,
 * ApiDataType::None if no matching type found
 */
ApiDataType getDataTypeFromName(const std::string& dataTypeName);

/**
 * @brief The DataPoint struct represents a data point with a data value and a
 * time and date from when it was taken.
 */
struct DataPoint
{
    QDateTime dateTime;
    float value;
};

class DataImporter;

/**
 * @brief The DataFetchDetails struct stores details of the data returned in a
 * dataFetched signal.
 */
struct DataFetchDetails
{
    ApiDataType dataType;
    std::string dataLocation;
    std::string unitOfMeasurement;
    DataImporter* importer;
    QDateTime startDateTime;
    QDateTime endDateTime;
    float maxValue;
    float minValue;
};

/**
 * @brief The DataImporter class is an abstract base class that every data
 * importer in the program inherits from.
 */
class DataImporter : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief fetchData fetches data of the given type from a REST API.
     * Abstract method: should be implemented by inheriting class.
     * @param dataType: The type of data to fetch
     * @param startTime: The start of the time period to fetch data from
     * @param endTime: The end of the time period to fetch data from
     * @param location: The physical location or area to fetch data from
     */
    virtual void fetchData(const ApiDataType& dataType,
        const QDateTime& startTime, const QDateTime& endTime,
        const std::string& location) = 0;

    /**
     * @brief getAvailableDataTypes returns all the data types this
     * DataImporter is able to fetch with fetchData.
     * @return A vector containing an enum for each data type this DataImporter
     * can fetch
     */
    virtual std::vector<ApiDataType> getAvailableDataTypes() = 0;

    /**
     * @brief getAvailableLocations returns all the locations this DataImporter
     * is able to fetch data from with fetchData.
     * @return A vector containing the name of each location this DataImporter
     * can fetch data from
     */
    virtual std::vector<std::string> getAvailableLocations() = 0;

    /**
     * @brief canFetchDataType checks whether this DataImporter can fetch the
     * given type of data.
     * @param dataType: The type of data to check for
     * @return True if this DataImporter can fetch this type of data, otherwise
     * false
     */
    virtual bool canFetchDataType(const ApiDataType& dataType) = 0;

    /**
     * @brief getSourceName returns the name of the source (website, company,
     * organization etc.) this DataImporter fetches data from.
     * @return The name of the source this DataImporter fetches data from
     */
    virtual std::string getSourceName() = 0;

signals:
    /**
     * @brief dataFetched is a signal that is emitted when the DataImporter
     * has successfully fetched data from a REST API.
     * @param fetchDetails: Details about the fetched data
     * @param data: The fetched data in a vector
     */
    void dataFetched(DataFetchDetails fetchDetails,
        std::shared_ptr<std::vector<DataPoint>> data) const;

protected:
    // Stores how many ApiDataTypes are defined (remember to update this if
    // adding new data types)
    static const int API_DATA_TYPE_COUNT = 19;

    /**
     * @brief The default constructor.
     * @param parent: The QObject to parent this DataImporter to
     */
    DataImporter(QObject* parent);

    /**
     * @brief separateDataTypes separates individual API data types from an
     * ApiDataType enum that contains multiple data types.
     * @param dataType: The ApiDataType to separate
     * @return The separated data types in a vector
     */
    static std::vector<ApiDataType> separateDataTypes(
        const ApiDataType& dataType);
};

}

#endif // DATAIMPORTER_HH
