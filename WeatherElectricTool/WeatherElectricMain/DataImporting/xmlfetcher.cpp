/**
  * @file xmlfetcher.cpp implements the XmlFetcher class.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 9.2.2021
  */

#include "xmlfetcher.hh"

namespace DataImporting
{

XmlFetcher::XmlFetcher(QObject* parent) : QObject(parent), networkManager_()
{
    // Call replyReceived whenever a network reply is finished
    connect(&networkManager_, &QNetworkAccessManager::finished,
            this, &XmlFetcher::replyReceived);
}

XmlFetcher::~XmlFetcher()
{

}

void XmlFetcher::fetchXml(const std::string& url,
                          const std::string& customHeaderName,
                          const std::string& customHeaderValue)
{
    QNetworkRequest request(QUrl(QString::fromStdString(url)));

    if (customHeaderName != "")
    {
        request.setRawHeader(QByteArray::fromStdString(customHeaderName),
                             QByteArray::fromStdString(customHeaderValue));
    }

    networkManager_.get(request);
}

void XmlFetcher::fetchXml(const std::string& url)
{
    fetchXml(url, "", "");
}

void XmlFetcher::replyReceived(QNetworkReply* reply)
{
    QDomDocument xmlDoc;

    // Create XML document from reply and pass it forward with xmlFetched
    xmlDoc.setContent(reply->readAll());

    emit xmlFetched(xmlDoc, reply->url().toString().toStdString());

    // The reply needs to be deleted using this special method
    reply->deleteLater();
}

}
