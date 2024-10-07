#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

// Logger class for handling log messages
class Logger : public QObject
{
    Q_OBJECT

public:
    // Enum for different log levels
    enum LogLevel
    {
        Debug,   // Debugging information
        Info,    // Informational messages
        Warning, // Warning messages
        Error,   // Error messages
        Critical // Critical error messages
    };

    // Get the singleton instance of Logger
    static Logger &instance();

    // Log a message with a specific log level
    void log(LogLevel level, const QString &message);

private:
    // Private constructor for singleton pattern
    explicit Logger(QObject *parent = nullptr);
    ~Logger();

    QFile logFile;         // File to write logs to
    QTextStream logStream; // Stream to write to the log file

    // Convert log level to string representation
    QString levelToString(LogLevel level);
};

// Macros for logging at different levels
#define LOG_DEBUG(msg) Logger::instance().log(Logger::Debug, msg)
#define LOG_INFO(msg) Logger::instance().log(Logger::Info, msg)
#define LOG_WARNING(msg) Logger::instance().log(Logger::Warning, msg)
#define LOG_ERROR(msg) Logger::instance().log(Logger::Error, msg)
#define LOG_CRITICAL(msg) Logger::instance().log(Logger::Critical, msg)

#endif
