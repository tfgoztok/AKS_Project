#ifndef AKSCORE_H
#define AKSCORE_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QString>
#include <QDateTime>
#include "CommunicationManager.h"

// Structure to hold missile configuration details
struct MissileConfig
{
    int index;    // Index of the missile
    QString type; // Type of the missile
    bool healthy; // Health status of the missile
};

class AKSCore : public QObject
{
    Q_OBJECT

public:
    explicit AKSCore(QObject *parent = nullptr); // Constructor
    ~AKSCore();                                  // Destructor

    void start(); // Start the core operations
    void stop();  // Stop the core operations

    enum MissileState
    {
        Unhealthy = 0,
        Healthy = 1,
        Powered = 2,
        Fired = 3
    };                                                                                     // Enum for missile states
    void updateLauncherCommunicationStatus(bool connected);                                // Update communication status
    CommunicationManager *getCommunicationManager() const { return communicationManager; } // Get communication manager

signals:
    void platformInfoUpdated(double latitude, double longitude, double altitude);                      // Signal for platform info update
    void communicationStatusChanged(bool ansStatus, bool launcherStatus);                              // Signal for communication status change
    void missileStatusChanged(int index, const QString &type, bool healthy, bool powered, bool fired); // Signal for missile status change
    void errorOccurred(const QString &message);                                                        // Signal for error occurrence

public slots:
    void updateTargetInfo(double latitude, double longitude, double altitude, double speed, double distance); // Update target information
    void toggleMissilePower(int missileIndex);                                                                // Toggle power for a specific missile
    void launchMissile();                                                                                     // Launch the missile
    void updateMissileConfig(int index, const QString &type, bool healthy);                                   // Update missile configuration
    void onANSHeartbeatReceived();                                                                            // Handle ANS heartbeat reception
    void onLauncherHeartbeatReceived();                                                                       // Handle launcher heartbeat reception
    void onPlatformInfoReceived(double latitude, double longitude, double altitude);                          // Handle platform info reception
    void onANSCommunicationLost();                                                                            // Handle loss of ANS communication
    void onLauncherCommunicationLost();                                                                       // Handle loss of launcher communication

private slots:
    void checkCommunicationStatus(); // Check the communication status

private:
    QVector<MissileConfig> missileConfigs; // Vector to hold missile configurations
    QVector<MissileState> missileStates;   // Vector to hold missile states
    void updateMissileStatus(int index);   // Update the status of a specific missile

    QTimer *launchSequenceTimer; // Timer for launch sequence

    int launchingMissileIndex;   // Index of the missile currently being launched
    int currentlyPoweredMissile; // Index of the currently powered missile

    bool ansConnected;      // Status of ANS connection
    bool launcherConnected; // Status of launcher connection

    QTimer *communicationCheckTimer;            // Timer for checking communication status
    CommunicationManager *communicationManager; // Pointer to the communication manager

    void startLaunchSequence(int missileIndex); // Start the launch sequence for a missile
    void continueLaunchSequence();              // Continue the launch sequence
    void emitError(const QString &message);     // Emit an error signal
    void finalizeLaunch();                      // Finalize the launch process
};

#endif