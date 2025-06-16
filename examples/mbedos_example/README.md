# Ejemplo BNO055 para MbedOS

Este ejemplo demuestra cómo usar la librería Adafruit_BNO055 con las funcionalidades específicas de MbedOS para un mejor rendimiento y gestión de energía.

## Características

-   **Uso de hilos (threads)**: El ejemplo utiliza un hilo separado para la lectura del sensor
-   **Sincronización thread-safe**: Usa mutexes para proteger el acceso a datos compartidos
-   **Pausas no bloqueantes**: Utiliza `ThisThread::sleep_for()` en lugar de delays bloqueantes
-   **Yield cooperativo**: Usa `ThisThread::yield()` para una mejor gestión de hilos

## Funcionalidades de MbedOS Utilizadas

### ThisThread::sleep_for()

Reemplaza las funciones de delay tradicionales:

```cpp
// En lugar de delay(100) o nonBlockingDelay(100)
ThisThread::sleep_for(chrono::milliseconds(100));
```

### ThisThread::yield()

Permite ceder el control del procesador:

```cpp
// Ceder control a otros hilos de la misma prioridad
ThisThread::yield();
```

### Mutex

Para proteger datos compartidos entre hilos:

```cpp
Mutex data_mutex;
data_mutex.lock();
// Acceso a datos compartidos
data_mutex.unlock();
```

## Compilación

Para compilar este ejemplo en MbedOS:

1. Asegúrate de tener MbedOS 6.0 o superior
2. Incluye la librería Adafruit_BNO055 en tu proyecto
3. Compila con:
    ```bash
    mbed compile -m <tu_target> -t GCC_ARM
    ```

## Beneficios

1. **Mejor eficiencia energética**: Los hilos pueden dormir permitiendo al procesador entrar en modos de bajo consumo
2. **Mayor responsividad**: Otros hilos pueden ejecutarse mientras uno está en pausa
3. **Gestión automática**: La librería detecta automáticamente MbedOS y usa las funciones apropiadas
4. **Compatibilidad**: El código sigue funcionando en Arduino sin modificaciones

## Configuración

El ejemplo puede configurarse mediante `mbed_lib.json`:

-   `i2c-frequency`: Frecuencia del bus I2C (por defecto 400kHz)
-   `sensor-sample-rate`: Tasa de muestreo en milisegundos (por defecto 100ms)
