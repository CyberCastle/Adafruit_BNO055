/*
 * Example for MbedOS with BNO055 demonstrating the use of the
 * MbedOS specific helpers to avoid blocking and improve power usage
 */

#include "Adafruit_BNO055.h"
#include "mbed.h"

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

// BNO055 sensor instance
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

// Thread for sensor readings
Thread sensor_thread;

// Mutex to protect shared data access
Mutex data_mutex;

// Shared variables
sensors_event_t orientationData;
bool new_data_available = false;

/**
 * @brief Thread function to continuously read sensor data
 */
void sensor_reading_task() {
    while (true) {
        sensors_event_t temp_data;

        // Read data from the sensor
        if (bno.getEvent(&temp_data, Adafruit_BNO055::VECTOR_EULER)) {
            // Protect access to shared data
            data_mutex.lock();
            orientationData = temp_data;
            new_data_available = true;
            data_mutex.unlock();
        }

        // Use the non-blocking delay to allow other threads to run
        ThisThread::sleep_for(chrono::milliseconds(BNO055_SAMPLERATE_DELAY_MS));
    }
}

int main() {
    printf("BNO055 MbedOS Example\n");

    // Initialize the sensor
    if (!bno.begin()) {
        printf("No BNO055 detected ... Check your wiring or I2C ADDR!\n");
        return -1;
    }

    printf("BNO055 initialized successfully\n");

    // Start sensor reading thread
    sensor_thread.start(sensor_reading_task);

    // Main loop
    while (true) {
        // Check if new data is available
        data_mutex.lock();
        if (new_data_available) {
            printf("Orientation - X: %.2f, Y: %.2f, Z: %.2f\n",
                   orientationData.orientation.x,
                   orientationData.orientation.y,
                   orientationData.orientation.z);
            new_data_available = false;
        }
        data_mutex.unlock();

        // Yield to other threads (similar to yield() in Arduino)
        ThisThread::yield();

        // Short pause to avoid saturating the CPU
        ThisThread::sleep_for(chrono::milliseconds(50));
    }

    return 0;
}
