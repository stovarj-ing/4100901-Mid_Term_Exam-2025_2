#include "room_control.h"

#include "gpio.h"    // Para controlar LEDs
#include "systick.h" // Para obtener ticks y manejar tiempos
#include "uart.h"    // Para enviar mensajes
#include "tim.h"     // Para controlar el PWM
// Estados de la sala
typedef enum {
    ROOM_IDLE,
    ROOM_OCCUPIED
} room_state_t;

// Variable de estado global
room_state_t current_state = ROOM_IDLE;
static uint32_t led_on_time = 0;
static uint8_t elduty = 0;
extern volatile uint32_t system_ms_counter;
extern volatile uint8_t last_duty;
static uint8_t time = 0;



void room_control_app_init(void)
{
    // Inicializar PWM al duty cycle inicial (estado IDLE -> LED apagado)
    tim3_ch1_pwm_set_duty_cycle(PWM_INITIAL_DUTY);// PWM al 20%
    uart_send_string("Control de Sala v2.0\r\n");
    uart_send_string("Estado: - Lámpara: 20%\r\n Puerta: Cerrada\r\n");

}

void room_control_on_button_press(void)
{
    
    
    if (current_state == ROOM_IDLE) {
        current_state = ROOM_OCCUPIED;
        tim3_ch1_pwm_set_duty_cycle(100);  // PWM al 100% al presionar el botón
        led_on_time = systick_get_ms();
        

    } else {
        current_state = ROOM_IDLE;
        tim3_ch1_pwm_set_duty_cycle(0);  // PWM al 0%
        uart_send_string("Sala vacía\r\n");
    }
}


void room_control_on_uart_receive(char received_char)
{
    switch (received_char) {
        case 'h':
        case 'H':
            tim3_ch1_pwm_set_duty_cycle(100);
            uart_send_string("PWM: 100%\r\n");
            break;
        case 'l':
        case 'L':
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("PWM: 0%\r\n");
            break;
        case 'O':
        case 'o':
            current_state = ROOM_OCCUPIED;
            tim3_ch1_pwm_set_duty_cycle(100);
            led_on_time = systick_get_ms();
            uart_send_string("Sala ocupada\r\n");
            break;
        case 'I':
        case 'i':
            current_state = ROOM_IDLE;
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("Sala vacía\r\n");
            break;
        case '1':
            tim3_ch1_pwm_set_duty_cycle(10);
            uart_send_string("PWM: 10%\r\n");
            break;
        case '2':
            tim3_ch1_pwm_set_duty_cycle(20);
            uart_send_string("PWM: 20%\r\n");
            break;
        case '3':
            tim3_ch1_pwm_set_duty_cycle(30);
            uart_send_string("PWM: 30%\r\n");
            break;
        case '4':
            tim3_ch1_pwm_set_duty_cycle(40);
            uart_send_string("PWM: 40%\r\n");
            break;
        case '5':
            tim3_ch1_pwm_set_duty_cycle(50);
            uart_send_string("PWM: 50%\r\n");
            break;
        case 's':
        case 'S':
            uart_send_string("Estado actual: ");
            tim3_ch1_pwm_set_duty_cycle(last_duty);
            uart_send(last_duty);
            if (current_state == ROOM_OCCUPIED) {
                uart_send_string("Puerta: Abierta\r\n");
            } else {
                uart_send_string("Puerta: Cerrada\r\n");
            }
            break;
        case '?': 
            uart_send_string("Comandos disponibles:\r\n");
            uart_send_string(" H/h: Encender lámpara (PWM 100%)\r\n");
            uart_send_string(" L/l: Apagar lámpara (PWM 0%)\r\n");
            uart_send_string(" O/o: Marcar sala como ocupada\r\n");
            uart_send_string(" I/i: Marcar sala como vacía\r\n");
            uart_send_string(" 1-5: Ajustar PWM de lámpara (10%-50%)\r\n");
            uart_send_string(" S/s: Estado actual de la sala\r\n");
            uart_send_string(" ?: Mostrar esta ayuda\r\n");
            break;
        case 'g':
            for (elduty = 0; elduty <= 100; elduty+= 10) {
                tim3_ch1_pwm_set_duty_cycle(elduty);
                uart_send_string("PWM: ");
                uart_send(elduty + '0');
                uart_send_string("%\r\n");
            }
        break;
        default:
            uart_send_string("Comando desconocido: ");
            uart_send(received_char);
            uart_send_string("\r\n");
            break;
    }
}

void room_control_update(void)
{
   if ( time - system_ms_counter >= 10000) {
            tim3_ch1_pwm_set_duty_cycle(last_duty);  // Restaurar al último duty cycle después de 10s
            uart_send_string("Restaurando último duty cycle\r\n");
    }
        
       
 
}