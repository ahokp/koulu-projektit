/**
  * @file weathergraph.cpp implements the WeatherGraph class
  * @author Johannes Simulainen 284213 johannes.simulainen@tuni.fi
  * @date 12.3.2021
  */

#include "weathergraph.hh"

WeatherGraph::WeatherGraph(QDateTime startDate) :
    WeatherChartBase(),
    startDate_(startDate)
{
    initGraphAxis();
}

WeatherGraph::~WeatherGraph()
{
    delete valueAxisX_;
    delete dateAxisX_;
}

bool WeatherGraph::addActiveSeries(std::pair<std::string, DataSeries> seriesPair)
{
    if (weatherData_.size() >= MAX_SERIES){
        return false;
    }

    QSplineSeries* splineSeries = seriesPair.second.splineSeries;

    if (splineSeries->points().size() == 0){
        return false;
    }

    splineSeries->setName(QString::fromStdString(seriesPair.first));
    splineSeries->setPen(QPen(determineSeriesColor(), 2, Qt::SolidLine, Qt::RoundCap));

    addSeries(splineSeries);
    splineSeries->attachAxis(valueAxisX_);

    // Update x-axes
    std::pair<int, int> graphValues = calcGraphX(series());
    valueAxisX_->setRange(graphValues.first, graphValues.second);
    updateDateAxisX(graphValues.first, graphValues.second);

    QValueAxis* axisY = findAxisByUnit(seriesPair.second.unitOfMeasurement);
    seriesPair.second.axisY = axisY;

    // Add new axis if added series has a new unit of measure, update existing
    // axis otherwise
    if (axisY == nullptr){
        float buffer = calcBuffer(seriesPair.second.minY, seriesPair.second.maxY);
        QValueAxis *axisY = createValueAxisY(seriesPair.second,
                                             seriesPair.second.minY - buffer,
                                             seriesPair.second.maxY + buffer);
        seriesPair.second.axisY = axisY;

        axisY->setGridLineColor(AXIS_HORIZONTAL_GRID_COLOR);
        addAxis(axisY, ALIGNMENT_Y[countUniqueAxisY()%2]);
        splineSeries->attachAxis(axisY);
    }
    else {
        splineSeries->attachAxis(axisY);

        // Compare current axis values to new one and update accordingly
        std::pair<float, float> minMax = findMinMaxAttachedToAxis(axisY);
        float minY = minMax.first;
        float maxY = minMax.second;

        float smallestY = std::min(minY, seriesPair.second.minY);
        float largestY = std::max(maxY, seriesPair.second.maxY);
        float buffer = calcBuffer(smallestY, largestY);

        axisY->setRange((smallestY - buffer), (largestY + buffer));
    }

    weatherData_.insert({seriesPair.first, seriesPair.second});

    return true;
}

bool WeatherGraph::deleteActiveSeries(std::string dataSeriesName)
{
    auto it = weatherData_.find(dataSeriesName);
    if (it == weatherData_.end()){
        return false;
    }
    QSplineSeries* dataSeries = it->second.splineSeries;
    QValueAxis* axisY = it->second.axisY;

    weatherData_.erase(dataSeriesName);
    delete dataSeries;

    // If axis has no attached series delete it, update range otherwise
    if (attachedSeries(axisY).size() == 0){
        delete axisY;
        distributeAxes();
    }
    else {
        std::pair<float, float> minMax = findMinMaxAttachedToAxis(axisY);
        float minY = minMax.first;
        float maxY = minMax.second;

        float buffer = calcBuffer(minY, maxY);

        axisY->setRange(minY - buffer, maxY + buffer);
        updateAxisColor(axisY);
    }

    // Update graph x-axes
    std::pair<int, int> graphValues = calcGraphX(series());
    valueAxisX_->setRange(graphValues.first, graphValues.second);
    updateDateAxisX(graphValues.first, graphValues.second);

    return true;
}

