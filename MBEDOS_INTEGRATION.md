# Integración con MbedOS

## Resumen

Esta librería ha sido modificada para soportar funcionalidades específicas de MbedOS que reemplazan el uso de `yield()` por alternativas más eficientes y apropiadas para sistemas empotrados.

## Cambios Implementados

### 1. Función `nonBlockingDelay(uint32_t ms)`

**Antes (Arduino):**

```cpp
void Adafruit_BNO055::nonBlockingDelay(uint32_t ms) {
  uint32_t start = millis();
  while (millis() - start < ms) {
    yield();
  }
}
```

**Después (MbedOS compatible):**

```cpp
void Adafruit_BNO055::nonBlockingDelay(uint32_t ms) {
#ifdef MBED_H
  // Para MbedOS, usar ThisThread::sleep_for para pausas no bloqueantes
  ThisThread::sleep_for(chrono::milliseconds(ms));
#else
  // Fallback para Arduino u otros sistemas
  uint32_t start = millis();
  while (millis() - start < ms) {
    yield();
  }
#endif
}
```

### 2. Nueva función `nonBlockingMicroDelay(uint32_t us)`

Esta función proporciona pausas precisas en microsegundos, útil para sincronización de hardware:

```cpp
void Adafruit_BNO055::nonBlockingMicroDelay(uint32_t us) {
#ifdef MBED_H
  // Para MbedOS, usar wait_us para pausas cortas precisas
  wait_us(us);
#else
  // Fallback para Arduino u otros sistemas
  delayMicroseconds(us);
#endif
}
```

### 3. Nueva función `yieldThread()`

Permite ceder el control del hilo de manera explícita:

```cpp
void Adafruit_BNO055::yieldThread() {
#ifdef MBED_H
  // Para MbedOS, usar ThisThread::yield para ceder control del hilo
  ThisThread::yield();
#else
  // Fallback para Arduino u otros sistemas
  yield();
#endif
}
```

## Funcionalidades de MbedOS Utilizadas

### 1. `ThisThread::sleep_for(chrono::milliseconds(ms))`

-   **Propósito**: Pausas no bloqueantes en milisegundos
-   **Ventajas**:
    -   Más eficiente en energía que polling
    -   Permite al scheduler ejecutar otros hilos
    -   Temporización precisa basada en hardware

### 2. `wait_us(us)`

-   **Propósito**: Pausas cortas y precisas en microsegundos
-   **Ventajas**:
    -   Temporización muy precisa para hardware crítico
    -   Optimizada para operaciones de bajo nivel
    -   No cambia de contexto para pausas muy cortas

### 3. `ThisThread::yield()`

-   **Propósito**: Ceder control voluntariamente
-   **Ventajas**:
    -   Permite que otros hilos de la misma prioridad se ejecuten
    -   Útil en loops cooperativos
    -   No consume tiempo de CPU innecesariamente

## Compatibilidad

La librería mantiene compatibilidad con Arduino mediante el uso de directivas de preprocesador `#ifdef MBED_H`. Si MbedOS no está presente, se utilizan las funciones originales de Arduino.

## Beneficios en MbedOS

1. **Mejor gestión de energía**: `ThisThread::sleep_for()` permite al procesador entrar en estados de bajo consumo
2. **Scheduling más eficiente**: El scheduler puede ejecutar otros hilos durante las pausas
3. **Temporización más precisa**: Basada en timers de hardware en lugar de polling
4. **Mejor responsividad del sistema**: Los hilos de mayor prioridad pueden interrumpir apropiadamente

## Uso Recomendado

En aplicaciones MbedOS, la librería ahora aprovecha automáticamente las funcionalidades del RTOS para un mejor rendimiento y eficiencia energética. No se requieren cambios en el código de usuario, ya que las mejoras están implementadas internamente en la librería.
