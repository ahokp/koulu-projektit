/**
  * @file xmldataimporter.cpp implements the XmlDataImporter class.
  * @author Arttu Leppäaho 282591 arttu.leppaaho@tuni.fi
  * @date 2.3.2021
  */

#include "xmldataimporter.hh"

namespace DataImporting
{

XmlDataImporter::XmlDataImporter(QObject* parent) : DataImporter(parent),
    xmlFetcher_()
{
    // Parse XML documents automatically when they're fetched
    connect(&xmlFetcher_, &XmlFetcher::xmlFetched,
            this, &XmlDataImporter::parseXml);
}

QDateTime XmlDataImporter::dateTimeFromApiString(
    const std::string& dateTimeString)
{
    unsigned int charIndex = 0;
    unsigned int componentIndex = 0;

    char delimiter = '-';
    char sectionEndChar = 'T';

    // Dates and times are given by the API in a string containing 6
    // components: year, month, day, hours, minutes, seconds
    // The string's format is {year}-{month}-{day}T{hours}:{minutes}:{seconds}Z
    std::string dateTimeComponents[DATE_TIME_COMPONENT_COUNT];

    // Separate all the components of the date and time
    while (charIndex < dateTimeString.length() &&
           componentIndex < DATE_TIME_COMPONENT_COUNT)
    {
        char currentChar = dateTimeString[charIndex];

        if (currentChar == delimiter)
        {
            // Delimiter reached: this component has been parsed, parse the
            // next one
            componentIndex++;
        }
        else if (currentChar == sectionEndChar)
        {
            // Section end character reached: stop parsing date, start parsing
            // time
            sectionEndChar = 'Z';
            delimiter = ':';

            componentIndex++;
        }
        else
        {
            // Append parsed characters to the current component
            dateTimeComponents[componentIndex] += dateTimeString[charIndex];
        }

        charIndex++;
    }

    int dateTimeComponentsInt[dateTimeComponents->length()];

    // Convert component strings into integers (exception handling left to
    // caller)
    for (int i = 0; i < DATE_TIME_COMPONENT_COUNT; i++)
    {
        dateTimeComponentsInt[i] = std::stoi(dateTimeComponents[i]);
    }

    return QDateTime(
        QDate(dateTimeComponentsInt[0],
              dateTimeComponentsInt[1],
              dateTimeComponentsInt[2]),
        QTime(dateTimeComponentsInt[3],
              dateTimeComponentsInt[4],
              dateTimeComponentsInt[5]));
}

std::string XmlDataImporter::dateTimeToApiString(const QDateTime& dateTime)
{
    const QDate& date = dateTime.date();
    const QTime& time = dateTime.time();

    std::string dateTimeComponents[DATE_TIME_COMPONENT_COUNT] =
    {
        std::to_string(date.year()),
        std::to_string(date.month()),
        std::to_string(date.day()),
        std::to_string(time.hour()),
        std::to_string(time.minute()),
        std::to_string(time.second())
    };

    // Padding is needed for the right format: all numbers except year are
    // at least two characters
    for (int i = 1; i < DATE_TIME_COMPONENT_COUNT; i++)
    {
        padString(dateTimeComponents[i], 2, '0');
    }

    return dateTimeComponents[0] + "-" +
           dateTimeComponents[1] + "-" +
           dateTimeComponents[2] + "T" +
           dateTimeComponents[3] + ":" +
           dateTimeComponents[4] + ":" +
           dateTimeComponents[5] + "Z";
}

void XmlDataImporter::padString(std::string& string, unsigned int minLength,
                                char paddingChar)
{
    while (string.length() < minLength)
    {
        string = paddingChar + string;
    }
}

int XmlDataImporter::findSubstring(const std::string& inspectedString,
                                   const std::string& substring)
{
    unsigned int stringCursor = 0;
    bool foundSubstring = false;

    while (!foundSubstring && stringCursor < inspectedString.length())
    {
        foundSubstring = true;

        // Check for substring
        for (unsigned int i = 0; i < substring.length(); i++)
        {
            // If a substring character doesn't match, substring can't be found
            // here
            if (inspectedString.at(stringCursor + i) != substring.at(i))
            {
                foundSubstring = false;
                break;
            }
        }

        // If substring was found, stop moving cursor so that it keeps pointing
        // at the start of the substring
        if (!foundSubstring)
        {
            stringCursor++;
        }
    }

    return stringCursor;
}

}
