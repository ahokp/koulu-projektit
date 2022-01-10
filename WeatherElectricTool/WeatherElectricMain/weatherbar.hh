/**
  * @file weatherbar.hh declares the WeatherBar class, which is used to
  * display the given data series as a bar chart
  * @author Johannes Simulainen 284213 johannes.simulainen@tuni.fi
  * @date 12.4.2021
  */

#ifndef WEATHERBAR_HH
#define WEATHERBAR_HH

#include "weatherchartbase.hh"

namespace Ui { class WeatherBar; }

const QStringList BAR_CATEGORIES = {"Minimum Value", "Maximum Value"};

/**
 * @brief The WeatherPie class initializes the given series min and max values
 * as a bar chart
 */
class WeatherBar : public WeatherChartBase
{
    Q_OBJECT

public:

    /**
     * @brief WeatherBar the default constructor
     */
    WeatherBar();

    /**
     * @brief addActiveSeries adds a given data series to the graph
     * @param seriesPair: Data to be added
     * @return true if data was successfully added, false otherwise
     */
    virtual bool addActiveSeries(std::pair<std::string, DataSeries> seriesPair) override;

    /**
     * @brief deleteActiveSeries removes the given series from the graph and
     * deletes the data/attached axes
     * @param dataSeriesName: Name of the data series that will be removed
     * @return true if data was successfully deleted, false otherwise
     */
    virtual bool deleteActiveSeries(std::string dataSeriesName) override;

    /**
     * @brief hideSeries hides/shows the given series from the graph temporarily
     * @param dataSeriesName: Name of the data series that will be hidden
     * @param hide: If the graph is to be hidden or shown
     * @return true if data was successfully hidden, false otherwise
     */
    virtual bool hideSeries(std::string dataSeriesName, bool hide) override;

    /**
     * @brief changeBarLength changes the bar length to given values
     * @param dataSeriesName: Series whose length will be changed
     * @param min: The new minimum value
     * @param max: The new maximum value
     * @return true if bar length was changed successfully
     */
    bool changeBarLength(std::string dataSeriesName, float min, float max);

private:

    /**
     * @brief determineSeriesColor finds an unused color from SERIESCOLORS
     * @return currently unused color or default color if none were found
     */
    QColor determineSeriesColor() override;

    /**
     * @brief createBarAxisX creates a horizontal axis which displays the bar categories
     */
    void createBarAxisX();

    /**
     * @brief createValueAxisY creates a vertical axis which displays the bar values
     */
    void createValueAxisY();

    /**
     * @brief updateValueAxisY updates the y-axis values when needed
     */
    void updateValueAxisY();

    /**
     * @brief getChartMinMaxY gets the smallest and largest values from the chart
     * @return chart's min and max values
     */
    std::pair<float, float> getChartMinMaxY();

    /**
     * @brief barSeries_ stores the bar series drawn on the chart
     */
    QBarSeries* barSeries_;

    /**
     * @brief allSets_ stores all bar sets currently in the chart
     */
    std::map<std::string, QBarSet*> allSets_;

    /**
     * @brief barAxisX_ stores the horizontal axis which displays the bar categories
     */
    QBarCategoryAxis* barAxisX_;

    /**
     * @brief barAxisY_ stores the vertical axis which displays the bar values
     */
    QValueAxis* barAxisY_;

};

#endif // WEATHERBAR_HH
