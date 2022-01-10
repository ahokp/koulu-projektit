/**
  * @file fmidataimporter.cpp implements the FmiDataImporter class.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 10.4.2021
  */

#include "fmidataimporter.hh"
#include "datasegmenter.hh"

namespace DataImporting
{

/**
 * @brief The DataTypeGroup struct is used by FmiDataImporter::fetchData
 * to group data types together with their request parameters.
 */
struct DataTypeGroup
{
    ApiDataType groupMask;
    std::string requestSourceParameter;
    std::string dataTypesParameter;
    bool firstTypeParameterAdded;
    std::vector<std::string> requestUrls;
};

FmiDataImporter::FmiDataImporter(QObject* parent) : XmlDataImporter(parent),
    segmenter_(new DataSegmenter())
{

}

FmiDataImporter::~FmiDataImporter()
{
    delete segmenter_;
}

void FmiDataImporter::fetchData(const ApiDataType& dataType,
    const QDateTime& startTime, const QDateTime& endTime,
    const std::string& location)
{
    // Group data type parameters based on the API sources they will have to be
    // requested from
    std::vector<DataTypeGroup> dataTypeGroups =
        {
            { OBSERVATION_DATA_TYPES, OBSERVATION_REQUEST_PARAMETER,
              "", false, {} },
            { FORECAST_DATA_TYPES, FORECAST_REQUEST_PARAMETER,
              "", false, {} },
            { AVERAGE_DATA_TYPES, AVERAGE_REQUEST_PARAMETER,
              "", false, {} },
        };

    // dataType can contain multiple data types: separate them here
    std::vector<ApiDataType> separatedDataTypes =
        DataImporter::separateDataTypes(dataType);

    // Gather type parameter strings for the API request
    for (ApiDataType& thisDataType : separatedDataTypes)
    {
        auto parameterCodeIt =
            DATA_TYPE_ENUMS_TO_PARAMETER_CODES.find(thisDataType);

        // If a parameter code doesn't exist for this data type, do nothing
        if (parameterCodeIt == DATA_TYPE_ENUMS_TO_PARAMETER_CODES.end())
        {
            continue;
        }

        for (DataTypeGroup& dataTypeGroup : dataTypeGroups)
        {
            // If this data type belongs in this type group
            if (thisDataType & dataTypeGroup.groupMask)
            {
                // Add separating commas only after the first type parameter
                if (dataTypeGroup.firstTypeParameterAdded)
                {
                    dataTypeGroup.dataTypesParameter += ",";
                }
                else
                {
                    dataTypeGroup.firstTypeParameterAdded = true;
                }

                // Append parameter code to parameter string
                dataTypeGroup.dataTypesParameter += parameterCodeIt->second;

                break;
            }
        }
    }

    QDateTime remainingTimeFrameStart = startTime;
    int remainingTimeFrameLength = endTime.toSecsSinceEpoch()
                                   - startTime.toSecsSinceEpoch();

    // Integer division rounded up
    int segmentCount = remainingTimeFrameLength / MAX_REQUEST_LENGTH_SECONDS
        + (remainingTimeFrameLength % MAX_REQUEST_LENGTH_SECONDS != 0);

    // The FMI API can only return one week of data at a time, so if the time
    // frame is larger than that, we have to send multiple requests like this
    for (int i = 0; i < segmentCount; i++)
    {
        int thisRequestLength = std::min(remainingTimeFrameLength,
                                         MAX_REQUEST_LENGTH_SECONDS);

        QDateTime thisRequestEndTime =
            remainingTimeFrameStart.addSecs(thisRequestLength);

        // Create the middle part of the request URL
        std::string requestMiddleString = "&place=" + location + "&starttime="
            + dateTimeToApiString(remainingTimeFrameStart) + "&endtime="
            + dateTimeToApiString(thisRequestEndTime) + "&parameters=";

        // Create URLs for this time period for each data group
        for (DataTypeGroup& dataTypeGroup : dataTypeGroups)
        {
            // If this data group received parameters to send
            if (dataTypeGroup.firstTypeParameterAdded == true)
            {
                std::string requestUrl = DATA_REQUEST_PREFIX
                                         + dataTypeGroup.requestSourceParameter
                                         + requestMiddleString
                                         + dataTypeGroup.dataTypesParameter;

                dataTypeGroup.requestUrls.push_back(requestUrl);

                xmlFetcher_.fetchXml(requestUrl);
            }
        }

        remainingTimeFrameStart = thisRequestEndTime;
        remainingTimeFrameLength -= thisRequestLength;
    }

    // Create data receptables for each of the data types being fetched
    for (ApiDataType& thisDataType : separatedDataTypes)
    {
        for (DataTypeGroup& dataTypeGroup : dataTypeGroups)
        {
            // If this data type belongs in this type group
            if (thisDataType & dataTypeGroup.groupMask)
            {
                // Create new receptable for this data type, time period and
                // the API request URLs used to fetch the data from this period
                segmenter_->openNewReceptable(thisDataType, location,
                                              startTime, endTime,
                                              dataTypeGroup.requestUrls);

                break;
            }
        }
    }
}

std::vector<ApiDataType> FmiDataImporter::getAvailableDataTypes()
{
    return DataImporter::separateDataTypes(AVAILABLE_DATA_TYPES);
}

std::vector<std::string> FmiDataImporter::getAvailableLocations()
{
    return AVAILABLE_PLACE_NAMES;
}

bool FmiDataImporter::canFetchDataType(const ApiDataType& dataType)
{
    return dataType & AVAILABLE_DATA_TYPES;
}

std::string FmiDataImporter::getSourceName()
{
    return FMI_FULL_NAME;
}

void FmiDataImporter::parseXml(const QDomDocument& xmlData,
                               const std::string& url)
{
    QDomElement xmlRoot = xmlData.documentElement();

    // Iterate through XML nodes
    for (QDomNode currentDataNode = xmlRoot.firstChild();
         !currentDataNode.isNull();
         currentDataNode = currentDataNode.nextSibling())
    {
        QDomElement wfsElement = currentDataNode.firstChildElement();

        // If this element doesn't seem to be what was expected, skip it
        if (wfsElement.tagName() != "BsWfs:BsWfsElement")
        {
            continue;
        }

        std::string dataTypeString = "unknown";
        std::string dataValueString = "null";
        std::string dataTimeString = "unknown";

        // Find the data type, value and time from the child elements
        for (QDomElement currentChildElement = wfsElement.firstChildElement();
             !currentChildElement.isNull();
             currentChildElement = currentChildElement.nextSiblingElement())
        {
            QString tagName = currentChildElement.tagName();

            if (tagName == "BsWfs:ParameterName")
            {
                dataTypeString = currentChildElement.firstChild()
                    .toText().data().toStdString();
            }
            else if (tagName == "BsWfs:ParameterValue")
            {
                dataValueString = currentChildElement.firstChild()
                    .toText().data().toStdString();
            }
            else if (tagName == "BsWfs:Time")
            {
                dataTimeString = currentChildElement.firstChild()
                    .toText().data().toStdString();
            }
        }

        // Find data type
        ApiDataType dataType = ApiDataType::None;

        auto dataTypeIter =
            PARAMETER_CODES_TO_DATA_TYPE_ENUMS.find(dataTypeString);

        if (dataTypeIter != PARAMETER_CODES_TO_DATA_TYPE_ENUMS.end())
        {
            dataType = dataTypeIter->second;
        }
        else
        {
            // Unknown data type, don't use this data point
            continue;
        }

        // Parse data value and time
        float dataValue = 0;
        QDateTime dataTime;

        try
        {
            dataValue = std::stof(dataValueString);
            dataTime = dateTimeFromApiString(dataTimeString);
        }
        catch (const std::invalid_argument& exception)
        {
            // Data value string or time string failed to parse, can't handle
            // this data point correctly, don't use it
            continue;
        }

        // Ignore invalid values
        if (std::isnan(dataValue) || !dataTime.isValid())
        {
            continue;
        }

        segmenter_->pushParsedDataPoint({ dataTime, dataValue },
                                        url, dataType);
    }

    // Get filled receptable data from segmenter. Each receptable contains all
    // the segments for a single fetchData call
    std::vector<SegmentedDataDetails> filledSegments =
        segmenter_->getFilledReceptables(url);

    for (SegmentedDataDetails& thisDataDetails : filledSegments)
    {
        // Pass the data from a completed fetch operation forward
        emit dataFetched(
            { thisDataDetails.dataType, thisDataDetails.dataLocation,
              DATA_TYPE_ENUMS_TO_UNITS.at(thisDataDetails.dataType), this,
              thisDataDetails.startDateTime, thisDataDetails.endDateTime,
              thisDataDetails.maxValue, thisDataDetails.minValue },
            thisDataDetails.data);
    }
}

}
