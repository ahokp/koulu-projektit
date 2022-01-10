/**
  * @file xmldataimporter.hh declares the XmlDataImporter class, which is used
  * as a base class to import XML format data.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 2.3.2021
  */

#ifndef XMLDATAIMPORTER_H
#define XMLDATAIMPORTER_H

#include "dataimporter.hh"
#include "xmlfetcher.hh"

#include <QtXml>

namespace DataImporting
{

/**
 * @brief The XmlDataImporter class is an abstract base class used by data
 * importers that import their data in XML format.
 */
class XmlDataImporter : public DataImporter
{
    Q_OBJECT

protected slots:
    /**
     * @brief parseXml is called when XML data has been received from
     * xmlFetcher_. Abstract method: implement in inheriting class.
     * @param xmlData: The fetched XML data
     * @param url: The URL the data was fetched from
     */
    virtual void parseXml(const QDomDocument& xmlData,
                          const std::string& url) = 0;

protected:
    // Stores how many components an API date/time string consists of
    static const int DATE_TIME_COMPONENT_COUNT = 6;

    // Stores how long API date/time strings are in characters
    static const int DATE_TIME_STRING_LENGTH = 20;

    /**
     * @brief xmlFetcher_ stores the XmlFetcher used to fetch XML data from the
     * API.
     */
    XmlFetcher xmlFetcher_;

    /**
     * @brief The default constructor.
     * @param parent: The QObject to parent this XmlDataImporter to
     */
    explicit XmlDataImporter(QObject* parent = nullptr);

    /**
     * @brief padString adds padding characters to the start of the given
     * string until its length is minLength.
     * @param string: The string to pad
     * @param minLength: The length that should be reached
     * @param paddingChar: The character to pad with
     */
    static void padString(std::string& string, unsigned int minLength,
                          char paddingChar);

    /**
     * @brief dateTimeFromApiString parses a date string given by the API and
     * turns it into a QDateTime.
     * @param dateTimeString: The API date/time string to parse
     * @return QDateTime matching the given date/time string
     * @exception std::invalid_argument: Thrown when string could not be parsed
     * into a QDateTime
     */
    static QDateTime dateTimeFromApiString(const std::string& dateTimeString);

    /**
     * @brief dateTimeToApiString creates an API compatible date string from a
     * QDateTime.
     * @param dateTime: The QDateTime to convert into a string
     * @return string matching the given QDateTime
     */
    static std::string dateTimeToApiString(const QDateTime& dateTime);

    /**
     * @brief findSubstring finds the given substring from inspectedString and
     * returns the index where the substring starts in inspectedString.
     * @param inspectedString: The string to try and find the substring in
     * @param substring: The substring to find in inspectedString
     * @return The index of the first character of substring in inspectedString
     * (inspectedString.length() if substring not found)
     */
    static int findSubstring(const std::string& inspectedString,
                             const std::string& substring);
};

}

#endif // XMLDATAIMPORTER_H
