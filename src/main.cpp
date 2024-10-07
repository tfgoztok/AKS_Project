#include <QApplication>
#include "AKSApp.h"

int main(int argc, char *argv[])
{
    // Initialize the QApplication object with command line arguments
    QApplication app(argc, argv);

    // Create an instance of the main application window
    AKSApp mainWindow;

    // Show the main application window
    mainWindow.show();

    // Enter the main event loop and wait for events
    return app.exec();
}