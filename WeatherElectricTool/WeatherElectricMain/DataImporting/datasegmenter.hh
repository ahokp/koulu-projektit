/**
  * @file dataimporter.hh declares the DataSegmenter class, which is used to
  * handle segments of fetched data and combining them.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 10.4.2021
  */

#ifndef DATASEGMENTER_HH
#define DATASEGMENTER_HH

#include "dataimporter.hh"

namespace DataImporting
{

/**
 * @brief The SegmentedDataDetails struct is used to store the details of a
 * segmented data instance.
 */
struct SegmentedDataDetails
{
    std::shared_ptr<std::vector<DataPoint>> data;
    ApiDataType dataType;
    std::string dataLocation;
    QDateTime startDateTime;
    QDateTime endDateTime;
    float maxValue;
    float minValue;
};

/**
 * @brief The DataSegmenter class handles segmented data using data receptables
 * that receive the data segments and combine them into a single data
 * structure.
 */
class DataSegmenter
{
public:
    /**
     * @brief The default constructor.
     */
    DataSegmenter();

    /**
     * @brief openNewReceptable opens a new receptable for segmented data in
     * this DataSegmenter. The receptable can then be filled with
     * pushParsedDataPoint and its contents retrieved with
     * getFilledReceptables.
     * @param dataType: The type of data to store in this receptable
     * @param dataLocation: The location from which the data in this receptable
     * will be from
     * @param startDateTime: The start of the time period from which this
     * receptable is going to receive data from
     * @param endDateTime: The end of the time period from which this
     * receptable is going to receive data from
     * @param segmentUrls: The API request URLs of the data segments that this
     * receptable is expected to receive, in chronological order
     */
    void openNewReceptable(const ApiDataType& dataType,
                           const std::string& dataLocation,
                           const QDateTime& startDateTime,
                           const QDateTime& endDateTime,
                           const std::vector<std::string>& segmentUrls);

    /**
     * @brief getFilledReceptables checks for filled data receptables with the
     * given request URL and returns their contents if found.
     * @param url: One of the URLs the receptable was expected to receive data
     * from
     * @return The data collected by each filled receptable
     */
    std::vector<SegmentedDataDetails> getFilledReceptables(
        const std::string& url);

    /**
     * @brief pushParsedDataPoint pushes a new data point to the receptable
     * corresponding to the given request URL and data type.
     * @param dataPoint: The data point to push
     * @param url: The request URL to label this data point under
     * @param dataType: The type of the data point's data
     */
    void pushParsedDataPoint(const DataPoint& dataPoint,
                             const std::string& url,
                             const ApiDataType& dataType);

protected:
    /**
     * @brief The DataSegmentReceptable struct is used as a receptable for
     * several data segments that should be later combined into one data
     * structure.
     */
    struct DataSegmentReceptable
    {
        SegmentedDataDetails dataDetails;
        std::vector<std::vector<DataPoint>*> dataSegments;
        std::map<std::string, int> segmentIndicesPerUrl;
        bool hasReceivedSegments;
    };

    // Maps API request URLs to the data segment receptables that the data
    // returned by the requests should be placed in
    std::map<std::string,
             std::map<ApiDataType, DataSegmentReceptable*>>
        dataSegmentReceptablesPerUrl_;
};

}

#endif // DATASEGMENTER_HH
