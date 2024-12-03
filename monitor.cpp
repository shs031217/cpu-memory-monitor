#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

// CPU 사용량 계산 함수
float calculateCPUUsage() {
    static long prevIdleTime = 0, prevTotalTime = 0;

    ifstream cpuFile("/proc/stat");
    if (!cpuFile.is_open()) {
        cerr << "Error: Unable to open /proc/stat" << endl;
        return 0.0;
    }

    string line, cpu;
    long user, nice, system, idle, iowait, irq, softirq, steal;

    if (getline(cpuFile, line)) {
        istringstream iss(line);
        iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

        long idleTime = idle + iowait;
        long totalTime = user + nice + system + idle + iowait + irq + softirq + steal;

        long diffIdle = idleTime - prevIdleTime;
        long diffTotal = totalTime - prevTotalTime;

        prevIdleTime = idleTime;
        prevTotalTime = totalTime;

        return (1.0 - static_cast<float>(diffIdle) / diffTotal) * 100.0;
    }

    return 0.0;
}

// 메모리 사용량 계산 함수
void displayMemoryUsage() {
    ifstream memFile("/proc/meminfo");
    if (!memFile.is_open()) {
        cerr << "Error: Unable to open /proc/meminfo" << endl;
        return;
    }

    string key, unit;
    long value;
    long memTotal = 0, memFree = 0, buffers = 0, cached = 0;

    while (memFile >> key >> value >> unit) {
        if (key == "MemTotal:") memTotal = value;
        if (key == "MemFree:") memFree = value;
        if (key == "Buffers:") buffers = value;
        if (key == "Cached:") cached = value;
    }

    long memUsed = memTotal - (memFree + buffers + cached);
    cout << "Memory Usage: " << memUsed / 1024 << " MB / " << memTotal / 1024 << " MB (" 
         << static_cast<float>(memUsed) / memTotal * 100.0 << "%)" << endl;
}

// 메인 함수
int main() {
    cout << "Starting CPU and Memory Monitor..." << endl;

    while (true) {
        float cpuUsage = calculateCPUUsage();
        cout << "\nCPU Usage: " << cpuUsage << " %" << endl;

        displayMemoryUsage();

        this_thread::sleep_for(chrono::seconds(1)); // 1초 간격으로 갱신
    }

    return 0;
}