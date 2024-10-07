#include "Logger.h"

// Constructor for Logger class
Logger::Logger(QObject *parent) : QObject(parent)
{
    logFile.setFileName("aks_log.txt"); // Set the log file name
    // Attempt to open the log file for writing
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        logStream.setDevice(&logFile); // Set the log stream to the log file
    }
}

// Destructor for Logger class
Logger::~Logger()
{
    // Close the log file if it is open
    if (logFile.isOpen())
    {
        logFile.close();
    }
}

// Singleton instance of Logger
Logger &Logger::instance()
{
    static Logger instance; // Create a static instance of Logger
    return instance;        // Return the instance
}

// Log a message with a specific log level
void Logger::log(LogLevel level, const QString &message)
{
    // Create a log entry with the current date, time, log level, and message
    QString logEntry = QString("%1 [%2] %3\n")
                           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")) // Current timestamp
                           .arg(levelToString(level))                                             // Convert log level to string
                           .arg(message);                                                         // The log message

    logStream << logEntry; // Write the log entry to the log stream
    logStream.flush();     // Ensure the log entry is written to the file

    // Also print to console for debugging purposes
    qDebug().noquote() << logEntry; // Output the log entry to the console
}

// Convert log level enum to string representation
QString Logger::levelToString(LogLevel level)
{
    switch (level) // Check the log level
    {
    case Debug:
        return "DEBUG"; // Return string for Debug level
    case Info:
        return "INFO"; // Return string for Info level
    case Warning:
        return "WARNING"; // Return string for Warning level
    case Error:
        return "ERROR"; // Return string for Error level
    case Critical:
        return "CRITICAL"; // Return string for Critical level
    default:
        return "UNKNOWN"; // Return string for unknown log level
    }
}