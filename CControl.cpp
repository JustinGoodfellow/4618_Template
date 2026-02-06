#include "stdafx.h"
#include "CControl.h"
#include <chrono>
#include <thread>

#define ADC_SCALE 4096
#define TIMEOUT_MS 50
#define DEBOUNCE_MS 10

std::atomic<bool> connected(false);
std::mutex control_mutex;

CControl::CControl()
{
}

CControl::~CControl()
{
}

void watchdog_thread(CControl* control_ptr);

void CControl::init_com(int comport)
{
    if (comport > 0)
    {
        std::string portName = "COM" + std::to_string(comport);
        _com.open(portName, 115200);
        connected = true;
    }
    if (comport < 0)
    {
        connected = false;
        std::thread wd(watchdog_thread, this);
        wd.detach();
    }
}

void watchdog_thread(CControl* control_ptr)
{
    while (1)
    {
        bool is_connected = false;
        {
            std::lock_guard<std::mutex> lock(control_mutex);
            if (connected)
            {
                int value;
                if (control_ptr->get_data(ANALOG, 0, value))
                {
                    is_connected = true;
                }
            }
        }
        
        if (!is_connected)
        {
			std::cout << "\nAttempting to connect..." << std::endl;
            for (int port = 1; port < 20; port++)
            {
                try
                {
					control_ptr->init_com(port);
                    int value;
                    if (control_ptr->get_data(ANALOG, 0, value))
                    {
                        std::lock_guard<std::mutex> lock(control_mutex);
                        connected = true;
                        std::cout << "Connected on " << port << std::endl;
                        is_connected = true;
                        break;
                    }
                }
                catch (...) {}
            }
            if (!is_connected)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
		}
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool CControl::get_data(int type, int channel, int& result)
{
    _com.flush();
    char buffer[16] = {};
    int len = snprintf(buffer, sizeof(buffer), "G %d %d\n", type, channel);
    _com.write(buffer, len);

    std::string response;
    char readBuffer[16];

    auto start = std::chrono::steady_clock::now();

    while (1)
    {
        int n = _com.read(readBuffer, sizeof(readBuffer));
        if (n > 0)
        {
            response.append(readBuffer, n);
            size_t pos;
            while ((pos = response.find('\n')) != std::string::npos)
            {
                std::string line = response.substr(0, pos);
                response.erase(0, pos + 1);

                int t, c, value;
                if (sscanf(line.c_str(), "A %d %d %d", &t, &c, &value) == 3)
                {
                    if (t == type && c == channel)
                    {
#ifdef DEBUG
                        std::cout << "Sent: " << buffer
                            << " Received: " << response.c_str() << std::endl;
#endif
                        result = value;
                        return true;
                    }
                }
            }
        }
        if (std::chrono::steady_clock::now() - start >
            std::chrono::milliseconds(TIMEOUT_MS))
        {
            //std::lock_guard<std::mutex> lock(control_mutex);
            //connected = false;
            return false;
        }
        Sleep(1);
    }
}

bool CControl::set_data(int type, int channel, int val)
{
	char buffer[16];
	int len = snprintf(buffer, sizeof(buffer), "S %d %d %d\n", type, channel, val);
	_com.write(buffer, len);
	return true;
}

bool CControl::get_analog(int channel, float& result)
{
	Sleep(10);
	int sample = 0; 
	if (!get_data(ANALOG, channel, sample)) 
		return false;

	const int adcMax = ADC_SCALE - 1;
	if (sample < 0) sample = 0;
	if (sample > adcMax) sample = adcMax;

	result = (static_cast<float>(sample) / static_cast<float>(adcMax)) * 100.f;
	return true;
}

bool CControl::get_button(int channel)
{
    int last_button_state = 1;
    auto last_change_time = std::chrono::steady_clock::now();
    const auto debounce_time = std::chrono::milliseconds(DEBOUNCE_MS);

    while (1)
    {
        int button_state = 1;
        get_data(DIGITAL, channel, button_state);
        auto now = std::chrono::steady_clock::now();
        if (button_state != last_button_state)
        {
            last_change_time = now;
        }
        if (now - last_change_time > debounce_time)
        {
			if (button_state == 0)
                return true;
            else
				return false;
        }
        last_button_state = button_state;
        Sleep(1);
    }
}
