/**
  * @file fingriddataimporter.hh declares the FingridDataImporter class, which
  * is used to import electricity data from Fingrid.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 10.4.2021
  */

#ifndef FINGRIDDATAIMPORTER_HH
#define FINGRIDDATAIMPORTER_HH

#include "xmldataimporter.hh"

namespace DataImporting
{

class DataSegmenter;

/**
 * @brief The FingridDataImporter class imports weather data from Fingrid's
 * electricity data API.
 */
class FingridDataImporter : public XmlDataImporter
{
    Q_OBJECT

public:
    /**
     * @brief The default constructor.
     * @param parent: The QObject to parent this FingridDataImporter to
     */
    explicit FingridDataImporter(QObject* parent = nullptr);

    /**
     * @brief The default destructor.
     */
    virtual ~FingridDataImporter();

    /**
     * @brief fetchData fetches data of the given type from Fingrid's
     * electricity data API.
     * @param dataType: The type of data to fetch
     * @param startTime: The start of the time period to fetch data from
     * @param endTime: The end of the time period to fetch data from
     * @param location: The physical location or area to fetch data from
     */
    void fetchData(const ApiDataType& dataType,
        const QDateTime& startTime, const QDateTime& endTime,
        const std::string& location) override;

    /**
     * @brief getAvailableDataTypes returns all the data types
     * FingridDataImporter is able to fetch with fetchData.
     * @return A vector containing an enum for each data type
     * FingridDataImporter can fetch
     */
    virtual std::vector<ApiDataType> getAvailableDataTypes() override;

    /**
     * @brief getAvailableLocations returns all the locations
     * FingridDataImporter is able to fetch data from with fetchData.
     * @return A vector containing the name of each location
     * FingridDataImporter can fetch data from
     */
    virtual std::vector<std::string> getAvailableLocations() override;

    /**
     * @brief canFetchDataType checks whether FingridDataImporter can fetch the
     * given type of data.
     * @param dataType: The type of data to check for
     * @return True if FingridDataImporter can fetch this type of data,
     * otherwise false
     */
    virtual bool canFetchDataType(const ApiDataType& dataType) override;

    /**
     * @brief getSourceName returns the full name of Fingrid as a string.
     * @return "Fingrid Oyj"
     */
    virtual std::string getSourceName() override;

protected slots:
    /**
     * @brief parseXml parses XML data for electricity data and relays the
     * parsed data through the dataFetched signal.
     * @param xmlData: The XML data that is to be parsed
     */
    void parseXml(const QDomDocument& xmlData,
                  const std::string& url) override;

protected:
    const ApiDataType AVAILABLE_DATA_TYPES =
          ApiDataType::ElectricityConsumption
        | ApiDataType::ElectricityProduction
        | ApiDataType::NuclearPowerProduction
        | ApiDataType::HydroPowerProduction
        | ApiDataType::WindPowerProduction
        | ApiDataType::PredictedElectricityConsumption
        | ApiDataType::PredictedElectricityProduction
        | ApiDataType::PredictedWindPowerProduction;

    // Maps electricity data type enums to the corresponding API variable IDs
    const std::map<ApiDataType, std::string>
        DATA_TYPE_ENUMS_TO_VARIABLE_IDS =
        {
            { ApiDataType::ElectricityConsumption, "193" },
            { ApiDataType::ElectricityProduction, "192" },
            { ApiDataType::NuclearPowerProduction, "188" },
            { ApiDataType::HydroPowerProduction, "191" },
            { ApiDataType::WindPowerProduction, "181" },
            { ApiDataType::PredictedElectricityConsumption, "166" },
            { ApiDataType::PredictedElectricityProduction, "242" },
            { ApiDataType::PredictedWindPowerProduction, "245" }
        };

    // Maps API variable IDs to the corresponding electricity data type enums
    const std::map<std::string, ApiDataType>
        VARIABLE_IDS_TO_DATA_TYPE_ENUMS =
        {
            { "193", ApiDataType::ElectricityConsumption },
            { "192", ApiDataType::ElectricityProduction },
            { "188", ApiDataType::NuclearPowerProduction },
            { "191", ApiDataType::HydroPowerProduction },
            { "181", ApiDataType::WindPowerProduction },
            { "166", ApiDataType::PredictedElectricityConsumption },
            { "242", ApiDataType::PredictedElectricityProduction },
            { "245", ApiDataType::PredictedWindPowerProduction }
        };

    // Stores the names of the locations FingridDataImporter can fetch data
    // from
    const std::vector<std::string> AVAILABLE_DATA_LOCATIONS =
        {
            "Finland"
        };

    // Stores the file path to a text file containing the API key
    const std::string API_KEY_PATH = "./FINGRID_API_KEY.txt";

    // Stores the name of the HTML header the API key is sent in
    const std::string API_KEY_HEADER_NAME = "x-api-key";

    // Stores the unit of measurement for the electricity data returned
    const std::string DATA_UNIT_OF_MEASUREMENT = "MW";

    // The shared, constant prefix used in every Fingrid API request
    // (defined in fingriddataimporter.cpp)
    const static std::string DATA_REQUEST_PREFIX;

    // Stores the full name of the company Fingrid.
    const std::string FINGRID_FULL_NAME = "Fingrid Oyj";

    // Stores how long the time frame in a single data fetch request can be in
    // seconds (currently 3 months)
    const int MAX_REQUEST_LENGTH_SECONDS = 3 * 30 * 24 * 60 * 60;

    /**
     * @brief apiKey_ stores the API key needed by Fingrid's API.
     */
    std::string apiKey_;

    /**
     * @brief segmenter_ stores the DataSegmenter used to handle data segments
     * returned by API requests.
     */
    DataSegmenter* segmenter_;

    /**
     * @brief variableIdFromApiUrl extracts the variable ID from a Fingrid API
     * data request URL.
     * @param url: The URL to get the variable ID from
     * @return The variable ID as a string
     * @exception std::invalid_argument: Thrown when the URL didn't contain a
     * variable ID
     */
    static std::string variableIdFromApiUrl(const std::string& url);
};

}

#endif // FINGRIDDATAIMPORTER_HH
