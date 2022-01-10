/**
  * @file cachefilehandler.cpp implements the CacheFileHandler class
  * @author Paul Ahokas 283302 paul.ahokas@tuni.fi
  * @date 15.4.2021
  */

#include "cachefilehandler.h"


void CacheFileHandler::saveDataSet(std::pair<std::pair<QDateTime, QDateTime>, std::vector<DataSet>> savedDataSet, QString fileName)
{
    qint64 startDate = savedDataSet.first.first.toSecsSinceEpoch();
    qint64 endDate = savedDataSet.first.second.toSecsSinceEpoch();
    std::vector<DataSet> data = savedDataSet.second;

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("savedDataSets");

    xmlWriter.writeStartElement("startTime");
    xmlWriter.writeTextElement("secsSinceEpoch", QString::number(startDate));
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("endTime");
    xmlWriter.writeTextElement("secsSinceEpoch", QString::number(endDate));
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("graphs");

    // Go through each graph
    for(DataSet data_it : data)
    {
        xmlWriter.writeStartElement("graph");
        // DataSourceDetails data
        xmlWriter.writeTextElement("dataSourceName", QString::fromStdString(data_it.dataSource.dataSourceName));
        xmlWriter.writeTextElement("dataLocationName", QString::fromStdString(data_it.dataSource.dataLocationName));
        xmlWriter.writeTextElement("graphName", QString::fromStdString(data_it.dataSource.graphName));
        xmlWriter.writeTextElement("importerIndex", QString::number(data_it.dataSource.dataSourceIndex));
        xmlWriter.writeTextElement("apiDataType", QString::number(data_it.dataSource.dataType));

        xmlWriter.writeTextElement("hide", QString::number(data_it.hide));

        // DataSeries data
        xmlWriter.writeTextElement("unitOfMeasurement", QString::fromStdString(data_it.series.unitOfMeasurement));
        xmlWriter.writeTextElement("maxY", QString::number(data_it.series.maxY));
        xmlWriter.writeTextElement("minY", QString::number(data_it.series.minY));
        xmlWriter.writeTextElement("magnitude", QString::number(data_it.series.magnitude));

        xmlWriter.writeStartElement("data");
        QSplineSeries* series = data_it.series.splineSeries;
        // Go through each data point
        for(int i = 0; series->count() > i; i++)
        {
            float y = series->at(i).y();
            qint64 x = series->at(i).x();
            xmlWriter.writeStartElement("point");
            xmlWriter.writeTextElement("value", QString::number(y));
            xmlWriter.writeTextElement("secsSinceEpoch", QString::number(x));
            xmlWriter.writeEndElement();
        }
        // Close tag "data"
        xmlWriter.writeEndElement();
        // Close tag "graph"
        xmlWriter.writeEndElement();
    }
    // Close tag "graphs"
    xmlWriter.writeEndElement();
    // Close tag "savedDataSets"
    xmlWriter.writeEndElement();

    xmlWriter.writeEndDocument();
    file.close();
}

std::pair<std::pair<QDateTime, QDateTime>, std::vector<DataSet> > CacheFileHandler::readDataSet(QString fileName)
{
    std::vector<DataSet> data;
    QDateTime startDate;
    QDateTime endDate;

    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
    {
        std::cout<<"Loading data set failed"<<std::endl;
    }
    else
    {
        QDomDocument xmlDoc;
        xmlDoc.setContent(&file);
        QDomElement root = xmlDoc.documentElement();
        QDomElement child = root.firstChild().toElement();

        // First child is the start date
        QDomElement startTimeElement = child.firstChild().toElement();
        qint64 startDateEpoch = startTimeElement.firstChild().toText().data().toLongLong();
        startDate = QDateTime::fromSecsSinceEpoch(startDateEpoch);

        // Second child is the end date
        child = child.nextSibling().toElement();
        QDomElement endTimeElement = child.firstChild().toElement();
        qint64 endDateEpoch = endTimeElement.firstChild().toText().data().toLongLong();
        endDate = QDateTime::fromSecsSinceEpoch(endDateEpoch);

        // Third child contains the graphs
        child = child.nextSibling().toElement();
        // Children of graphs contain "graph" nodes
        child = child.firstChild().toElement();
        while(!child.isNull())
        {
            if(child.tagName() == "graph")
            {
                DataSet dataSet = {};
                dataSet.series.splineSeries = new QSplineSeries();
                QDomElement graphElement = child.firstChild().toElement();
                // Go through graph info
                while(!graphElement.isNull())
                {
                    // DataSourceDetails info
                    if(graphElement.tagName() == "apiDataType"){
                        dataSet.dataSource.dataType = static_cast<DataImporting::ApiDataType>
                                                      (graphElement.firstChild().toText().data().toInt());
                    }
                    else if(graphElement.tagName() == "importerIndex"){
                        dataSet.dataSource.dataSourceIndex = graphElement.firstChild().toText().data().toInt();
                    }
                    else if(graphElement.tagName() == "dataSourceName"){
                        dataSet.dataSource.dataSourceName = graphElement.firstChild().toText().data().toStdString();
                    }
                    else if(graphElement.tagName() == "dataLocationName"){
                        dataSet.dataSource.dataLocationName = graphElement.firstChild().toText().data().toStdString();
                    }
                    else if(graphElement.tagName() == "graphName"){
                        dataSet.dataSource.graphName = graphElement.firstChild().toText().data().toStdString();
                    }
                    else if(graphElement.tagName() == "hide"){
                        dataSet.hide = graphElement.firstChild().toText().data().toInt();
                    }
                    // DataSeries info
                    else if(graphElement.tagName() == "unitOfMeasurement"){
                        dataSet.series.unitOfMeasurement = graphElement.firstChild().toText().data().toStdString();
                    }
                    else if(graphElement.tagName() == "maxY"){
                        dataSet.series.maxY = graphElement.firstChild().toText().data().toFloat();
                    }
                    else if(graphElement.tagName() == "minY"){
                        dataSet.series.minY = graphElement.firstChild().toText().data().toFloat();
                    }
                    else if(graphElement.tagName() == "magnitude"){
                        dataSet.series.magnitude = graphElement.firstChild().toText().data().toFloat();
                    }
                    else if(graphElement.tagName() == "data")
                    {
                        QDomElement dataElement = graphElement.firstChild().toElement();
                        // Go through data
                        while(!dataElement.isNull())
                        {
                            float x;
                            float y;
                            QDomElement pointElement = dataElement.firstChild().toElement();
                            // Go through point values
                            while(!pointElement.isNull())
                            {
                                if(pointElement.tagName() == "value"){
                                    y = pointElement.firstChild().toText().data().toFloat();
                                }
                                else if(pointElement.tagName() == "secsSinceEpoch"){
                                    x = pointElement.firstChild().toText().data().toFloat();
                                }
                                pointElement = pointElement.nextSibling().toElement();
                            }
                            dataSet.series.splineSeries->append(QPointF(x, y));
                            dataElement = dataElement.nextSibling().toElement();
                        }
                    }
                    graphElement = graphElement.nextSibling().toElement();
                }
                data.push_back(dataSet);
            }
            child = child.nextSibling().toElement();
        }
    }
    file.close();
    return {{startDate, endDate}, data};
}

