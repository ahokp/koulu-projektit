/**
  * @file dataimporter.cpp implements the DataImporter class.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 10.4.2021
  */

#include "DataImporting/dataimporter.hh"

namespace DataImporting
{

const std::map<std::string, ApiDataType>
    DATA_TYPE_NAMES_TO_DATA_TYPE_ENUMS =
{
    { "Temperature",
      ApiDataType::Temperature },
    { "Cloud Amount",
      ApiDataType::CloudAmount },
    { "Wind Speed",
      ApiDataType::WindSpeed },
    { "Predicted Temperature",
      ApiDataType::PredictedTemperature },
    { "Predicted Cloud Amount",
      ApiDataType::PredictedCloudAmount },
    { "Predicted Wind Speed",
      ApiDataType::PredictedWindSpeed },
    { "Average Temperature",
      ApiDataType::AverageTemperature },
    { "Average Minimum Temperature",
      ApiDataType::AverageMinTemperature },
    { "Average Maximum Temperature",
      ApiDataType::AverageMaxTemperature },
    { "Electricity Consumption",
      ApiDataType::ElectricityConsumption },
    { "Electricity Production",
      ApiDataType::ElectricityProduction },
    { "Nuclear Power Production",
      ApiDataType::NuclearPowerProduction },
    { "Hydropower Production",
      ApiDataType::HydroPowerProduction },
    { "Wind Power Production",
      ApiDataType::WindPowerProduction },
    { "Predicted Electricity Consumption",
      ApiDataType::PredictedElectricityConsumption },
    { "Predicted Electricity Production",
      ApiDataType::PredictedElectricityProduction },
    { "Predicted Nuclear Power Production",
      ApiDataType::PredictedNuclearPowerProduction },
    { "Predicted Hydropower Production",
      ApiDataType::PredictedHydroPowerProduction },
    { "Predicted Wind Power Production",
      ApiDataType::PredictedWindPowerProduction }
};

const std::map<ApiDataType, std::string>
    DATA_TYPE_ENUMS_TO_DATA_TYPE_NAMES =
{
    { ApiDataType::Temperature,
      "Temperature" },
    { ApiDataType::CloudAmount,
      "Cloud Amount" },
    { ApiDataType::WindSpeed,
      "Wind Speed" },
    { ApiDataType::PredictedTemperature,
      "Predicted Temperature" },
    { ApiDataType::PredictedCloudAmount,
      "Predicted Cloud Amount" },
    { ApiDataType::PredictedWindSpeed,
      "Predicted Wind Speed" },
    { ApiDataType::AverageTemperature,
      "Average Temperature" },
    { ApiDataType::AverageMinTemperature,
      "Average Minimum Temperature" },
    { ApiDataType::AverageMaxTemperature,
      "Average Maximum Temperature" },
    { ApiDataType::ElectricityConsumption,
      "Electricity Consumption" },
    { ApiDataType::ElectricityProduction,
      "Electricity Production" },
    { ApiDataType::NuclearPowerProduction,
      "Nuclear Power Production" },
    { ApiDataType::HydroPowerProduction,
      "Hydropower Production" },
    { ApiDataType::WindPowerProduction,
      "Wind Power Production" },
    { ApiDataType::PredictedElectricityConsumption,
      "Predicted Electricity Consumption" },
    { ApiDataType::PredictedElectricityProduction,
      "Predicted Electricity Production" },
    { ApiDataType::PredictedNuclearPowerProduction,
      "Predicted Nuclear Power Production" },
    { ApiDataType::PredictedHydroPowerProduction,
      "Predicted Hydropower Production" },
    { ApiDataType::PredictedWindPowerProduction,
      "Predicted Wind Power Production" }
};

ApiDataType operator|(ApiDataType a, ApiDataType b)
{
    return static_cast<ApiDataType>(static_cast<int>(a) | static_cast<int>(b));
}

ApiDataType operator&(ApiDataType a, ApiDataType b)
{
    return static_cast<ApiDataType>(static_cast<int>(a) & static_cast<int>(b));
}

ApiDataType operator~(ApiDataType a)
{
    return static_cast<ApiDataType>(~static_cast<int>(a));
}

std::string getDataTypeName(const ApiDataType& dataType)
{
    auto dataTypeIter = DATA_TYPE_ENUMS_TO_DATA_TYPE_NAMES.find(dataType);

    if (dataTypeIter == DATA_TYPE_ENUMS_TO_DATA_TYPE_NAMES.end())
    {
        return "None";
    }

    return dataTypeIter->second;
}

ApiDataType getDataTypeFromName(const std::string& dataTypeName)
{
    auto dataTypeIter = DATA_TYPE_NAMES_TO_DATA_TYPE_ENUMS.find(dataTypeName);

    if (dataTypeIter == DATA_TYPE_NAMES_TO_DATA_TYPE_ENUMS.end())
    {
        return ApiDataType::None;
    }

    return dataTypeIter->second;
}

DataImporter::DataImporter(QObject* parent) : QObject(parent)
{

}

std::vector<ApiDataType> DataImporter::separateDataTypes(
    const ApiDataType& dataType)
{
    std::vector<ApiDataType> separatedDataTypes;

    // Check dataType for set bits one at a time
    for (int i = 0; i < API_DATA_TYPE_COUNT; i++)
    {
        // Isolate ith bit of dataType
        ApiDataType inspectedBit =
            static_cast<ApiDataType>(1 << i & dataType);

        // If inspected bit is set, dataType contains the ApiDataType
        // represented by this bit
        if (inspectedBit)
        {
            separatedDataTypes.push_back(inspectedBit);
        }
    }

    return separatedDataTypes;
}

}