bool WeatherGraph::hideSeries(std::string dataSeriesName, bool hide)
{
    DataSeries* currentSeries = &weatherData_[dataSeriesName];
    QSplineSeries* currentSplineSeries = currentSeries->splineSeries;
    QValueAxis* seriesAxis = currentSeries->axisY;

    if (hide){
        if (countShownSeries(seriesAxis) == 1){
            seriesAxis->hide();
        }

        currentSplineSeries->hide();

    }
    else {
        currentSplineSeries->show();
        seriesAxis->show();
    }

    return true;
}


void WeatherGraph::addPointsToSeries(std::string dataSeriesName,
                                     DataSeries addedSeries, bool addBefore)
{
    DataSeries* currentSeries = &weatherData_[dataSeriesName];
    QSplineSeries* oldSplineSeries = currentSeries->splineSeries;
    QSplineSeries* newSplineSeries = addedSeries.splineSeries;
    QValueAxis* seriesAxis = currentSeries->axisY;

    // Add points to series
    if (addBefore){
        *newSplineSeries << oldSplineSeries->points();

        addSeries(newSplineSeries);
        newSplineSeries->attachAxis(valueAxisX_);
        newSplineSeries->attachAxis(seriesAxis);
        newSplineSeries->setName(oldSplineSeries->name());
        newSplineSeries->setPen(oldSplineSeries->pen());

        currentSeries->splineSeries = newSplineSeries;

        delete oldSplineSeries;
    }
    else {
        *oldSplineSeries << newSplineSeries->points();
    }

    // Update dataSeries min and max Y
    if (addedSeries.maxY > currentSeries->maxY){
        currentSeries->maxY = addedSeries.maxY;
    }
    if (addedSeries.minY < currentSeries->minY){
        currentSeries->minY = addedSeries.minY;
    }

    // Update y-axis
    std::pair<float, float> minMax = findMinMaxAttachedToAxis(seriesAxis);
    float buffer = calcBuffer(minMax.first, minMax.second);

    seriesAxis->setRange(minMax.first - buffer, minMax.second + buffer);

    // Update both x-axes
    std::pair<int, int> graphValues = calcGraphX(series());

    valueAxisX_->setRange(graphValues.first, graphValues.second);
    updateDateAxisX(graphValues.first, graphValues.second);
}

void WeatherGraph::removePointsFromSeries(std::string dataSeriesName, int delIndex, bool delBefore)
{
    DataSeries* currentSeries = &weatherData_[dataSeriesName];
    QSplineSeries* currentSplineSeries = currentSeries->splineSeries;
    QValueAxis* seriesAxis = currentSeries->axisY;

    if (delBefore){
        currentSplineSeries->removePoints(0, delIndex);
    }
    else {
        currentSplineSeries->removePoints(delIndex, currentSplineSeries->count()-delIndex);
    }

    // Update DataSeries Y-values
    std::tuple<float, float> seriesValuesY = findSeriesMinMaxY(*currentSeries);

    currentSeries->minY = std::get<0>(seriesValuesY);
    currentSeries->maxY = std::get<1>(seriesValuesY);

    // Update Y-axis range
    std::pair<float, float> minMax = findMinMaxAttachedToAxis(seriesAxis);
    float buffer = calcBuffer(minMax.first, minMax.second);

    seriesAxis->setRange(minMax.first - buffer, minMax.second + buffer);

    // Update X-axes ranges
    std::pair<int, int> graphValues = calcGraphX(series());

    valueAxisX_->setRange(graphValues.first, graphValues.second);
    updateDateAxisX(graphValues.first, graphValues.second);
}

void WeatherGraph::scaleAxis(std::string dataSeriesName, double multiplier, bool maxValue)
{
    DataSeries* currentSeries = &weatherData_[dataSeriesName];
    QValueAxis* seriesAxis = currentSeries->axisY;

    std::pair<float, float> minMax = findMinMaxAttachedToAxis(seriesAxis);
    float buffer = calcBuffer(minMax.first, minMax.second);

    if (maxValue){
        seriesAxis->setMax((minMax.second + buffer)*multiplier);
    }
    else {
        seriesAxis->setMin((minMax.first - buffer)*multiplier);
    }
}

