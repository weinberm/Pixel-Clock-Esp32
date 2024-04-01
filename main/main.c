#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "led_matrix.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void) {
    led_strip_control_t control;
    init_led_matrix(&control);

    // Startwerte für Stunden und Minuten
    int hours = 11;
    int minutes = 7;

    while (true) {
        // Aktualisieren Sie die Stunden und Minuten auf der LED-Matrix
        set_pixel_number(0, hours / 10);    // Erste Stunde Ziffer
        set_pixel_number(1, hours % 10);    // Zweite Stunde Ziffer
        set_pixel_number(2, minutes / 10);  // Erste Minute Ziffer
        set_pixel_number(3, minutes % 10);  // Zweite Minute Ziffer
        transmit_pixels(&control);

        // Warte eine Minute (Simulation)
        vTaskDelay(pdMS_TO_TICKS(60000)); // Wartet 60 Sekunden (in einer echten Uhr, für Simulationen, diesen Wert anpassen)

        // Inkrementiere die Minuten
        minutes++;

        // Stunden und Minuten aktualisieren
        if (minutes >= 60) {
            minutes = 0; // Zurücksetzen der Minuten auf 0
            hours++;     // Stunden inkrementieren
        }

        if (hours >= 24) {
            hours = 0; // Zurücksetzen der Stunden auf 0 nach einem Tag
        }
    }
}
