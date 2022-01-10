/**
  * @file weatherpie.cpp implements the WeatherPie class
  * @author Johannes Simulainen 284213 johannes.simulainen@tuni.fi
  * @date 12.4.2021
  */

#include "weatherpie.hh"
#include <iostream>

WeatherPie::WeatherPie() : WeatherChartBase()
{
    pieSeries_ = new QPieSeries();
    addSeries(pieSeries_);

    pieSeries_->setLabelsPosition(QPieSlice::LabelInsideHorizontal);
    setAnimationOptions(QChart::SeriesAnimations);

    connect(pieSeries_, &QPieSeries::countChanged, this, &WeatherPie::updateSliceLabels);
}

bool WeatherPie::addActiveSeries(std::pair<std::string, DataSeries> seriesPair)
{
    if (allSlices_.size() >= MAX_SERIES){
        return false;
    }

    QPieSlice* slice = new QPieSlice(QString::fromStdString(seriesPair.first),
                                     seriesPair.second.magnitude);

    slice->setBrush(QBrush(determineSeriesColor()));
    slice->setPen(QPen(DEFAULT_COLOR, 1));
    slice->setLabelVisible();

    allSlices_.insert({seriesPair.first, slice});
    pieSeries_->append(slice);

    return true;
}

bool WeatherPie::deleteActiveSeries(std::string dataSeriesName)
{
    auto it = allSlices_.find(dataSeriesName);
    if (it == allSlices_.end()){
        return false;
    }

    allSlices_.erase(it);
    pieSeries_->remove(it->second);

    return true;
}

bool WeatherPie::hideSeries(std::string dataSeriesName, bool hide)
{
    auto it = allSlices_.find(dataSeriesName);
    if (it == allSlices_.end()){
        return false;
    }

    QPieSlice* slice = it->second;

    if (hide){
        pieSeries_->take(slice);
    }
    else {
        pieSeries_->append(slice);
    }

    return true;
}

bool WeatherPie::changeSliceMagnitude(std::string dataSeriesName, float newMagnitude)
{
    auto it = allSlices_.find(dataSeriesName);
    if (it == allSlices_.end()){
        return false;
    }

    it->second->setValue(newMagnitude);
    updateSliceLabels();

    return true;
}

QColor WeatherPie::determineSeriesColor()
{

    std::vector<QColor> lineColors = SERIESCOLORS;

    for (auto pieSlice : allSlices_){
        for (int i = 0; i < int(lineColors.size()); i++){
            if (pieSlice.second->brush().color() == lineColors[i]){
                lineColors.erase(lineColors.begin() + i);
                i--;
            }
        }
    }

    if (lineColors.size() > 0){
        return lineColors[0];
    }
    else {
        return DEFAULT_COLOR;
    }
}

void WeatherPie::updateSliceLabels()
{
    for (auto slice : allSlices_){
        QString label = QString::fromStdString(slice.first) + " " +
                QString::number(slice.second->percentage() * 100, 'f', 2) + "%";
        slice.second->setLabel(label);
    }
}