QColor WeatherGraph::determineSeriesColor()
{
    std::vector<QColor> lineColors = SERIESCOLORS;

    for (QAbstractSeries* series : series()){
        QSplineSeries* splineSeries = dynamic_cast<QSplineSeries*>(series);

        for (int i = 0; i < int(lineColors.size()); i++){
            if (splineSeries->pen().color() == lineColors[i]){
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

void WeatherGraph::initGraphAxis()
{
    QList<QAbstractSeries*> seriesList = series();

    // Create value axis that the series are attached to
    valueAxisX_ = new QValueAxis();

    valueAxisX_->setRange(0, 2);
    valueAxisX_->hide();
    addAxis(valueAxisX_, Qt::AlignBottom);

    // Create date axis which will be displayed on the chart
    dateAxisX_ = createDateAxisX(0, 2);
    dateAxisX_->setGridLineColor(AXIS_VERTICAL_GRID_COLOR);
    addAxis(dateAxisX_, Qt::AlignBottom);
}

QDateTimeAxis *WeatherGraph::createDateAxisX(int startTimeInSeconds, int endTimeInSeconds)
{
    QDateTimeAxis* dateAxis = new QDateTimeAxis();
    dateAxis->setRange(startDate_.addSecs(startTimeInSeconds), startDate_.addSecs(endTimeInSeconds));
    dateAxis->setTickCount(5);
    dateAxis->setLinePen(AXIS_PEN);

    QString formatString = "HH:mm";
    dateAxis->setFormat(formatString);

    return dateAxis;
}

QValueAxis *WeatherGraph::createValueAxisY(DataSeries series, float minY, float maxY)
{
    QValueAxis *axisY = new QValueAxis();

    axisY->setLabelsColor(series.splineSeries->pen().color());
    axisY->setTitleText(QString::fromStdString(series.unitOfMeasurement));
    axisY->setTitleBrush(QBrush(series.splineSeries->pen().color()));
    axisY->setRange(minY, maxY);
    axisY->setLinePen(AXIS_PEN);

    return axisY;
}

std::pair<int, int> WeatherGraph::calcGraphX(QList<QAbstractSeries*> seriesList)
{
    if (seriesList.size() == 0){
        return {0, 0};
    }

    float largestX = std::numeric_limits<float>::min();
    float smallestX = std::numeric_limits<float>::max();

    for (QAbstractSeries* series : seriesList){

        QSplineSeries* splineSeries = dynamic_cast<QSplineSeries*>(series);
        float maxX = splineSeries->at(splineSeries->count()-1).x();
        float minX = splineSeries->at(0).x();

        largestX = std::max(maxX, largestX);
        smallestX = std::min(minX, smallestX);
    }

    float buffer = calcBuffer(smallestX, largestX);

    return {floor(smallestX), ceil(largestX + buffer)};
}

float WeatherGraph::calcBuffer(float min, float max)
{
    return fabs((max - min) * BUFFER_RATIO);
}

std::pair<float, float> WeatherGraph::findMinMaxAttachedToAxis(QValueAxis *axis)
{
    std::vector<DataSeries> attached = attachedSeries(axis);
    float largestY = std::numeric_limits<float>::min();
    float smallestY = std::numeric_limits<float>::max();

    for (DataSeries series : attached){
        smallestY = std::min(series.minY, smallestY);
        largestY = std::max(series.maxY, largestY);
    }

    return {smallestY, largestY};
}

std::pair<float, float> WeatherGraph::findSeriesMinMaxY(const DataSeries &series)
{
    float largestY = std::numeric_limits<float>::min();
    float smallestY = std::numeric_limits<float>::max();

    for (QPointF dataPoint : series.splineSeries->points()){

        largestY = std::max(float(dataPoint.y()), largestY);
        smallestY = std::min(float(dataPoint.y()), smallestY);
    }

    return {smallestY, largestY};
}

QValueAxis *WeatherGraph::findAxisByUnit(std::string dataSeriesUnit)
{
    for (auto dataSeries : weatherData_){
        if (dataSeries.second.unitOfMeasurement == dataSeriesUnit){
            return dataSeries.second.axisY;
        }
    }

    return nullptr;
}

void WeatherGraph::distributeAxes()
{
    int left = 0, right = 0;
    std::vector<QValueAxis*> counted;

    for (QAbstractSeries* series : series()){
        for (QAbstractAxis* axis : series->attachedAxes()){
            QValueAxis* valueAxis = dynamic_cast<QValueAxis*>(axis);
            auto alignment = valueAxis->alignment();

            if (std::find(counted.begin(), counted.end(), valueAxis) != counted.end()){
                continue;
            }
            counted.push_back(valueAxis);

            if (alignment == Qt::AlignLeft){
                left += 1;
                if (left > right+1){
                    changeAxisAlignment(valueAxis, series, Qt::AlignRight);
                    left -= 1;
                    right += 1;
                }
            }
            else if (alignment == Qt::AlignRight){
                right += 1;
                if (right > left){
                    changeAxisAlignment(valueAxis, series, Qt::AlignLeft);
                    left += 1;
                    right -= 1;
                }
            }
        }
    }
}

void WeatherGraph::changeAxisAlignment(QValueAxis* axis, QAbstractSeries* series, QFlag newAlignment)
{
    removeAxis(axis);
    addAxis(axis, newAlignment);
    series->attachAxis(axis);
}

void WeatherGraph::updateDateAxisX(int newMinX, int newMaxX)
{
    if (newMinX == 0 and newMaxX == 0){
        dateAxisX_->setRange(startDate_.addSecs(0), startDate_.addSecs(2));
    }
    else {
        dateAxisX_->setRange(startDate_.addSecs(newMinX), startDate_.addSecs(newMaxX));
    }

    QString formatString = "HH:mm";
    if (DAY_FORMAT_BOUNDARY < (newMaxX - newMinX) and (newMaxX - newMinX) < MONTH_FORMAT_BOUNDARY){
        formatString = "dd/MM";
    }
    else if (MONTH_FORMAT_BOUNDARY < (newMaxX - newMinX)){
        formatString = "MM/yyyy";
    }

    dateAxisX_->setFormat(formatString);
}

void WeatherGraph::updateAxisColor(QValueAxis* axis)
{
    std::vector<DataSeries> attached = attachedSeries(axis);
    axis->setLabelsColor(attached[0].splineSeries->pen().color());
    axis->setTitleBrush(QBrush(attached[0].splineSeries->pen().color()));
}

std::vector<DataSeries> WeatherGraph::attachedSeries(QValueAxis* axis)
{
    std::vector<DataSeries> attached;

    for (auto dataSeries : weatherData_){
        if (dataSeries.second.axisY  == axis){
            attached.push_back(dataSeries.second);
        }
    }

    return attached;
}

int WeatherGraph::countShownSeries(QValueAxis* axis)
{
    int count = 0;

    for (auto dataSeries : weatherData_){
        if (dataSeries.second.axisY  == axis and dataSeries.second.splineSeries->isVisible()){
            count += 1;
        }
    }

    return count;
}

int WeatherGraph::countUniqueAxisY()
{
    int count = 0;
    std::vector<QValueAxis*> counted;

    for (QAbstractAxis* axis : axes()){
        if (axis->orientation() == Qt::Vertical){
            QValueAxis* valueAxis = dynamic_cast<QValueAxis*>(axis);

            if (std::find(counted.begin(), counted.end(), valueAxis) == counted.end()){
                counted.push_back(valueAxis);
                count += 1;
            }
        }
    }

    return count;
}
