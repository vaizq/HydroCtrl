#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "camera.hpp"
#include <uvgrtp/lib.hh>
#include <linux/videodev2.h>


class VideoServer
{
public:
    VideoServer(const char* address, int port)
        : m_cam("/dev/video0", V4L2_PIX_FMT_MJPEG, 1280, 720),
          m_address(address), m_port(port)
    {
        m_sess = m_ctx.create_session(address);
    }

    ~VideoServer()
    {
        if (m_sess)
            m_ctx.destroy_session(m_sess);
    }

    void start()
    {
        running = true;

        m_cam.start();

        constexpr int flags = RCE_FRAGMENT_GENERIC | RCE_SEND_ONLY;
        uvgrtp::media_stream* send = m_sess->create_stream(m_port, RTP_FORMAT_GENERIC, flags);

        while (!m_cam.hasFrame()) {} // Wait until camera has frames


        while (running)
        {
            auto frame = m_cam.getFrame();    

            auto t0 = std::chrono::steady_clock::now();

            if (send->push_frame(frame.data(), frame.size(), RTP_NO_FLAGS) != RTP_OK)
            {
                std::cerr << "Failed to send frame\n";
                running = false;
                break;
            }

            auto t1 = std::chrono::steady_clock::now();

            if (std::chrono::milliseconds(33) > (t1 - t0))
            {
                auto sleepTime = std::chrono::milliseconds(33) - (t1 - t0);
                std::this_thread::sleep_for(sleepTime);
            }
        }

        m_sess->destroy_stream(send);
    }

    void stop()
    {
        running = false;
    }

private:
    Camera m_cam;
    uvgrtp::context m_ctx;
    uvgrtp::session *m_sess;
    bool running = false;
    std::string m_address;
    int m_port;
};


#endif
