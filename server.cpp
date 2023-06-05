#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8000  // Port number to listen on

int main() {
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

    while (true) {
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
        
        std::cout << "bytes_read : " << bytes_read << "\n";
        for(int i=0; i<10; i++){
            std::cout << "byte number " << i << " " << int(buffer[i]) << "\n";
        }

        // ssize_t bytes_read = read(new_socket, buffer.data(), message_size);
        // if(bytes_read == 0){
        //     perror("Client disconnected");
        //     break;
        // }else if(bytes_read < 0){
        //     // Error reading the byte data
        //     std::cerr << "Failed to read the byte data." << std::endl;
        //     break;
        // }
        // std::cout << "bytes_read : " << bytes_read << "\n";
        // std::cout << "first_byte : " << buffer[0] << "\n";

        // Deserialize the frame
        frame = cv::imdecode(buffer, cv::IMREAD_COLOR);

        std::cout << "isempty : " << frame.empty() << "\n";

        // Display the received frame
        cv::imshow("Received Frame", frame);

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
