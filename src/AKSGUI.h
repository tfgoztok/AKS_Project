#ifndef AKSGUI_H
#define AKSGUI_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLCDNumber>
#include "clickablemissilewidget.h"
#include <QGroupBox>
#include <QHBoxLayout>

namespace Ui
{
    class MainWindow; // Forward declaration of the MainWindow UI class
}

class AKSGUI : public QWidget // Main GUI class for the application
{
    Q_OBJECT

public:
    explicit AKSGUI(QWidget *parent = nullptr); // Constructor
    ~AKSGUI();                                  // Destructor

    // Methods to update platform and missile information
    void updatePlatformInfo(double latitude, double longitude, double altitude);
    void updateCommunicationStatus(bool ansStatus, bool launcherStatus);
    void updateMissileStatus(int index, const QString &name, bool healthy, bool powered, bool fired);
    void setupMissileWidgets(); // Setup missile widgets in the UI

signals:
    // Signals for communication with other components
    void targetInfoUpdated(double latitude, double longitude, double altitude, double speed, double distance);
    void powerToggled(int missileIndex);
    void missileLaunched();
    void errorOccurred(const QString &message);

private slots:
    // Slot functions for button click events
    void onUpdateTargetClicked();
    void onPowerToggleClicked();
    void onLaunchButtonClicked();

private:
    Ui::MainWindow *ui;               // Pointer to the UI class
    QLabel *platformInfoLabels[3];    // Labels for platform information
    QLineEdit *platformInfoValues[3]; // Input fields for platform information
    QLabel *targetInfoLabels[5];      // Labels for target information
    QLineEdit *targetInfoValues[5];   // Input fields for target information
    QPushButton *updateTargetButton;  // Button to update target information
    QWidget *communicationStatus[2];  // Widgets to show communication status
    QLabel *missileNames[4];          // Labels for missile names
    QPushButton *launchMissileButton; // Button to launch a missile

    bool launcherConnected;         // Status of the launcher connection
    void updateLaunchButtonState(); // Update the state of the launch button

    void setupUi();        // Setup the UI components
    void connectSignals(); // Connect signals and slots

    QGroupBox *missileGroup;                          // Group box for missile widgets
    QHBoxLayout *missileLayout;                       // Layout for missile widgets
    QVector<ClickableMissileWidget *> missileWidgets; // Vector of clickable missile widgets

    void onMissileWidgetClicked(int index); // Handle missile widget click events
};

#endif
