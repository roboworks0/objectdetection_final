#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>
#include <opencv2/opencv.hpp>

// Function to receive and update the frame
void updateFrame(cv::Mat frame, QLabel* frameLabel)
{
    // Convert the frame to QImage
    QImage image(frame.data, frame.cols, frame.rows, QImage::Format_RGB888);
    image = image.rgbSwapped();

    // Set the QImage as the pixmap of the frame label widget
    frameLabel->setPixmap(QPixmap::fromImage(image));
    frameLabel->setScaledContents(true);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create the main window
    QMainWindow window;

    // Create a central widget
    QWidget* centralWidget = new QWidget(&window);
    window.setCentralWidget(centralWidget);

    // Create a layout for the central widget
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    // Create a label widget to display the frame
    QLabel* frameLabel = new QLabel(centralWidget);
    layout->addWidget(frameLabel);

    // Create random buttons
    for (int i = 0; i < 5; i++) {
        QPushButton* button = new QPushButton(QString("Button %1").arg(i + 1), centralWidget);
        layout->addWidget(button);
    }

    // Set the layout to the central widget
    centralWidget->setLayout(layout);

    // Initialize the server communication and receive the frame
    // Replace this part with your server communication code from server.cpp

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        qDebug() << "Failed to open the camera.";
        return -1;
    }

    cv::Mat frame;
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        cap >> frame;
        if (frame.empty())
            return;

        // Update the frame in the GUI
        updateFrame(frame, frameLabel);
    });
    timer.start(33); // Update every 33 milliseconds (approximately 30 frames per second)

    // Show the main window
    window.show();

    return a.exec();
}
