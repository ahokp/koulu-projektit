/**
  * @file mainwindow.h declares the MainWindow class, which is used as the main
  * window of the application.
  * @author Petrus Jussila 283047 petrus.jussila@tuni.fi
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @author Paul Ahokas 283302 paul.ahokas@tuni.fi
  * @date 3.4.2021
  */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "addgraphform.h"

#include <QMainWindow>
#include <QEvent>

#include <unordered_set>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class AddGraphForm;
class WeatherGraph;

/**
 * @brief The MainWindow class is the main window of the application that shows
 * the graphs and graph controls inside it.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief The default constructor.
     * @param parent: The QWidget to parent this MainWindow to
     */
    MainWindow(QWidget* parent = nullptr);

    /**
      * @brief The default destructor.
      */
    ~MainWindow();

    /**
     * @brief createDataSourceWidget creates a data source widget for the given
     * data source details. These widgets are the boxes shown on the left side
     * of the window.
     * @param sourceDetails: The data source details to show on this widget
     */
    void createDataSourceWidget(const DataSourceDetails& sourceDetails);

    /**
     * @brief getDataConnector returns the DataConnector used by this
     * MainWindow.
     * @return The DataConnector used by this MainWindow
     */
    DataConnector* getDataConnector() const;

private slots:
    /**
     * @brief on_addGraphButton_clicked is called when the "Add Graph"-button
     * on the main window is clicked.
     */
    void on_addGraphButton_clicked();

    /**
     * @brief on_refreshButton_clicked is called when the "Refresh"-button
     * on the main window is clicked.
     */
    void on_refreshButton_clicked();

    void reAddGraphSeries(DataSourceDetails dataSource);

    void on_saveView_clicked();

    void on_loadView_clicked();

    void on_savePreference_clicked();

    void on_loadPreference_clicked();

    void on_screenshotButton_clicked();

    void on_chartComboBox_currentTextChanged(const QString &arg1);

private:
    // Stores the UI of this MainWindow
    Ui::MainWindow* ui_;

    // Stores the AddGraphForm of this MainWindow when it's shown
    AddGraphForm* addGraphForm_;

    // Stores a pointer to the UI layout of the data source widgets on the left
    QLayout* dataSourceBoxLayout_;

    // Stores the DataConnector used by this MainWindow
    DataConnector* dataConnector_;

    // Stores the WeatherGraph shown on this MainWindow
    WeatherGraph* weatherGraph_;

    // Stores the WeatherPie shown on this MainWindow
    WeatherPie* weatherPie_;

    // Stores the WeatherBar shown on this MainWindow
    WeatherBar* weatherBar_;

    // Stores what graph names have been passed to WeatherGraph so that they
    // won't be passed again
    std::unordered_set<std::string> passedGraphNames_;

    /**
     * @brief passDataToGraph passes newly imported data from dataConnector_ to
     * weatherGraph_.
     */
    void passDataToGraph();

    /**
     * @brief passPointsToSeries passes series of points from dataConnector_ to
     * weatherGraph_ that will be added to an active series.
     * @param dataSeriesName is the name of the active series.
     * @param addedSeries is the series that will be added the active series.
     * @param addBefore indicates if addedSeries will be added to the front or
     * back of the active series.
     */
    void passPointsToGraph(std::string dataSeriesName, DataSeries addedSeries,
                           bool addBefore);

    /**
     * @brief passDelInfoToGraph passes information from dataConnector_ to
     * weatherGraph_ of how points will be removed from active series.
     * @param dataSeriesName is the name of active series from which points will
     * be removed.
     * @param delIndex decides the index where point removal starts/ends.
     * @param delBefore indicates if point removal starts or ends at delIndex.
     */
    void passDelInfoToGraph(std::string dataSeriesName, int delIndex, bool delBefore);

    /**
     * @brief updateChartValues updates active charts with new values.
     * @param dataSeriesName is the name of the series which recieve updates.
     * @param newMagnitude is a new value for magnitude.
     * @param newMaxY is a new value for highest Y value.
     * @param newMinY is a new value for smallest Y value.
     */
    void updateChartValues(std::string dataSeriesName, float newMagnitude, float newMaxY, float newMinY);
};

#endif // MAINWINDOW_H
