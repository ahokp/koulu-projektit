/**
  * @file fmidataimporter.hh declares the FmiDataImporter class, which is used
  * to import weather data from FMI.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 6.2.2021
  */

#ifndef FMIDATAIMPORTER_HH
#define FMIDATAIMPORTER_HH

#include "xmldataimporter.hh"

namespace DataImporting
{

class DataSegmenter;

/**
 * @brief The FmiDataImporter class imports weather data from the Finnish
 * Meteorological Institute's weather data API.
 */
class FmiDataImporter : public XmlDataImporter
{
    Q_OBJECT

public:
    /**
     * @brief The default constructor.
     * @param parent: The QObject to parent this FmiDataImporter to
     */
    explicit FmiDataImporter(QObject* parent = nullptr);

    /**
     * @brief The default destructor.
     */
    virtual ~FmiDataImporter();

    /**
     * @brief fetchData fetches data of the given type from FMI's weather data
     * API.
     * @param dataType: The type of data to fetch
     * @param startTime: The start of the time period to fetch data from
     * @param endTime: The end of the time period to fetch data from
     * @param location: The physical location or area to fetch data from
     */
    void fetchData(const ApiDataType& dataType,
        const QDateTime& startTime, const QDateTime& endTime,
        const std::string& location) override;

    /**
     * @brief getAvailableDataTypes returns all the data types FmiDataImporter
     * is able to fetch with fetchData.
     * @return A vector containing an enum for each data type FmiDataImporter
     * can fetch
     */
    virtual std::vector<ApiDataType> getAvailableDataTypes() override;

    /**
     * @brief getAvailableLocations returns all the locations FmiDataImporter
     * is able to fetch data from with fetchData.
     * @return A vector containing the name of each location FmiDataImporter
     * can fetch data from
     */
    virtual std::vector<std::string> getAvailableLocations() override;

    /**
     * @brief canFetchDataType checks whether FmiDataImporter can fetch the
     * given type of data.
     * @param dataType: The type of data to check for
     * @return True if FmiDataImporter can fetch this type of data, otherwise
     * false
     */
    virtual bool canFetchDataType(const ApiDataType& dataType) override;

    /**
     * @brief getSourceName returns the full name of FMI as a string.
     * @return "Finnish Meteorological Institute"
     */
    virtual std::string getSourceName() override;

protected slots:
    /**
     * @brief parseXml parses XML data for weather data and relays the
     * parsed data through the dataFetched signal.
     * @param xmlData: The XML data that is to be parsed
     * @param url: The URL the XML data was fetched from
     */
    void parseXml(const QDomDocument& xmlData,
                  const std::string& url) override;

protected:
    // Stores every available data type that can be fetched with
    // FmiDataImporter
    const ApiDataType AVAILABLE_DATA_TYPES =
          ApiDataType::Temperature
        | ApiDataType::WindSpeed
        | ApiDataType::CloudAmount
        | ApiDataType::PredictedTemperature
        | ApiDataType::PredictedWindSpeed
        | ApiDataType::PredictedCloudAmount
        | ApiDataType::AverageTemperature
        | ApiDataType::AverageMinTemperature
        | ApiDataType::AverageMaxTemperature;

    // Stores every observation data type, used to select the correct API
    // request parameters
    const ApiDataType OBSERVATION_DATA_TYPES =
          ApiDataType::Temperature
        | ApiDataType::WindSpeed
        | ApiDataType::CloudAmount;

    // Stores every forecast data type, used to select the correct API request
    // parameters
    const ApiDataType FORECAST_DATA_TYPES =
          ApiDataType::PredictedTemperature
        | ApiDataType::PredictedWindSpeed
        | ApiDataType::PredictedCloudAmount;

    // Stores every average data type, used to select the correct API request
    // parameters
    const ApiDataType AVERAGE_DATA_TYPES =
          ApiDataType::AverageTemperature
        | ApiDataType::AverageMinTemperature
        | ApiDataType::AverageMaxTemperature;

