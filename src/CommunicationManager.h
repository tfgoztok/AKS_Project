#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QDateTime>

// Class responsible for managing communication with ANS and Launcher
class CommunicationManager : public QObject
{
    Q_OBJECT

public:
    // Constructor and Destructor
    explicit CommunicationManager(QObject *parent = nullptr); // Initializes the communication manager
    ~CommunicationManager();                                  // Cleans up resources

    // Check if there has been a recent heartbeat from ANS or Launcher
    bool hasRecentANSHeartbeat() const;
    bool hasRecentLauncherHeartbeat() const;

    // Update the last heartbeat timestamps
    void updateANSLastHeartbeat();
    void updateLauncherLastHeartbeat();

    // Start and stop communication
    void start();
    void stop();

signals:
    // Signals emitted on receiving heartbeats or communication loss
    void ansHeartbeatReceived();
    void launcherHeartbeatReceived();
    void ansCommunicationLost();
    void launcherCommunicationLost();
    void platformInfoReceived(double latitude, double longitude, double altitude);

private slots:
    // Private slots for handling heartbeat sending and reading datagrams
    void sendHeartbeat();
    void readPendingDatagrams();
    void checkCommunicationStatus();

private:
    QUdpSocket *socket;              // Socket for UDP communication
    QTimer *heartbeatTimer;          // Timer for sending heartbeats
    QTimer *communicationCheckTimer; // Timer for checking communication status

    const quint16 AKS_PORT = 5000;      // Port for AKS communication
    const quint16 ANS_PORT = 5001;      // Port for ANS communication
    const quint16 LAUNCHER_PORT = 5002; // Port for Launcher communication

    int ansMissedHeartbeats;      // Counter for missed ANS heartbeats
    int launcherMissedHeartbeats; // Counter for missed Launcher heartbeats

    // Process received heartbeats and platform info
    void processANSHeartbeat();
    void processLauncherHeartbeat();
    void processPlatformInfo(const QByteArray &data);

    QDateTime lastANSHeartbeat;      // Timestamp of the last ANS heartbeat
    QDateTime lastLauncherHeartbeat; // Timestamp of the last Launcher heartbeat
};

#endif
