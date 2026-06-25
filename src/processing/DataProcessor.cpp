#include "DataProcessor.h"
#include "../utils/Config.h"
#include <Arduino.h>

// Verifica que la lectura sea válida y esté dentro de rangos físicos aceptables.
// Retorna el mismo SensorData con valid = false si no pasa alguna verificación.
SensorData DataProcessor::validate(const SensorData& data) const {
    SensorData processed = data;

    // Rechaza si el sensor ya marcó la lectura como inválida o devolvió NaN
    if (!data.valid || isnan(data.temperature) || isnan(data.humidity)) {
        processed.valid = false;
        return processed;
    }

    // Rechaza si la temperatura está fuera del rango esperado para el vivero
    if (data.temperature < TEMP_MIN || data.temperature > TEMP_MAX) {
        processed.valid = false;
        return processed;
    }

    // Rechaza si la humedad está fuera del rango físicamente posible
    if (data.humidity < HUM_MIN || data.humidity > HUM_MAX) {
        processed.valid = false;
        return processed;
    }

    processed.valid = true;
    return processed;
}
