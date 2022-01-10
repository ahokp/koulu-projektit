/**
  * @file weathergraph.hh declares the WeatherGraph class, which is used to
  * display the given data series as a graph
  * @author Johannes Simulainen 284213 johannes.simulainen@tuni.fi
  * @date 12.3.2021
  */

#ifndef WEATHERGRAPH_HH
#define WEATHERGRAPH_HH

#include "weatherchartbase.hh"
#include <vector>

namespace Ui { class WeatherGraph; }

// Different axis alignments
const std::vector<QFlag> ALIGNMENT_Y{Qt::AlignLeft, Qt::AlignRight};

/**
 * @brief The WeatherGraph class initializes the graphs characteristics and
 * displays given splineseries as spline graphs
 */
class WeatherGraph : public WeatherChartBase
{
    Q_OBJECT

public:

    /**
     * @brief The default constructor.
     * @param startDate: The date where the data was fetched from
     */
    WeatherGraph(QDateTime startDate);

    /**
     * @brief The default destructor.
     */
    ~WeatherGraph();

    /**
     * @brief addActiveSeries adds a given data series to the graph
     * @param seriesPair: Data to be added
     * @return true if data was successfully added, false otherwise
     */
    bool addActiveSeries(std::pair<std::string, DataSeries> seriesPair) override;

    /**
     * @brief deleteActiveSeries removes the given series from the graph and
     * deletes the data/attached axes
     * @param dataSeriesName: Name of the data series that will be removed
     * @return true if data was successfully deleted, false otherwise
     */
    bool deleteActiveSeries(std::string dataSeriesName) override;

    /**
     * @brief hideSeries hides/shows the given series from the graph temporarily
     * @param dataSeriesName: Name of the data series that will be hidden
     * @param hide: If the graph is to be hidden or shown
     * @return true if data was successfully hidden, false otherwise
     */
    bool hideSeries(std::string dataSeriesName, bool hide) override;

    /**
     * @brief addPointsToSeries adds data points to an existing series
     * @param dataSeriesName: Name of the data series that the points will be
     * added to
     * @param addedSeries: DataSeries that will be combined to the existing one
     * @param addBefore: Defines if the new series will be added in front or
     * after the existing one
     */
    void addPointsToSeries(std::string dataSeriesName, DataSeries addedSeries, bool addBefore);

    /**
     * @brief removePointsFromSeries removes data points from an existing series
     * @param dataSeriesName: Name of the data series that the points will be
     * removed from
     * @param delIndex: Index where the point removal starts or ends depending
     * on delBefore
     * @param delBefore: Defines if the points that will be removed are before
     * or after delIndex
     */
    void removePointsFromSeries(std::string dataSeriesName, int delIndex, bool delBefore);

    /**
     * @brief scaleAxis scales the axis related to the given dataseries
     * @param dataSeriesName: Name of the data series whose axis will be scaled
     * @param multiplier: Scaling multiplier
     * @param maxValue: True if scaling max value, false if scaling min value
     */
    void scaleAxis(std::string dataSeriesName, double multiplier, bool maxValue);

private:

    /**
     * @brief determineSeriesColor finds an unused color from SERIESCOLORS
     * @return currently unused color or default color if none were found
     */
    QColor determineSeriesColor() override;

    /**
     * @brief initGraphAxis initializes graph axes and their properties
     */
    void initGraphAxis();

    /**
     * @brief createDateAxisX creates the date axis which will be visible to
     * the user and sets it's attributes
     * @param endTimeInSeconds: Time when data fetching stopped in seconds
     * @return pointer to QDateTimeAxis which will act as the graphs X-axis
     */
    QDateTimeAxis* createDateAxisX(int startTimeInSeconds, int endTimeInSeconds);

    /**
     * @brief createValueAxisY creates the Y value axis for a given series and
     * sets it's attributes
     * @param splineSeries: The series for which the axis will be created for
     * @param minY: Minimum value of the axis
     * @param maxY: Maximum value of the axis
     * @return pointer to QValueAxis which displays the Y-values of the given series
     */
    QValueAxis* createValueAxisY(DataSeries series, float minY, float maxY);

    /**
     * @brief calcGraphX calculates the largest X-value out of given series list
     * @param seriesList: List of series attached to the graph
     * @return smallest and largest x for the graph
     */
    std::pair<int, int> calcGraphX(QList<QAbstractSeries*> seriesList);

    /**
     * @brief calcBuffer calculates the buffer from given min and max values
     * @param min: Minimum value which is used to calculate the buffer
     * @param max: Maximum value which is used to calculate the buffer
     * @return the buffer from the values
     */
    float calcBuffer(float min, float max);

    /**
     * @brief findMinMaxAttachedToAxis finds the minimum and maximum values
     * attached to a given axis
     * @param axis: The axis which the values will be found for
     * @return the min and max values found for given axis
     */
    std::pair<float, float> findMinMaxAttachedToAxis(QValueAxis* axis);

    /**
     * @brief findSeriesMinMaxY iterates through series' points and return the
     * min and max y-values
     * @param series: Series which the values will be searched for
     * @return found maximum and minimum values
     */
    std::pair<float, float> findSeriesMinMaxY(const DataSeries &series);

    /**
     * @brief findAxisByUnit tries to find an axis matching the given unit
     * @param dataSeriesUnit: Dataseries unit of measurement
     * @return pointer to the found axis, nullptr if no axis was found
     */
    QValueAxis* findAxisByUnit(std::string dataSeriesUnit);

    /**
     * @brief distributeAxes distributes y-axes so they wont stack on one side
     */
    void distributeAxes();

    /**
     * @brief changeAxisAlignment changes axis alignment to the given one
     * @param axis: The axis whose alignment will be changed
     * @param series: The series the axis will be attached to
     * @param newAlignment: The alignment which will be set for the axis
     */
    void changeAxisAlignment(QValueAxis *axis, QAbstractSeries *series, QFlag newAlignment);

    /**
     * @brief updateDateAxisX updates dateAxisX with new max and min values
     * @param newMinX: New minimum value in seconds
     * @param newMaxX: New maximum value in seconds
     */
    void updateDateAxisX(int newMinX, int newMaxX);

    /**
     * @brief updateAxisColor updates the color for the given axis
     * @param axis: Axis whose color will be updated
     */
    void updateAxisColor(QValueAxis* axis);

    /**
     * @brief attachedSeries finds DataSeries attached to the given axis
     * @param axis: Axis the searched series are attached to
     * @return series attached to the given axis
     */
    std::vector<DataSeries> attachedSeries(QValueAxis* axis);

    /**
     * @brief countShownSeries counts the amount of series shown on the graph
     * for the given axis
     * @param axis: Axis the shown series are counted for
     * @return how many series are shown for the gihorizontal axis which displays the bar categoriesven axis
     */
    int countShownSeries(QValueAxis* axis);

    /**
     * @brief countUniqueAxisY counts the amount of unique axes on the graph
     * @return how many unique axes are on the graph
     */
    int countUniqueAxisY();


    /**
     * @brief weatherData_ stores the data which will be plotted
     */
    std::map<std::string, DataSeries> weatherData_;

    /**
     * @brief startDate_ stores the startdate used as an anchor point
     */
    QDateTime startDate_;

    /**
     * @brief valueAxisX_ stores the hidden X value axis that every series gets
     * attached to
     */
    QValueAxis* valueAxisX_;

    /**
     * @brief dateAxisX_ stores the X date axis that is displayed on the graph
     */
    QDateTimeAxis* dateAxisX_;

};

#endif // WEATHERGRAPH_HH