    // Maps weather data type enums to the corresponding API parameter codes
    const std::map<ApiDataType, std::string>
        DATA_TYPE_ENUMS_TO_PARAMETER_CODES =
        {
            { ApiDataType::Temperature, "t2m" },
            { ApiDataType::WindSpeed, "ws_10min" },
            { ApiDataType::CloudAmount, "n_man" },
            { ApiDataType::PredictedTemperature, "temperature" },
            { ApiDataType::PredictedWindSpeed, "windspeedms" },
            { ApiDataType::PredictedCloudAmount, "totalcloudcover" },
            { ApiDataType::AverageTemperature, "TA_PT1H_AVG" },
            { ApiDataType::AverageMinTemperature, "TA_PT1H_MIN" },
            { ApiDataType::AverageMaxTemperature, "TA_PT1H_MAX" },
        };

    // Maps API parameter codes to the corresponding weather data type enums
    const std::map<std::string, ApiDataType>
        PARAMETER_CODES_TO_DATA_TYPE_ENUMS =
        {
            { "t2m", ApiDataType::Temperature },
            { "ws_10min", ApiDataType::WindSpeed },
            { "n_man", ApiDataType::CloudAmount },
            { "temperature", ApiDataType::PredictedTemperature },
            { "windspeedms", ApiDataType::PredictedWindSpeed },
            { "totalcloudcover", ApiDataType::PredictedCloudAmount },
            { "TA_PT1H_AVG" , ApiDataType::AverageTemperature },
            { "TA_PT1H_MIN", ApiDataType::AverageMinTemperature },
            { "TA_PT1H_MAX", ApiDataType::AverageMaxTemperature },
        };

    // Maps weather data type enums to the corresponding units of measurement
    const std::map<ApiDataType, std::string>
        DATA_TYPE_ENUMS_TO_UNITS =
        {
            { ApiDataType::Temperature, "C" },
            { ApiDataType::WindSpeed, "m/s" },
            { ApiDataType::CloudAmount, "oktas" },
            { ApiDataType::PredictedTemperature, "C" },
            { ApiDataType::PredictedWindSpeed, "m/s" },
            { ApiDataType::PredictedCloudAmount, "%" },
            { ApiDataType::AverageTemperature, "C" },
            { ApiDataType::AverageMinTemperature, "C" },
            { ApiDataType::AverageMaxTemperature, "C" },
        };

    // Stores the names of the places FmiDataImporter can fetch data from
    const std::vector<std::string> AVAILABLE_PLACE_NAMES =
        {
            "Pirkkala",
            "Helsinki",
            "Kuopio",
            "Oulu",
            "Rovaniemi",
            "Vaasa",
            "Utsjoki",
            "Hanko",
            "Lappeenranta"
        };

    // The prefix used in every FMI API request
    const std::string DATA_REQUEST_PREFIX =
        "https://opendata.fmi.fi/wfs?request=getFeature&version=2.0.0";

    // The API request parameter for fetching observation data
    const std::string OBSERVATION_REQUEST_PARAMETER =
        "&storedquery_id=fmi::observations::weather::simple";

    // The API request parameter for fetching forecast data
    const std::string FORECAST_REQUEST_PARAMETER =
        "&storedquery_id=fmi::forecast::hirlam::surface::point::simple";

    // The API request parameter for fetching average data
    const std::string AVERAGE_REQUEST_PARAMETER =
        "&storedquery_id=fmi::observations::weather::hourly::simple"
        "&timestep=1440";

    // Stores the full name of the FMI
    const std::string FMI_FULL_NAME = "Finnish Meteorological Institute";

    // Stores how long the time frame in a single data fetch request can be in
    // seconds (currently 6 days)
    const int MAX_REQUEST_LENGTH_SECONDS = 6 * 24 * 60 * 60;

    /**
     * @brief segmenter_ stores the DataSegmenter used to handle data segments
     * returned by API requests.
     */
    DataSegmenter* segmenter_;
};

}

#endif // FMIDATAIMPORTER_HH
