/*
 * Ejemplo para MbedOS con BNO055
 * Demuestra el uso de las funcionalidades específicas de MbedOS
 * para evitar bloqueos y mejorar la eficiencia energética
 */

#include "Adafruit_BNO055.h"
#include "mbed.h"

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

// Instancia del sensor BNO055
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

// Thread para lecturas del sensor
Thread sensor_thread;

// Mutex para proteger acceso a datos compartidos
Mutex data_mutex;

// Variables compartidas
sensors_event_t orientationData;
bool new_data_available = false;

void sensor_reading_task() {
    while (true) {
        sensors_event_t temp_data;

        // Leer datos del sensor
        if (bno.getEvent(&temp_data, Adafruit_BNO055::VECTOR_EULER)) {
            // Proteger acceso a datos compartidos
            data_mutex.lock();
            orientationData = temp_data;
            new_data_available = true;
            data_mutex.unlock();
        }

        // Usar la función de delay no bloqueante de MbedOS
        // Esto permite que otros hilos se ejecuten durante la pausa
        ThisThread::sleep_for(chrono::milliseconds(BNO055_SAMPLERATE_DELAY_MS));
    }
}

int main() {
    printf("BNO055 MbedOS Example\n");

    // Inicializar el sensor
    if (!bno.begin()) {
        printf("No BNO055 detected ... Check your wiring or I2C ADDR!\n");
        return -1;
    }

    printf("BNO055 inicializado correctamente\n");

    // Iniciar thread de lectura del sensor
    sensor_thread.start(sensor_reading_task);

    // Loop principal
    while (true) {
        // Verificar si hay nuevos datos disponibles
        data_mutex.lock();
        if (new_data_available) {
            printf("Orientación - X: %.2f, Y: %.2f, Z: %.2f\n",
                   orientationData.orientation.x,
                   orientationData.orientation.y,
                   orientationData.orientation.z);
            new_data_available = false;
        }
        data_mutex.unlock();

        // Ceder control a otros hilos
        // Esto es equivalente al yield() pero específico para MbedOS
        ThisThread::yield();

        // Pausa corta para no saturar el CPU
        ThisThread::sleep_for(chrono::milliseconds(50));
    }

    return 0;
}
