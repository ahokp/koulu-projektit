/**
  * @file weatherchartbase.cpp implements the WeatherChartBase class
  * @author Johannes Simulainen 284213 johannes.simulainen@tuni.fi
  * @date 12.4.2021
  */

#include "weatherchartbase.hh"

WeatherChartBase::WeatherChartBase()
{
    initGraphBackground();
}

void WeatherChartBase::initGraphBackground()
{
    setBackgroundBrush(BACKGROUND_COLOR);
    setBackgroundRoundness(0);
    legend()->setVisible(true);
    legend()->setAlignment(Qt::AlignTop);
    legend()->attachToChart();

}
