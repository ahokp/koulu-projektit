/**
  * @file weatherpie.hh declares the WeatherPie class, which is used to
  * display the given data series as a pie chart
  * @author Johannes Simulainen 284213 johannes.simulainen@tuni.fi
  * @date 12.4.2021
  */

#ifndef WEATHERPIE_HH
#define WEATHERPIE_HH

#include "weatherchartbase.hh"

namespace Ui { class WeatherPie; }

/**
 * @brief The WeatherPie class initializes the given series magnitudes as a pie
 * chart
 */
class WeatherPie : public WeatherChartBase
{
    Q_OBJECT
public:

    /**
     * @brief WeatherPie the default constructor
     */
    WeatherPie();

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
     * @brief changeSliceMagnitude changes the magnitude of the given slice
     * @param dataSeriesName: Slices whose magnitude will be changed
     * @param newMagnitude: New magnitude that the slice will be changed to
     * @return true if the magnitude was changed successfully, false otherwise
     */
    bool changeSliceMagnitude(std::string dataSeriesName, float newMagnitude);

private:

    /**
     * @brief determineSeriesColor finds an unused color from SERIESCOLORS
     * @return currently unused color or default color if none were found
     */
    QColor determineSeriesColor() override;

    /**
     * @brief updateSliceLabels updates the slice labels when they are changed
     */
    void updateSliceLabels();

    /**
     * @brief pieSeries_ stores the pie series displayed on the chart
     */
    QPieSeries* pieSeries_;

    /**
     * @brief allSlices_ stores all slices currently in the chart
     */
    std::map<std::string, QPieSlice*> allSlices_;
};

#endif // WEATHERPIE_HH
