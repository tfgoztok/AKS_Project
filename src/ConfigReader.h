#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <QObject>
#include <QVector>
#include <QString>
#include "AKSCore.h"

class ConfigReader : public QObject
{
    Q_OBJECT

public:
    // Constructor: Initializes the ConfigReader with an optional parent QObject
    explicit ConfigReader(QObject *parent = nullptr);

    // Loads missile configuration from a specified file
    bool loadMissileConfig(const QString &filename);

    // Returns a QVector containing all loaded MissileConfig objects
    QVector<MissileConfig> getMissileConfigs() const;

    // Returns the last error message encountered during operations
    QString getLastError() const;

private:
    // Stores the missile configurations loaded from the file
    QVector<MissileConfig> missileConfigs;

    // Holds the last error message for debugging purposes
    QString lastError;
};

#endif