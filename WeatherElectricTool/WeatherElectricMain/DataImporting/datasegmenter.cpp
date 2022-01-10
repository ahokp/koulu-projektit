#include "datasegmenter.hh"

namespace DataImporting
{

DataSegmenter::DataSegmenter()
{

}

void DataSegmenter::openNewReceptable(const ApiDataType& dataType,
                                      const std::string& dataLocation,
                                      const QDateTime& startDateTime,
                                      const QDateTime& endDateTime,
                                      const std::vector<std::string>& segmentUrls)
{
    DataSegmentReceptable* thisReceptable = new DataSegmentReceptable();

    thisReceptable->dataDetails = { nullptr, dataType, dataLocation,
                                    startDateTime, endDateTime, 0, 0 };

    thisReceptable->dataSegments =
        std::vector<std::vector<DataPoint>*>(segmentUrls.size(), nullptr);

    int segmentIndex = 0;

    // Connect segment request URLs to the correct segment slots in the
    // receptable
    for (const std::string& thisUrl : segmentUrls)
    {
        auto urlReceptables =
            dataSegmentReceptablesPerUrl_.find(thisUrl);

        if (urlReceptables == dataSegmentReceptablesPerUrl_.end())
        {
            urlReceptables =
                dataSegmentReceptablesPerUrl_.insert({ thisUrl, {} }).first;
        }

        urlReceptables->second.insert({ dataType, thisReceptable });
        thisReceptable->segmentIndicesPerUrl.insert({ thisUrl, segmentIndex });

        segmentIndex++;
    }
}

std::vector<SegmentedDataDetails> DataSegmenter::getFilledReceptables(
    const std::string& url)
{
    std::vector<SegmentedDataDetails> filledReceptables;

    auto thisUrlReceptablesIt = dataSegmentReceptablesPerUrl_.find(url);

    std::map<ApiDataType, DataSegmentReceptable*>& thisUrlReceptables =
        thisUrlReceptablesIt->second;

    // Check for filled receptables
    for (auto it = thisUrlReceptables.begin();
         it != thisUrlReceptables.end();)
    {
        DataSegmentReceptable& receptable = *it->second;

        int segmentIndex = receptable.segmentIndicesPerUrl.at(url);
        std::vector<DataPoint>*& dataVector =
            receptable.dataSegments.at(segmentIndex);

        // If a segment that was meant to be in this request URL could not be
        // parsed
        if (dataVector == nullptr)
        {
            // Return empty segment
            dataVector = new std::vector<DataPoint>();
        }

        // Count how many segments are still null (haven't been received yet)
        int nullSegmentCount =
            std::count(receptable.dataSegments.begin(),
                       receptable.dataSegments.end(), nullptr);

        // If all segments have been received
        if (nullSegmentCount == 0)
        {
            // Calculate total data point count
            unsigned int totalDataLength = 0;

            for (auto it2 = receptable.dataSegments.begin();
                 it2 != receptable.dataSegments.end(); it2++)
            {
                totalDataLength += (*it2)->size();
            }

            // Create vector to store all the data from this receptable in
            receptable.dataDetails.data =
                std::make_shared<std::vector<DataPoint>>();

            std::shared_ptr<std::vector<DataPoint>>& allData =
                receptable.dataDetails.data;

            allData->reserve(totalDataLength);

            unsigned int addedDataLength = 0;

            // Combine segment data into one vector
            for (std::vector<DataPoint>*& segment : receptable.dataSegments)
            {
                allData->insert(allData->begin() + addedDataLength,
                                segment->begin(), segment->end());

                addedDataLength += segment->size();
            }

            // Add the filled receptable to total filled receptables
            filledReceptables.push_back(receptable.dataDetails);

            // Delete filled receptable as it's served its purpose
            delete it->second;
            it = thisUrlReceptables.erase(it);
        }
        else
        {
            it++;
        }
    }

    dataSegmentReceptablesPerUrl_.erase(thisUrlReceptablesIt);

    return filledReceptables;
}

void DataSegmenter::pushParsedDataPoint(const DataPoint& dataPoint,
                                        const std::string& url,
                                        const ApiDataType& dataType)
{
    // Get receptable for this data type and URL
    DataSegmentReceptable& dataReceptable =
        *dataSegmentReceptablesPerUrl_.at(url).find(dataType)->second;

    int segmentIndex = dataReceptable.segmentIndicesPerUrl.at(url);

    std::vector<DataPoint>*& dataVector =
        dataReceptable.dataSegments.at(segmentIndex);

    if (dataVector == nullptr)
    {
        // Haven't parsed data of this type yet, create new vector for it
        dataVector = new std::vector<DataPoint>();

        // If this is the first data point received by this receptable, set
        // max and min value to the data value to make sure they're correct
        if (!dataReceptable.hasReceivedSegments)
        {
            dataReceptable.dataDetails.minValue = dataPoint.value;
            dataReceptable.dataDetails.maxValue = dataPoint.value;

            dataReceptable.hasReceivedSegments = true;
        }
    }
    else
    {
        // Update min/max values
        dataReceptable.dataDetails.minValue =
            std::min(dataReceptable.dataDetails.minValue, dataPoint.value);
        dataReceptable.dataDetails.maxValue =
            std::max(dataReceptable.dataDetails.maxValue, dataPoint.value);
    }

    // Add data point to total parsed data
    dataVector->push_back(dataPoint);
}

}
