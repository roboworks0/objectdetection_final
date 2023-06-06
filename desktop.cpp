#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QVBoxLayout>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>

#define PORT 8000  // Port number to listen on

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        // Create QLabel for video display
        videoLabel = new QLabel(this);
        videoLabel->setScaledContents(true);

        // Create main widget and set the layout
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        layout->addWidget(videoLabel);
        setCentralWidget(centralWidget);

        // Create server socket
        server = new QTcpServer(this);
        connect(server, &QTcpServer::newConnection, this, &MainWindow::acceptConnection);

        if (!server->listen(QHostAddress::Any, PORT))
        {
            qDebug() << "Server could not start!";
            return;
        }

        qDebug() << "Server started!";

        // Accept incoming connections
        frameSize = 0;
        totalBytesRead = 0;

        startNextFrame();
    }

private slots:
    void acceptConnection()
    {
        clientSocket = server->nextPendingConnection();
        connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::receiveFrameSize);
        qDebug() << "New client connected!";
    }

    void receiveFrameSize()
    {
        QDataStream in(clientSocket);
        if (frameSize == 0 && clientSocket->bytesAvailable() >= sizeof(frameSize))
        {
            in >> frameSize;
            frameData.resize(frameSize);
            totalBytesRead = 0;
        }

        receiveFrameData();
    }

    void receiveFrameData()
    {
        qint64 bytesRead = clientSocket->read(frameData.data() + totalBytesRead, frameSize - totalBytesRead);
        totalBytesRead += bytesRead;

        if (totalBytesRead == frameSize)
        {
            displayFrame();
            startNextFrame();
        }
    }

    void displayFrame()
    {
        cv::Mat frame = cv::imdecode(frameData, cv::IMREAD_COLOR);
        if (!frame.empty())
        {
            QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
            videoLabel->setPixmap(QPixmap::fromImage(image));
        }
    }

    void startNextFrame()
    {
        frameSize = 0;
        totalBytesRead = 0;
    }

private:
    QLabel *videoLabel;
    QTcpServer *server;
    QTcpSocket *clientSocket;
    QByteArray frameData;
    qint32 frameSize;
    qint64 totalBytesRead;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

#include "main.moc"
