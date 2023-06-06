#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>

int main() {
    // Load YOLO model
    cv::dnn::Net net = cv::dnn::readNetFromDarknet("yolov3.cfg", "yolov3.weights");

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

    // Initialize webcam
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Failed to open the webcam." << std::endl;
        return -1;
    }

    // Process frames from webcam
    cv::Mat frame;
    while (cap.read(frame)) {
        // Preprocess frame and create blob
        cv::Mat blob = cv::dnn::blobFromImage(frame, 1 / 255.0, cv::Size(416, 416), cv::Scalar(0, 0, 0), true, false);

        // Forward pass through the network
        net.setInput(blob);
        std::vector<cv::Mat> outs;
        net.forward(outs, net.getUnconnectedOutLayersNames());

        // Parse output and draw bounding boxes
        float confidenceThreshold = 0.5;  // Confidence threshold for filtering detections
        float nmsThreshold = 0.4;  // Non-maximum suppression threshold
        std::vector<int> classIds;
        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;

        for (const auto& out : outs) {
            // Get confidence, class ID, and bounding box
            for (int i = 0; i < out.rows; ++i) {
                cv::Mat scores = out.row(i).colRange(5, out.cols);
                cv::Point classIdPoint;
                double confidence;
                cv::minMaxLoc(scores, nullptr, &confidence, nullptr, &classIdPoint);
                if (confidence > confidenceThreshold) {
                    int centerX = static_cast<int>(out.at<float>(i, 0) * frame.cols);
                    int centerY = static_cast<int>(out.at<float>(i, 1) * frame.rows);
                    int width = static_cast<int>(out.at<float>(i, 2) * frame.cols);
                    int height = static_cast<int>(out.at<float>(i, 3) * frame.rows);
                    int left = centerX - width / 2;
                    int top = centerY - height / 2;

                    classIds.push_back(classIdPoint.x);
                    confidences.push_back(static_cast<float>(confidence));
                    boxes.emplace_back(left, top, width, height);
                }
            }
        }

        // Perform non-maximum suppression to remove overlapping bounding boxes
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, nmsThreshold, indices);

        // Draw the filtered bounding boxes
        for (int i : indices) {
            const cv::Rect& box = boxes[i];
            cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2);

            std::string label = classLabels[classIds[i]];
            std::string confidenceLabel = cv::format("%.2f", confidences[i]);
            std::string text = label + ": " + confidenceLabel;
            cv::putText(frame, text, cv::Point(box.x, box.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
        }

        // Show the frame
        cv::imshow("YOLO Webcam", frame);

        // Exit loop if 'q' is pressed
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Release the webcam and destroy windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
