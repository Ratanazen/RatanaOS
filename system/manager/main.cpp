#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

class SystemManager {
public:
    void init() {
        std::cout << "[Ratana System Manager] Initializing Core Services...\n";
        startServices();
        collectHardwareInfo();
    }

    void run() {
        std::cout << "[Ratana System Manager] Daemon running in background.\n";
        // Simulate event loop
        for (int i = 0; i < 3; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "[Ratana System Manager] Heartbeat " << i + 1 << "\n";
        }
    }

private:
    void startServices() {
        std::vector<std::string> services = {
            "NetworkManager",
            "AudioDaemon",
            "DisplayServer"
        };
        for (const auto& service : services) {
            std::cout << " -> Starting " << service << "...\n";
        }
    }

    void collectHardwareInfo() {
        std::cout << " -> Hardware Info: CPU [OK], Memory [OK], Disks [OK]\n";
    }
};

int main() {
    SystemManager manager;
    manager.init();
    manager.run();
    std::cout << "[Ratana System Manager] Shutdown.\n";
    return 0;
}
