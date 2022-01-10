/**
  * @file weatherbar.cpp implements the WeatherBar class
  * @author Johannes Simulainen 284213 johannes.simulainen@tuni.fi
  * @date 12.4.2021
  */

#include "weatherbar.hh"

WeatherBar::WeatherBar()
{
    barSeries_ = new QBarSeries();
    addSeries(barSeries_);

    createBarAxisX();
    createValueAxisY();
    setAnimationOptions(QChart::SeriesAnimations);

    connect(barSeries_, &QBarSeries::countChanged, this, &WeatherBar::updateValueAxisY);
}

bool WeatherBar::addActiveSeries(std::pair<std::string, DataSeries> seriesPair)
{
    if (allSets_.size() >= MAX_SERIES){
        return false;
    }

    QBarSet* barSet = new QBarSet(QString::fromStdString(seriesPair.first));

    *barSet << seriesPair.second.minY << seriesPair.second.maxY;

    barSet->setBrush(QBrush(determineSeriesColor()));
    barSet->setPen(QPen(DEFAULT_COLOR, 1));

    allSets_.insert({seriesPair.first, barSet});
    barSeries_->append(barSet);

    return true;
}

bool WeatherBar::deleteActiveSeries(std::string dataSeriesName)
{
    auto it = allSets_.find(dataSeriesName);
    if (it == allSets_.end()){
        return false;
    }

    allSets_.erase(it);
    barSeries_->remove(it->second);

    return true;
}

bool WeatherBar::hideSeries(std::string dataSeriesName, bool hide)
{
    auto it = allSets_.find(dataSeriesName);
    if (it == allSets_.end()){
        return false;
    }

    QBarSet* barSet = it->second;

    if (hide){
        barSeries_->take(barSet);
    }
    else {
        barSeries_->append(barSet);
    }

    return true;
}

bool WeatherBar::changeBarLength(std::string dataSeriesName, float min, float max)
{
    auto it = allSets_.find(dataSeriesName);
    if (it == allSets_.end()){
        return false;
    }

    it->second->replace(0, min);
    it->second->replace(1, max);
    updateValueAxisY();

    return true;
}

QColor WeatherBar::determineSeriesColor()
{
    std::vector<QColor> lineColors = SERIESCOLORS;

    for (auto barSet : allSets_){
        for (int i = 0; i < int(lineColors.size()); i++){
            if (barSet.second->brush().color() == lineColors[i]){
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

void WeatherBar::createBarAxisX()
{
    barAxisX_ = new QBarCategoryAxis();

    barAxisX_->append(BAR_CATEGORIES);
    barAxisX_->setLinePen(AXIS_PEN);
    barAxisX_->setGridLineColor(AXIS_VERTICAL_GRID_COLOR);

    addAxis(barAxisX_, Qt::AlignBottom);
    barSeries_->attachAxis(barAxisX_);
}

void WeatherBar::createValueAxisY()
{
    barAxisY_ = new QValueAxis();

    barAxisY_->setRange(-10, 20);
    barAxisY_->setLinePen(AXIS_PEN);
    barAxisY_->setGridLineColor(AXIS_HORIZONTAL_GRID_COLOR);

    addAxis(barAxisY_, Qt::AlignLeft);
    barSeries_->attachAxis(barAxisY_);
}

void WeatherBar::updateValueAxisY()
{
    std::pair<float, float> graphValues = getChartMinMaxY();

    float buffer = (graphValues.second - graphValues.first) * BUFFER_RATIO;

    barAxisY_->setRange(floor(graphValues.first - buffer),
                        ceil(graphValues.second + buffer));
}

std::pair<float, float> WeatherBar::getChartMinMaxY()
{
    float smallestY = std::numeric_limits<float>::max();
    float largestY = std::numeric_limits<float>::min();

    for (auto barSet : barSeries_->barSets()){
        float minY = barSet->at(0);
        float maxY = barSet->at(1);

        smallestY = std::min(smallestY, minY);
        largestY = std::max(largestY, maxY);
    }

    return {smallestY, largestY};
}
