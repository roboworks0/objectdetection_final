#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define PORT 8000  // Port number to listen on

// Function to apply non-maximum suppression
void applyNMS(std::vector<cv::Rect>& boxes, std::vector<float>& confidences, float nmsThreshold)
{
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, 0.0, nmsThreshold, indices);

    std::vector<cv::Rect> selectedBoxes;
    std::vector<float> selectedConfidences;

    for (int idx : indices)
    {
        selectedBoxes.push_back(boxes[idx]);
        selectedConfidences.push_back(confidences[idx]);
    }

    boxes = selectedBoxes;
    confidences = selectedConfidences;
}


int main() {
    // Load the pre-trained YOLO model
    cv::dnn::Net net = cv::dnn::readNetFromDarknet("yolov3.cfg", "yolov3.weights");
    // Specify the target backend and compute backend for the network
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    
    // Load class labels
    std::vector<std::string> classLabels;
    std::string labelFile = "coco.names";
    std::ifstream ifs(labelFile.c_str());
    if (!ifs.is_open()) {
        std::cerr << "Failed to open label file: " << labelFile << std::endl;
        return -1;
    }
    std::string line;
    while (std::getline(ifs, line)) {
        classLabels.push_back(line);
    }

    int server_socket, new_socket, valread;
    struct sockaddr_in address{};
    int addrlen = sizeof(address);

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(server_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connection
    if ((new_socket = accept(server_socket, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    cv::Mat frame;
    std::vector<uchar> buffer;
    int message_size;

    std::cout << "test..." << "\n";
    int counter = 0;

    while (true) {
        // Receive the sensor data in tuple
        char sensor_buffer[12];
        if(read(new_socket, sensor_buffer, sizeof(sensor_buffer)) == 0 ) {
            perror("client disconnected");
            break;
        }
        // Unpack the received bytes into a tuple
        int value1 = *reinterpret_cast<int*>(sensor_buffer);
        int value2 = *reinterpret_cast<int*>(sensor_buffer + 4);
        int value3 = *reinterpret_cast<int*>(sensor_buffer + 8);

        std::cout << "value1 : " << value1 << ", value2 : " << value2 << ", value3 : " << value3 << std::endl;

        // Receive the frame size
        std::cout << "debug1\n";
        if (read(new_socket, &message_size, sizeof(message_size)) == 0) {
            perror("Client disconnected");
            break;
        }
        std::cout << "debug2\n";

        std::cout << "message size : " << message_size << "\n";

        // Resize the buffer to fit the received frame
        buffer.resize(message_size);

        auto start = high_resolution_clock::now();
        ssize_t bytes_read;
        ssize_t total_bytes_read = 0;
        while (total_bytes_read < message_size)
        {
            std::cout << "total_bytes_read : " << total_bytes_read << "\n";
            ssize_t bytes_read = read(new_socket, buffer.data() + total_bytes_read, message_size - total_bytes_read);
            std::cout << "bytes_read : " << bytes_read << "\n";            
            if (bytes_read == 0)
            {
                // Connection closed by the sender
                break;
            }
            else if (bytes_read < 0)
            {
                // Error reading the byte data
                std::cerr << "Failed to read the byte data." << std::endl;
                break;
            }

            total_bytes_read += bytes_read;
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by function: "
            << duration.count() * 0.001 << " miliseconds" << endl;


        std::cout << "bytes_read : " << bytes_read << "\n";
        for(int i=0; i<10; i++){
            std::cout << "byte number " << i << " " << int(buffer[i]) << "\n";
        }

        start = high_resolution_clock::now();

        // Deserialize the frame
        frame = cv::imdecode(buffer, cv::IMREAD_COLOR);

        // // Preprocess frame and create blob
        // cv::Mat blob = cv::dnn::blobFromImage(frame, 1 / 255.0, cv::Size(416, 416), cv::Scalar(0, 0, 0), true, false);

        // // Forward pass through the network
        // net.setInput(blob);
        // std::vector<cv::Mat> outs;
        // net.forward(outs, net.getUnconnectedOutLayersNames());

        // // Parse output and draw bounding boxes
        // float confidenceThreshold = 0.5;  // Confidence threshold for filtering detections
        // float nmsThreshold = 0.4;  // Non-maximum suppression threshold
        // std::vector<int> classIds;
        // std::vector<float> confidences;
        // std::vector<cv::Rect> boxes;

        // for (const auto& out : outs) {
        //     // Get confidence, class ID, and bounding box
        //     for (int i = 0; i < out.rows; ++i) {
        //         cv::Mat scores = out.row(i).colRange(5, out.cols);
        //         cv::Point classIdPoint;
        //         double confidence;
        //         cv::minMaxLoc(scores, nullptr, &confidence, nullptr, &classIdPoint);
        //         if (confidence > confidenceThreshold) {
        //             int centerX = static_cast<int>(out.at<float>(i, 0) * frame.cols);
        //             int centerY = static_cast<int>(out.at<float>(i, 1) * frame.rows);
        //             int width = static_cast<int>(out.at<float>(i, 2) * frame.cols);
        //             int height = static_cast<int>(out.at<float>(i, 3) * frame.rows);
        //             int left = centerX - width / 2;
        //             int top = centerY - height / 2;

        //             classIds.push_back(classIdPoint.x);
        //             confidences.push_back(static_cast<float>(confidence));
        //             boxes.emplace_back(left, top, width, height);
        //         }
        //     }
        // }

        // // Perform non-maximum suppression to remove overlapping bounding boxes
        // std::vector<int> indices;
        // cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);

        // // Draw the filtered bounding boxes
        // for (int i : indices) {
        //     const cv::Rect& box = boxes[i];
        //     cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2);

        //     std::string label = classLabels[classIds[i]];
        //     std::string confidenceLabel = cv::format("%.2f", confidences[i]);
        //     std::string text = label + ": " + confidenceLabel;
        //     cv::putText(frame, text, cv::Point(box.x, box.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
        // }

        // stop = high_resolution_clock::now();
        // duration = duration_cast<microseconds>(stop - start);
        // cout << "Time taken by function: "
        //     << duration.count() * 0.001 << " miliseconds" << endl;

        // std::cout << "isempty : " << frame.empty() << "\n";
        // std::cout << "fps : " << 1/(duration.count()*0.000001) << std::endl;

        // cv::putText(frame, "FPS " + std::to_string(1/(duration.count()*0.000001)), cv::Point(10,30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255));

        // Display the received frame
        cv::imshow("Received Frame", frame);


        std::cout << "--------------------- " << counter++ << std::endl;

        // Exit if 'q' is pressed
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Release resources
    close(new_socket);
    close(server_socket);
    cv::destroyAllWindows();

    return 0;
}
