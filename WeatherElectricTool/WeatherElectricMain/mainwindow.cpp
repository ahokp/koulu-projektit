/**
  * @file mainwindow.cpp implements the MainWindow class.
  * @author Petrus Jussila 283047 petrus.jussila@tuni.fi
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @author Paul Ahokas 283302 paul.ahokas@tuni.fi
  * @date 12.4.2021
  */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addgraphform.h"
#include "weathergraph.hh"
#include "dataconnector.h"
#include "datasourcewidget.hh"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow), dataConnector_(new DataConnector()),
      weatherGraph_(new WeatherGraph(QDateTime::fromSecsSinceEpoch(0)))
{
    ui_->setupUi(this);

    // Setup data source scroll area
    QWidget* centralWidget = new QWidget();
    dataSourceBoxLayout_ = new QVBoxLayout(centralWidget);

    weatherPie_ = new WeatherPie();
    weatherBar_ = new WeatherBar();

    ui_->dataSourceScrollArea->setWidget(centralWidget);
    ui_->chartView->setChart(weatherGraph_);
    ui_->chartView->setRenderHint(QPainter::Antialiasing);

    ui_->startDateTimeEdit->setDateTime(dataConnector_->getStartDate());
    ui_->endDateTimeEdit->setDateTime(dataConnector_->getEndDate());
    ui_->firstValueTextBrowser->setText(dataConnector_->getStartDate().toString());
    ui_->secondValueTextBrowser->setText(dataConnector_->getEndDate().toString());

    connect(dataConnector_, &DataConnector::data_saved,
            this, &MainWindow::passDataToGraph);

    connect(dataConnector_, &DataConnector::addToSeries,
            this, &MainWindow::passPointsToGraph);

    connect(dataConnector_, &DataConnector::delFromSeries,
            this, &MainWindow::passDelInfoToGraph);

    connect(dataConnector_, &DataConnector::reAddSeries,
            this, &MainWindow::reAddGraphSeries);

    connect(dataConnector_, &DataConnector::updateCharts,
            this, &MainWindow::updateChartValues);

    connect(dataConnector_, &DataConnector::addSourceWidget,
            this, &MainWindow::createDataSourceWidget);
}

MainWindow::~MainWindow()
{
    delete ui_;
    delete dataConnector_;
    delete weatherGraph_;
}

void MainWindow::createDataSourceWidget(const DataSourceDetails &sourceDetails)
{
    auto passedDataNamesIter = passedGraphNames_.find(
                sourceDetails.graphName);

    // Don't add duplicate data sources
    if (passedDataNamesIter != passedGraphNames_.end())
    {
        return;
    }

    DataSourceWidget* dataSourceWidget = new DataSourceWidget(sourceDetails);

    connect(dataSourceWidget, &DataSourceWidget::removed,
            [this, sourceDetails, dataSourceWidget]()
    {
        // Only delete other things if graph deletion is successful
        if (weatherGraph_->deleteActiveSeries(
                    sourceDetails.graphName))
        {
            weatherPie_->deleteActiveSeries(sourceDetails.graphName);
            weatherBar_->deleteActiveSeries(sourceDetails.graphName);
            dataConnector_->removeActiveDataSource(sourceDetails);

            dataSourceBoxLayout_->removeWidget(dataSourceWidget);
            passedGraphNames_.erase(
                        passedGraphNames_.find(sourceDetails.graphName));

            delete dataSourceWidget;
            ui_->addGraphButton->setEnabled(true);
        }
    });

    connect(dataSourceWidget, &DataSourceWidget::editedScaleMaximum,
            [this, sourceDetails](double d)
    {
        weatherGraph_->scaleAxis(sourceDetails.graphName, d, true);
    });

    connect(dataSourceWidget, &DataSourceWidget::editedScaleMinimum,
            [this, sourceDetails](double d)
    {
        weatherGraph_->scaleAxis(sourceDetails.graphName, d, false);
    });


    connect(dataSourceWidget, &DataSourceWidget::changedState,
            [this, sourceDetails](int state)
    {
        weatherGraph_->hideSeries(sourceDetails.graphName, (bool)state);
        weatherPie_->hideSeries(sourceDetails.graphName, (bool)state);
        weatherBar_->hideSeries(sourceDetails.graphName, (bool)state);
    });

    dataSourceBoxLayout_->addWidget(dataSourceWidget);

    // Tell DataConnector about the new active data source so that it can
    // import its data
    dataConnector_->addActiveDataSource(sourceDetails);

    if (dataSourceBoxLayout_->count() >= MAX_SERIES) {ui_->addGraphButton->setEnabled(false);}
}

DataConnector* MainWindow::getDataConnector() const
{
    return dataConnector_;
}

void MainWindow::on_addGraphButton_clicked()
{
    addGraphForm_ = new AddGraphForm(this);
    addGraphForm_->show();
}

