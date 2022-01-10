/**
  * @file xmlfetcher.hh declares the XmlFetcher class that fetches XML
  * data from URLs.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 9.2.2021
  */

#ifndef XMLFETCHER_HH
#define XMLFETCHER_HH

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtXml>

namespace DataImporting
{

/**
 * @brief The XmlFetcher class fetches XML data from URLs and passes it on to
 * be parsed for data.
 */
class XmlFetcher : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief The default constructor.
     * @param parent: The QObject to parent this XmlFetcher to
     */
    explicit XmlFetcher(QObject* parent = nullptr);

    /**
      * @brief The default destructor.
      */
    virtual ~XmlFetcher();

    /**
     * @brief fetchXml fetches XML data from the given URL.
     * @param url: The URL to fetch the XML data from
     * @param customHeaderName: The name of the custom HTML header to include
     * @param customHeaderValue: The value of the custom HTML header to include
     */
    void fetchXml(const std::string& url, const std::string& customHeaderName,
                  const std::string& customHeaderValue);

    /**
     * @brief fetchXml fetches XML data from the given URL.
     * @param url: The URL to fetch the XML data from
     */
    void fetchXml(const std::string& url);

signals:
    /**
     * @brief The xmlFetched signal is sent whenever XML data has been fetched
     * successfully.
     * @param xmlData: The fetched data
     * @param url: The URL the data was fetched from
     */
    void xmlFetched(const QDomDocument& xmlData, const std::string url);

protected slots:
    /**
     * @brief replyReceived is called whenever networkManager_ finishes
     * receiving a network reply.
     * @param reply: A pointer to the QNetworkReply received
     */
    void replyReceived(QNetworkReply* reply);

protected:
    /**
     * @brief networkManager_ stores the QNetworkAccessManager that makes
     * fetch requests.
     */
    QNetworkAccessManager networkManager_;
};

}

#endif // XMLFETCHER_HH
