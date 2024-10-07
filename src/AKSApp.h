#ifndef AKSAPP_H
#define AKSAPP_H

#include <QMainWindow>
#include "AKSGUI.h"
#include "AKSCore.h"
#include "ConfigReader.h"

// Main application class for the AKS system, inheriting from QMainWindow
class AKSApp : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor: Initializes the application with an optional parent widget
    AKSApp(QWidget *parent = nullptr);

    // Destructor: Cleans up resources
    ~AKSApp();

private slots:
    // Slot for handling updates to target information
    void onTargetInfoUpdated(double latitude, double longitude, double altitude, double speed, double distance);

    // Slot for toggling power of a missile
    void onPowerToggled(int missileIndex);

    // Slot for handling missile launch events
    void onMissileLaunched();

    // Slot for updating platform information
    void onPlatformInfoUpdated(double latitude, double longitude, double altitude);

    // Slot for handling changes in communication status
    void onCommunicationStatusChanged(bool ansStatus, bool launcherStatus);

    // Slot for handling changes in missile status
    void onMissileStatusChanged(int index, const QString &type, bool healthy, bool powered, bool fired);

public slots:
    // Slot for handling error messages
    void handleError(const QString &message);

private:
    // Pointer to the GUI component
    AKSGUI *gui;

    // Pointer to the core logic component
    AKSCore *core;

    // Pointer to the configuration reader
    ConfigReader *configReader;

    // Method to set up signal-slot connections
    void setupConnections();

    // Method to load missile configuration settings
    void loadMissileConfig();
};

#endif