void CacheFileHandler::savePreference(std::vector<DataSourceDetails> savedPreference, QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("SavedPreference");
    xmlWriter.writeStartElement("Types");

    // Go through each data type
    for(DataSourceDetails dataSource: savedPreference)
    {
        xmlWriter.writeStartElement("DataDetail");
        // DataSourceDetails data
        xmlWriter.writeTextElement("dataSourceName", QString::fromStdString(dataSource.dataSourceName));
        xmlWriter.writeTextElement("dataLocationName", QString::fromStdString(dataSource.dataLocationName));
        xmlWriter.writeTextElement("graphName", QString::fromStdString(dataSource.graphName));
        xmlWriter.writeTextElement("importerIndex", QString::number(dataSource.dataSourceIndex));
        xmlWriter.writeTextElement("apiDataType", QString::number(dataSource.dataType));

        // Close tag "DataDetail"
        xmlWriter.writeEndElement();
    }
    // Close "Types"
    xmlWriter.writeEndElement();
    // Close "SavedPreference"
    xmlWriter.writeEndElement();
    file.close();
}

std::vector<DataSourceDetails> CacheFileHandler::loadPreference(QString fileName)
{
    std::vector<DataSourceDetails> preference = {};
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
    {
        std::cout<<"Loading preference failed"<<std::endl;
    }
    else
    {
        QDomDocument xmlDoc;
        xmlDoc.setContent(&file);
        QDomElement root = xmlDoc.documentElement();
        // First child contains the data types
        QDomElement child = root.firstChild().toElement();

        // Children of types contain data details
        child = child.firstChild().toElement();
        while(!child.isNull())
        {
            if(child.tagName() == "DataDetail")
            {
                DataSourceDetails dataSource;
                QDomElement dataDetailElement = child.firstChild().toElement();
                // Go through saved details
                while(!dataDetailElement.isNull())
                {
                    // DataSourceDetails info
                    if(dataDetailElement.tagName() == "apiDataType"){
                        dataSource.dataType = static_cast<DataImporting::ApiDataType>
                                                      (dataDetailElement.firstChild().toText().data().toInt());
                    }
                    else if(dataDetailElement.tagName() == "importerIndex"){
                        dataSource.dataSourceIndex = dataDetailElement.firstChild().toText().data().toInt();
                    }
                    else if(dataDetailElement.tagName() == "dataSourceName"){
                        dataSource.dataSourceName = dataDetailElement.firstChild().toText().data().toStdString();
                    }
                    else if(dataDetailElement.tagName() == "dataLocationName"){
                        dataSource.dataLocationName = dataDetailElement.firstChild().toText().data().toStdString();
                    }
                    else if(dataDetailElement.tagName() == "graphName"){
                        dataSource.graphName = dataDetailElement.firstChild().toText().data().toStdString();
                    }
                    dataDetailElement = dataDetailElement.nextSibling().toElement();
                }
                preference.push_back(dataSource);
            }
            child = child.nextSibling().toElement();
        }
    }
    file.close();
    return preference;
}