void MainWindow::passDataToGraph()
{
    auto data = dataConnector_->getData();

    for (auto it = data.second.begin(); it != data.second.end(); it++)
    {
        // getData returns every data series ever imported: only pass the ones
        // that haven't already been passed
        if (passedGraphNames_.find(it->first) == passedGraphNames_.end())
        {
            weatherPie_->addActiveSeries(*it);
            weatherGraph_->addActiveSeries(*it);
            weatherBar_->addActiveSeries(*it);
            passedGraphNames_.insert(it->first);
        }
    }
}

void MainWindow::passPointsToGraph(std::string dataSeriesName, DataSeries addedSeries, bool addBefore)
{
    weatherGraph_->addPointsToSeries(dataSeriesName, addedSeries, addBefore);
}

void MainWindow::passDelInfoToGraph(std::string dataSeriesName, int delIndex, bool delBefore)
{
    weatherGraph_->removePointsFromSeries(dataSeriesName, delIndex, delBefore);
}

void MainWindow::updateChartValues(std::string dataSeriesName, float newMagnitude, float newMaxY, float newMinY)
{
    weatherPie_->changeSliceMagnitude(dataSeriesName, newMagnitude);
    weatherBar_->changeBarLength(dataSeriesName, newMinY, newMaxY);
}

void MainWindow::on_refreshButton_clicked()
{
    // Check if data changed is eligible
    if (ui_->startDateTimeEdit->dateTime() >= ui_->endDateTimeEdit->dateTime()) {

        QMessageBox::information(
                    this, tr("Weather&Electricity App"), tr("Incomparable parameters" "\nStarting date cannot be later than ending date"));
    }

    else {
        dataConnector_->setBoundaryDates(ui_->startDateTimeEdit->dateTime(),
                                         ui_->endDateTimeEdit->dateTime());

        ui_->firstValueTextBrowser->setText(ui_->startDateTimeEdit->dateTime().toString());
        ui_->secondValueTextBrowser->setText(ui_->endDateTimeEdit->dateTime().toString());
    }
}

void MainWindow::reAddGraphSeries(DataSourceDetails dataSource)
{
    weatherGraph_->deleteActiveSeries(dataSource.graphName);
    weatherPie_->deleteActiveSeries(dataSource.graphName);
    weatherBar_->deleteActiveSeries(dataSource.graphName);
    passedGraphNames_.erase(passedGraphNames_.find(dataSource.graphName));
}

void MainWindow::on_saveView_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Data File",
                                                    QString(), "XML files (*.xml);;All files (*)");
    if (fileName.isEmpty())
        {return;}

    dataConnector_->saveCurrentDataSets(fileName);
}

void MainWindow::on_loadView_clicked()
{
    if(dataConnector_->getActiveDataSources().size() != 0)
    {
        QMessageBox::warning(this, "Unable to load view",
                             "You must remove active charts before loading a view!");
    }
    else
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Data File",
                                                        QString(), "XML files (*.xml);;All files (*)");

        if (fileName.isEmpty())
            {return;}

        dataConnector_->loadDataSets(fileName);

        QDateTime startDate = dataConnector_->getStartDate();
        QDateTime endDate = dataConnector_->getEndDate();
        // Set new date values to boxes
        ui_->startDateTimeEdit->setDateTime(startDate);
        ui_->endDateTimeEdit->setDateTime(endDate);
        ui_->firstValueTextBrowser->setText(startDate.toString());
        ui_->secondValueTextBrowser->setText(endDate.toString());
    }
}

void MainWindow::on_savePreference_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Data File",
                                                    QString(), "XML files (*.xml);;All files (*)");
    if (fileName.isEmpty())
        {return;}

    dataConnector_->savePreference(fileName);
}

void MainWindow::on_loadPreference_clicked()
{
    if(dataConnector_->getActiveDataSources().size() != 0)
    {
        QMessageBox::warning(this, "Unable to load preference",
                             "You must remove active charts before loading preferences!");
    }
    else
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Load Data File",
                                                        QString(), "XML files (*.xml);;All files (*)");

        if (fileName.isEmpty())
            {return;}

        dataConnector_->loadPreference(fileName);
    }
}

void MainWindow::on_screenshotButton_clicked()
{
    if (passedGraphNames_.size() == 0) {
        if (QMessageBox::No == QMessageBox::warning(this,
                                 "Empty Chart", "No charts found. Continue ? ",
                                 QMessageBox::Yes|QMessageBox::No))
        {return;}
    }
    QPixmap screenshot = ui_->chartView->grab();
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Screenshot", QString(), "Images (*.png)");
    if (!fileName.isEmpty())
        {screenshot.save(fileName + ".png");}
}

void MainWindow::on_chartComboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == "Line Chart") {
        ui_->chartView->setChart(weatherGraph_);
    }

    else if (arg1 == "Pie Chart") {
        ui_->chartView->setChart(weatherPie_);
    }

    else if (arg1 == "Bar Chart") {
        ui_->chartView->setChart(weatherBar_);
    }
}
