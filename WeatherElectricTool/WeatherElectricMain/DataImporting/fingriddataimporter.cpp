/**
  * @file fingriddataimporter.cpp implements the FingridDataImporter class.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 10.4.2021
  */

#include "fingriddataimporter.hh"
#include "datasegmenter.hh"

#include <fstream>
#include <iostream>

namespace DataImporting
{

const std::string FingridDataImporter::DATA_REQUEST_PREFIX =
    "https://api.fingrid.fi/v1/variable/";

FingridDataImporter::FingridDataImporter(QObject* parent) :
    XmlDataImporter(parent), segmenter_(new DataSegmenter())
{
    std::ifstream apiKeyFile(API_KEY_PATH);

    // Read API key from file
    if (apiKeyFile.is_open())
    {
        getline(apiKeyFile, apiKey_);
        apiKeyFile.close();
    }
}

FingridDataImporter::~FingridDataImporter()
{
    delete segmenter_;
}

void FingridDataImporter::fetchData(const ApiDataType& dataType,
    const QDateTime& startTime, const QDateTime& endTime,
    const std::string& location)
{
    // Fingrid only has data for the entirety of Finland, so the location can't
    // be chosen precisely
    Q_UNUSED(location);

    // dataType can contain multiple data types: separate them here
    std::vector<ApiDataType> separatedDataTypes =
        DataImporter::separateDataTypes(dataType);

    QDateTime remainingTimeFrameStart = startTime;
    int remainingTimeFrameLength = endTime.toSecsSinceEpoch()
                                   - startTime.toSecsSinceEpoch();

    // Integer division rounded up
    int segmentCount = remainingTimeFrameLength / MAX_REQUEST_LENGTH_SECONDS
        + (remainingTimeFrameLength % MAX_REQUEST_LENGTH_SECONDS != 0);

    std::vector<std::string> timeFrameStrings;

    // The Fingrid API can only return data from a limited time frame in one
    // request, so we have to send multiple requests like this
    for (int i = 0; i < segmentCount; i++)
    {
        int thisRequestLength = std::min(remainingTimeFrameLength,
                                         MAX_REQUEST_LENGTH_SECONDS);

        QDateTime thisRequestEndTime =
            remainingTimeFrameStart.addSecs(thisRequestLength);

        timeFrameStrings.push_back("start_time="
            + dateTimeToApiString(remainingTimeFrameStart)
            + "&end_time=" + dateTimeToApiString(thisRequestEndTime));

        remainingTimeFrameStart = thisRequestEndTime;
        remainingTimeFrameLength -= thisRequestLength;
    }

    for (auto it = separatedDataTypes.begin();
         it != separatedDataTypes.end(); it++)
    {
        ApiDataType& thisDataType = *it;

        auto variableIdIter =
            DATA_TYPE_ENUMS_TO_VARIABLE_IDS.find(thisDataType);

        // If a variable ID doesn't exist for this data type, do nothing
        if (variableIdIter == DATA_TYPE_ENUMS_TO_VARIABLE_IDS.end())
        {
            continue;
        }

        std::vector<std::string> thisDataTypeRequestUrls;

        for (auto it2 = timeFrameStrings.begin();
             it2 != timeFrameStrings.end(); it2++)
        {
            std::string& timeFrameString = *it2;

            std::string requestUrl = DATA_REQUEST_PREFIX + variableIdIter->second +
                "/events/xml?" + timeFrameString;

            thisDataTypeRequestUrls.push_back(requestUrl);

            // Send fetch request for this variable ID
            xmlFetcher_.fetchXml(requestUrl, API_KEY_HEADER_NAME, apiKey_);
        }

        segmenter_->openNewReceptable(thisDataType, location,
                                      startTime, endTime,
                                      thisDataTypeRequestUrls);
    }
}

std::vector<ApiDataType> FingridDataImporter::getAvailableDataTypes()
{
    return DataImporter::separateDataTypes(AVAILABLE_DATA_TYPES);
}

std::vector<std::string> FingridDataImporter::getAvailableLocations()
{
    return AVAILABLE_DATA_LOCATIONS;
}

bool FingridDataImporter::canFetchDataType(const ApiDataType& dataType)
{
    return AVAILABLE_DATA_TYPES & dataType;
}

std::string FingridDataImporter::getSourceName()
{
    return FINGRID_FULL_NAME;
}

void FingridDataImporter::parseXml(const QDomDocument& xmlData,
                                   const std::string& url)
{
    // Identify received data type based on variable ID
    auto dataTypeIter = VARIABLE_IDS_TO_DATA_TYPE_ENUMS.find(
                            variableIdFromApiUrl(url));

    // Unknown data type, do nothing
    if (dataTypeIter == VARIABLE_IDS_TO_DATA_TYPE_ENUMS.end())
    {
        return;
    }

    const ApiDataType& dataType = dataTypeIter->second;

    // Parse the XML data
    QDomElement xmlRoot = xmlData.documentElement();

    for (QDomNode currentDataElement = xmlRoot.firstChildElement();
         !currentDataElement.isNull();
         currentDataElement = currentDataElement.nextSibling())
    {
        std::string dataValueString = "null";
        std::string dataStartTimeString = "unknown";
        std::string dataEndTimeString = "unknown";

        // Find the data value, start time and end time from the child elements
        for (QDomElement currentChildElement =
                currentDataElement.firstChildElement();
             !currentChildElement.isNull();
             currentChildElement = currentChildElement.nextSiblingElement())
        {
            QString tagName = currentChildElement.tagName();

            if (tagName == "value")
            {
                dataValueString = currentChildElement.firstChild()
                    .toText().data().toStdString();
            }
            else if (tagName == "start_time")
            {
                dataStartTimeString = currentChildElement.firstChild()
                    .toText().data().toStdString();
            }
            else if (tagName == "end_time")
            {
                dataEndTimeString = currentChildElement.firstChild()
                    .toText().data().toStdString();
            }
        }

        float dataValue;
        QDateTime dataStartTime;
        QDateTime dataEndTime;

        try
        {
            dataValue = std::stof(dataValueString);
            dataStartTime = dateTimeFromApiString(dataStartTimeString);
            dataEndTime = dateTimeFromApiString(dataStartTimeString);
        }
        catch (const std::invalid_argument& exception)
        {
            // Data failed to parse, can't handle this data point correctly,
            // don't use it
            continue;
        }

        // Ignore invalid values
        if (std::isnan(dataValue)
            || !dataStartTime.isValid()
            || !dataEndTime.isValid())
        {
            continue;
        }

        // Add data point for just end time to reduce memory needed
        segmenter_->pushParsedDataPoint({ dataEndTime, dataValue },
                                        url, dataType);
    }

    // Get filled receptable data from segmenter. Each receptable contains all
    // the segments for a single fetchData call
    std::vector<SegmentedDataDetails> filledSegments =
        segmenter_->getFilledReceptables(url);

    for (auto it = filledSegments.begin(); it != filledSegments.end(); it++)
    {
        SegmentedDataDetails& thisDataDetails = *it;

        // Pass the data from a completed fetch operation forward
        emit dataFetched(
            {
                thisDataDetails.dataType, thisDataDetails.dataLocation,
                DATA_UNIT_OF_MEASUREMENT, this,
                thisDataDetails.startDateTime, thisDataDetails.endDateTime,
                thisDataDetails.maxValue, thisDataDetails.minValue

            }, thisDataDetails.data);
    }
}

std::string FingridDataImporter::variableIdFromApiUrl(const std::string& url)
{
    std::string variableId = "";

    unsigned int urlSize = url.size();
    unsigned int i = DATA_REQUEST_PREFIX.size();

    if (i >= urlSize)
    {
        throw std::invalid_argument(
            "URL too short to contain a variable id");
    }

    char currentChar = url[i];

    while (currentChar != '/')
    {
        variableId += currentChar;
        i++;

        if (i >= urlSize)
        {
            throw std::invalid_argument(
                "URL too short to contain a variable id");
        }

        currentChar = url[i];
    }

    return variableId;
}

}
