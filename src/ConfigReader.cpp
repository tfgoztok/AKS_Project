#include "ConfigReader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

// Constructor for ConfigReader
ConfigReader::ConfigReader(QObject *parent)
    : QObject(parent)
{
}

// Load missile configuration from a specified file
bool ConfigReader::loadMissileConfig(const QString &filename)
{
    QFile file(filename); // Create a QFile object for the given filename
    if (!file.exists())   // Check if the file exists
    {
        lastError = QString("Config file does not exist: %1").arg(QDir::toNativeSeparators(QFileInfo(file).absoluteFilePath()));
        qDebug() << lastError; // Log the error
        return false;          // Return false if the file does not exist
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) // Attempt to open the file
    {
        lastError = QString("Failed to open config file: %1. Error: %2").arg(filename).arg(file.errorString());
        qDebug() << lastError; // Log the error
        return false;          // Return false if the file cannot be opened
    }

    missileConfigs.clear(); // Clear any existing missile configurations
    QTextStream in(&file);  // Create a QTextStream to read the file
    int lineNumber = 0;     // Initialize line number for error reporting
    while (!in.atEnd())     // Read the file line by line
    {
        QString line = in.readLine().trimmed();     // Read and trim the line
        lineNumber++;                               // Increment line number
        if (line.isEmpty() || line.startsWith("#")) // Skip empty lines and comments
        {
            continue; // Go to the next line
        }

        QStringList parts = line.split(","); // Split the line by commas
        if (parts.size() != 3)               // Check if the line has the correct number of parts
        {
            lastError = QString("Invalid format in config file at line %1: %2").arg(lineNumber).arg(line);
            qDebug() << lastError; // Log the error
            file.close();          // Close the file
            return false;          // Return false for invalid format
        }

        bool ok;
        int index = parts[0].toInt(&ok);   // Convert the first part to an integer
        if (!ok || index < 0 || index > 3) // Validate the index
        {
            lastError = QString("Invalid missile index at line %1: %2").arg(lineNumber).arg(parts[0]);
            qDebug() << lastError; // Log the error
            file.close();          // Close the file
            return false;          // Return false for invalid index
        }

        QString type = parts[1].trimmed();                       // Get the missile type
        bool healthy = (parts[2].trimmed().toLower() == "true"); // Determine if the missile is healthy

        missileConfigs.append({index, type, healthy}); // Append the configuration to the list
    }

    file.close();                                                                           // Close the file after reading
    qDebug() << "Successfully loaded" << missileConfigs.size() << "missile configurations"; // Log success message
    return true;                                                                            // Return true if loading was successful
}

// Get the list of missile configurations
QVector<MissileConfig> ConfigReader::getMissileConfigs() const
{
    return missileConfigs; // Return the missile configurations
}

// Get the last error message
QString ConfigReader::getLastError() const
{
    return lastError; // Return the last error message
}