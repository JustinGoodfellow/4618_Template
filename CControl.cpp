#include "stdafx.h"
#include "CControl.h"
#include <chrono>
#include <thread>

#define ADC_SCALE 4096
#define TIMEOUT_MS 50
#define DEBOUNCE_MS 10

std::atomic<bool> connected(false);
std::mutex com_mutex;

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
        for(int port = 1; port <= 10; port++)
{
            std::string portName = "COM" + std::to_string(port);
            if (_com.open(portName, 115200))
    {
//				std::cout << "\nConnected to " << portName << std::endl;
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
    //_com.flush();
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
			std::cerr << "\nTimeout waiting for response to: " << buffer;
            _com.~Serial();
            init_com();
            Sleep(500);
            return false;
        }
        //Sleep(1);
    }
}

bool CControl::set_data(int type, int channel, int val)
{
	char buffer[16];
	int len = snprintf(buffer, sizeof(buffer), "S %d %d %d\n", type, channel, val);
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
                        std::cout << "\nSent: " << buffer << "Received: " << line.c_str() << std::endl;
#endif
                        return true;
                    }
                }
            }
        }
        if (std::chrono::steady_clock::now() - start >
            std::chrono::milliseconds(TIMEOUT_MS))
        {
            std::cerr << "\nTimeout waiting for response to: " << buffer;
            _com.~Serial();
            init_com();
            Sleep(500);
            return false;
        }
        //Sleep(1);
    }
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

//bool CControl::get_button(int channel)
//{
//    int last_button_state = 1;
//    auto last_change_time = std::chrono::steady_clock::now();
//    const auto debounce_time = std::chrono::milliseconds(DEBOUNCE_MS);
//
//    while (1)
//    {
//        int button_state = 1;
//        get_data(DIGITAL, channel, button_state);
//        auto now = std::chrono::steady_clock::now();
//        if (button_state != last_button_state)
//        {
//            last_change_time = now;
//        }
//        if (now - last_change_time > debounce_time)
//        {
//			if (button_state == 0)
//                return true;
//            else
//				return false;
//        }
//        last_button_state = button_state;
//        Sleep(1);
//    }
//}

bool CControl::get_button(int channel)
{
    int raw_state = 1;
    if (!get_data(DIGITAL, channel, raw_state))
        return false;

    while (1)
    {
        int button_state = 1;
        get_data(DIGITAL, channel, button_state);
        auto now = std::chrono::steady_clock::now();
    const auto debounce = std::chrono::milliseconds(DEBOUNCE_MS);

    if (_last_raw_state.count(channel) == 0)
        {
        _last_raw_state[channel] = raw_state;
        _stable_state[channel] = raw_state;
        _last_change_time[channel] = now;
        return false;
        }

    if (raw_state != _last_raw_state[channel])
        {
        _last_raw_state[channel] = raw_state;
        _last_change_time[channel] = now;
				return false;
        }

    if (now - _last_change_time[channel] < debounce)
        return false;

    if (raw_state != _stable_state[channel])
    {
        int prev = _stable_state[channel];
        _stable_state[channel] = raw_state;

        if (prev == 1 && raw_state == 0)
            return true;
    }

    return false;
}


