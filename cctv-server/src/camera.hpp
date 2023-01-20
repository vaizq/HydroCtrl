#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>              /* low-level i/o */
#include <sys/mman.h>
#include <linux/videodev2.h>



using Image = std::vector<uint8_t>;
using guard = std::lock_guard<std::mutex>;
using namespace std::string_literals;

class Camera
{
public:
    Camera(const char* device, int pixelFormat, unsigned int width, unsigned int height)
    {
        // Open device
        m_fd = open(device, O_RDWR);
        if (m_fd < 0)
        {
            throw std::runtime_error("Failed to open device\n");
        }


        // Check capabilites
        struct v4l2_capability cap;
        if(ioctl(m_fd, VIDIOC_QUERYCAP, &cap) < 0){
            throw std::runtime_error("VIDIOC_QUERYCAP");
        }

        std::cout << "Device: " << cap.card << '\n'; 
        std::cout << "Driver: " << cap.driver << '\n';
        std::cout << "Bus-info: " << cap.bus_info << '\n';
        
        if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
            throw std::runtime_error("The device does not handle single-planar video capture.\n");
        }

        if(!(cap.capabilities & V4L2_CAP_STREAMING)){
            throw std::runtime_error("The device does not handle streaming.\n");
        }
        
        // Spesify format
        struct v4l2_format format;
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        format.fmt.pix.pixelformat = pixelFormat;
        format.fmt.pix.width = width;
        format.fmt.pix.height = height;

        if(ioctl(m_fd, VIDIOC_S_FMT, &format) < 0){
            throw std::runtime_error("VIDIOC_S_FMT: "s + strerror(errno));
        }

        // Spesify request buffer 
        struct v4l2_requestbuffers bufrequest;
        bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufrequest.memory = V4L2_MEMORY_MMAP;
        bufrequest.count = 1;

        if(ioctl(m_fd, VIDIOC_REQBUFS, &bufrequest) < 0){
            std::cerr << "VIDIOC_REQBUFS\n";
            exit(1);
        }

        // Spesify query buffer
        struct v4l2_buffer bufferinfo;
        memset(&bufferinfo, 0, sizeof(bufferinfo));

        bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        bufferinfo.memory = V4L2_MEMORY_MMAP;
        bufferinfo.index = 0;

        if(ioctl(m_fd, VIDIOC_QUERYBUF, &bufferinfo) < 0){
            throw std::runtime_error("VIDIOC_QUERYBUF\n");
        }

        // Memory map
        m_buffer = mmap(
            NULL,
            bufferinfo.length,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            m_fd,
            bufferinfo.m.offset
        );
        if(m_buffer == MAP_FAILED){
            throw std::runtime_error("mmap\n");
        }
        memset(m_buffer, 0, bufferinfo.length);
    }

    ~Camera()
    {
        m_mtx.lock();
        m_running = false;
        m_mtx.unlock();

        m_thread->join();

        // Deactivate streaming
        if(ioctl(m_fd, VIDIOC_STREAMOFF, &m_type) < 0){
            std::cerr << "VIDIOC_STREAMOFF\n";
        }

        guard g(m_mtx);
        munmap(m_buffer, m_bufferInfo.length);

        close(m_fd);
    }

    void start()
    {
        m_running = true; 

        auto videoLoop =
            [this]() 
            {
                resetBufferInfo();

                // Put the buffer in the incoming queue.
                if(ioctl(m_fd, VIDIOC_QBUF, &m_bufferInfo) < 0){
                    throw std::runtime_error("VIDIOC_QBUF\n");
                }

                // Activate streaming
                m_type = m_bufferInfo.type;
                if(ioctl(m_fd, VIDIOC_STREAMON, &m_type) < 0){
                    throw std::runtime_error("VIDIOC_STREAMON\n");
                }

                while (m_running)
                {
                    // The buffer's waiting in the outgoing queue.
                    m_mtx.lock();
                    if(ioctl(m_fd, VIDIOC_DQBUF, &m_bufferInfo) < 0){
                        throw std::runtime_error("VIDIOC_DQBUF");
                    }

                    m_hasFrame = true;

                    m_bufferInfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    m_bufferInfo.memory = V4L2_MEMORY_MMAP;

                    // Queue next frame 
                    if(ioctl(m_fd, VIDIOC_QBUF, &m_bufferInfo) < 0){
                        throw std::runtime_error("VIDIOC_QBUF\n");
                    }
                    m_mtx.unlock();

                    // Sleep so client can use mutex
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            };

        m_thread = std::make_unique<std::thread>(videoLoop);
    }

    bool hasFrame()
    {
        guard g(m_mtx);
        return m_hasFrame;
    }

    Image getFrame()
    {
        guard g(m_mtx);
        Image img;
        img.insert(img.end(), static_cast<uint8_t*>(m_buffer), static_cast<uint8_t*>(m_buffer) + m_bufferInfo.length);
        return img;
    }

private:
    void resetBufferInfo()
    {
        memset(&m_bufferInfo, 0, sizeof(m_bufferInfo));
        m_bufferInfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        m_bufferInfo.memory = V4L2_MEMORY_MMAP;
        m_bufferInfo.index = 0;
    }

private:
    int m_fd;
    int m_type;
    bool m_hasFrame = false;
    bool m_running = false;
    void* m_buffer;
    struct v4l2_buffer m_bufferInfo;
    std::mutex m_mtx;
    std::unique_ptr<std::thread> m_thread;
};


#endif
