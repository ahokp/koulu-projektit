/**
  * @file weatherchartbase.hh declares the WeatherChartBase class, which is used
  * in different charts as a base class
  * @author Johannes Simulainen 284213 johannes.simulainen@tuni.fi
  * @date 12.4.2021
  */

#ifndef WEATHERCHARTBASE_HH
#define WEATHERCHARTBASE_HH

#include <QtCharts>
#include <limits>

namespace Ui { class WeatherChartBase; }

/**
 * @brief The DataSeries struct defines the DataSeries handled by different charts
 */
struct DataSeries {
    std::string unitOfMeasurement;
    QSplineSeries* splineSeries;
    QValueAxis* axisY;
    float maxY;
    float minY;
    float magnitude;
};

// Maximum number of series that will be displayed
const int MAX_SERIES = 4;

// RGB color codes for different components of the graph
const QColor BACKGROUND_COLOR(202, 218, 229);
// Red
const QColor SERIES_COLOR1(255, 54, 34);
// Blue
const QColor SERIES_COLOR2(34, 54, 255);
// Green
const QColor SERIES_COLOR3(59, 162, 0);
// Purple
const QColor SERIES_COLOR4(188, 70, 255);
// Black
const QColor DEFAULT_COLOR(0, 0, 0);
const std::vector<QColor> SERIESCOLORS{SERIES_COLOR1, SERIES_COLOR2, SERIES_COLOR3, SERIES_COLOR4};

// Orange
const QColor AXIS_COLOR(255,155,50);

// Blueish grey
const QColor AXIS_HORIZONTAL_GRID_COLOR(179,192,200);

// Blueish grey
const QColor AXIS_VERTICAL_GRID_COLOR(179,192,200);

const QPen AXIS_PEN = QPen(QBrush(AXIS_COLOR), 2);

// Defines the cutoff in seconds on how DateTimeAxis will display it's time
// MM * dd * HH * mm * ss
const int DAY_FORMAT_BOUNDARY = 3*24*60*60;
const int MONTH_FORMAT_BOUNDARY = 5*30*24*60*60;

// Defines the size of the buffer relative to the range
const float BUFFER_RATIO = 0.1;

/**
 * @brief The WeatherChartBase class defines the base class used by the program's
 * charts
 */
class WeatherChartBase : public QChart
{
    Q_OBJECT
public:

    /**
     * @brief addActiveSeries adds a given data series to the graph
     * @param seriesPair: Data to be added
     * @return true if data was successfully added, false otherwise
     */
    virtual bool addActiveSeries(std::pair<std::string, DataSeries> seriesPair) = 0;

    /**
     * @brief deleteActiveSeries removes the given series from the graph and
     * deletes the data/attached axes
     * @param dataSeriesName: Name of the data series that will be removed
     * @return true if data was successfully deleted, false otherwise
     */
    virtual bool deleteActiveSeries(std::string dataSeriesName) = 0;

    /**
     * @brief hideSeries hides/shows the given series from the graph temporarily
     * @param dataSeriesName: Name of the data series that will be hidden
     * @param hide: If the graph is to be hidden or shown
     * @return true if data was successfully hidden, false otherwise
     */
    virtual bool hideSeries(std::string dataSeriesName, bool hide) = 0;

protected:

    /**
     * @brief determineSeriesColor finds an unused color from SERIESCOLORS
     * @return currently unused color or default color if none were found
     */
    virtual QColor determineSeriesColor() = 0;

    /**
     * @brief WeatherChartBase the default constructor
     */
    WeatherChartBase();

private:
    /**
     * @brief initGraphBackground initializes the graph background characteristics
     */
    void initGraphBackground();

};

#endif // WEATHERCHARTBASE_HH
