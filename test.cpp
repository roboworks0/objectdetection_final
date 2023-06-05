#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <vector>

template <typename T>
T SwapBytes(T value)
{
    T result = 0;
    size_t numBytes = sizeof(T);
    unsigned char* ptrValue = reinterpret_cast<unsigned char*>(&value);
    unsigned char* ptrResult = reinterpret_cast<unsigned char*>(&result);

    for (size_t i = 0; i < numBytes; ++i)
    {
        ptrResult[i] = ptrValue[numBytes - i - 1];
    }

    return result;
}

int main()
{
    // Create a socket
    int server_fd, new_socket;
    sockaddr_in address;
    int addrlen = sizeof(address);

    // Create a socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8000);

    // Bind the socket to localhost:8000
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0)
    {
        perror("listen failed");
        return 1;
    }

    // Accept the incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept failed");
        return 1;
    }

    // Receive and process byte data continuously
    while (true)
    {
        printf("debug\n");
        // Read the size of the byte data (4-byte unsigned integer)
        uint32_t data_size;
        ssize_t bytes_read = read(new_socket, &data_size, sizeof(data_size));
        std::cout << "size : " << bytes_read << "\n";
        if (bytes_read == 0)
        {
            std::cout << "break1" << "\n";
            // Connection closed by the sender
            break;
        }
        else if (bytes_read != sizeof(data_size))
        {
            std::cout << "break2" << "\n";
            // Error reading the size
            std::cerr << "Failed to read the data size." << std::endl;
            break;
        }
        data_size = SwapBytes(data_size);

        std::cout << "data_size : " << data_size << "\n";

        // Resize the buffer to accommodate the incoming byte data
        std::vector<uint8_t> buffer(data_size);

        // Read the byte data
        ssize_t total_bytes_read = 0;
        while (total_bytes_read < data_size)
        {
            std::cout << "total_bytes_read : " << total_bytes_read << "\n";
            ssize_t bytes_read = read(new_socket, buffer.data() + total_bytes_read, data_size - total_bytes_read);
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

        // Process the received byte data
        for (size_t i = 0; i < buffer.size(); i++)
        {
            std::cout << (int)buffer[i] << " - ";
        }
        std::cout << "\n";
    }

    // Close the socket
    close(new_socket);
    close(server_fd);

    return 0;
}
