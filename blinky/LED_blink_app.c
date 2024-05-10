/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "tx_api.h"
#include "Driver_GPIO.h"
#include "pinconf.h"
#include <stdio.h>
#include "RTE_Components.h"
#if defined(RTE_Compiler_IO_STDOUT)
#include "retarget_stdout.h"
#endif  /* RTE_Compiler_IO_STDOUT */


#define LED_BLINK_THREAD_STACK_SIZE     (1024)

/* LED0 gpio pins */
#define GPIO12_PORT                     12
#define PIN3                            3

#define GPIO15_PORT                     15
#define PIN4                            4

TX_THREAD                               led_thread;
TX_EVENT_FLAGS_GROUP                    event_flags_button;

#define BUTTON_EVENT                    0x01

/* GPIO port used for LED0_R */
extern  ARM_DRIVER_GPIO ARM_Driver_GPIO_(GPIO12_PORT);
ARM_DRIVER_GPIO *gpioDrv12 = &ARM_Driver_GPIO_(GPIO12_PORT);

/* GPIO port used for button */
extern  ARM_DRIVER_GPIO ARM_Driver_GPIO_(GPIO15_PORT);
ARM_DRIVER_GPIO *gpioDrv15 = &ARM_Driver_GPIO_(GPIO15_PORT);


static void button_callback(uint32_t event)
{
    if (event == ARM_GPIO_IRQ_EVENT_EXTERNAL) {
        tx_event_flags_set(&event_flags_button, BUTTON_EVENT, TX_OR);
    }
}

/**
  \fn         void led_blink_app(ULONG thread_input)
  \brief      LED blinky function
  \param[in]  thread_input : thread input
  \return     none
*/
void led_blink_app (ULONG thread_input)
{
    int32_t ret1 = 0;
    uint8_t LED0_R = PIN3;
    uint8_t BUTTON = PIN4;

    printf("led blink demo application for ThreadX started\n\n");

    /* pinmux configurations for all GPIOs */
    pinconf_set(GPIO12_PORT, LED0_R, PINMUX_ALTERNATE_FUNCTION_0, 0);

    uint32_t config_button = PADCTRL_READ_ENABLE | PADCTRL_SCHMITT_TRIGGER_ENABLE |PADCTRL_DRIVER_DISABLED_PULL_UP;
    pinconf_set(GPIO15_PORT, BUTTON, PINMUX_ALTERNATE_FUNCTION_0, config_button);

    ret1 = gpioDrv12->Initialize(LED0_R, NULL);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("ERROR: Failed to initialize\n");
        return;
    }
    ret1 = gpioDrv15->Initialize(BUTTON, button_callback);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("ERROR: Failed to initialize\n");
        return;
    }

    ret1 = gpioDrv12->PowerControl(LED0_R, ARM_POWER_FULL);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("ERROR: Failed to powered full\n");
        goto error_uninitialize;
    }
    ret1 = gpioDrv15->PowerControl(BUTTON, ARM_POWER_FULL);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("ERROR: Failed to powered full\n");
        goto error_uninitialize;
    }

    ret1 = gpioDrv12->SetDirection(LED0_R, GPIO_PIN_DIRECTION_OUTPUT);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("ERROR: Failed to configure\n");
        goto error_power_off;
    }
    ret1 = gpioDrv15->SetDirection(BUTTON, GPIO_PIN_DIRECTION_INPUT);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("ERROR: Failed to configure\n");
        goto error_power_off;
    }

    /* Configure button GPIO IRQ */
    uint32_t button_irq_settings = ARM_GPIO_IRQ_POLARITY_LOW + ARM_GPIO_IRQ_SENSITIVE_EDGE;
    ret1 = gpioDrv15->Control(BUTTON, ARM_GPIO_ENABLE_INTERRUPT, &button_irq_settings);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("ERROR: Failed to configure interrupt\n");
        goto error_power_off;
    }

    /* Toggle LED ON and start waiting for button */
    ret1 = gpioDrv12->SetValue(LED0_R, GPIO_PIN_OUTPUT_STATE_HIGH);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("ERROR: Failed to toggle LEDs\n");
        goto error_power_off;
    }

    while (1)
    {
        UINT status;
        ULONG events_button = 0;
        UINT button_state = 0;

        /* wait for button press event */
        status = tx_event_flags_get(&event_flags_button, BUTTON_EVENT, TX_OR_CLEAR, &events_button, TX_WAIT_FOREVER);

        /* Toggle Red LED */
        ret1 = gpioDrv12->SetValue(LED0_R, GPIO_PIN_OUTPUT_STATE_TOGGLE);
        if ((ret1 != ARM_DRIVER_OK)) {
            printf("ERROR: Failed to toggle LEDs\n");
            goto error_power_off;
        }
    }

error_power_off:

    ret1 = gpioDrv12->PowerControl(LED0_R, ARM_POWER_OFF);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("ERROR: Failed to power off \n");
    } else {
        printf("LEDs power off \n");
    }
    ret1 = gpioDrv15->PowerControl(BUTTON, ARM_POWER_OFF);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("ERROR: Failed to power off \n");
    } else {
        printf("Button power off \n");
    }

error_uninitialize:

    ret1 = gpioDrv12->Uninitialize(LED0_R);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("Failed to Un-initialize \n");
    } else {
        printf("Un-initialized \n");
    }
    ret1 = gpioDrv15->Uninitialize(BUTTON);
    if ((ret1 != ARM_DRIVER_OK)) {
        printf("Failed to Un-initialize \n");
    } else {
        printf("Un-initialized \n");
    }
}

/* Define main entry point.  */
int main ()
{
    #if defined(RTE_Compiler_IO_STDOUT_User)
    int32_t ret;
    ret = stdout_init();
    if(ret != ARM_DRIVER_OK)
    {
        while(1)
        {
        }
    }
    #endif

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

/* Define what the initial system looks like.  */
void tx_application_define (void *first_unused_memory)
{
    UINT ret;

    /* Create the event flags group used by button press */
    ret = tx_event_flags_create(&event_flags_button, "event flags UART");
    if (ret != TX_SUCCESS)
    {
        printf("Could not create event flags\n");
        return;
    }

    /* Create the main thread.  */
    ret = tx_thread_create (&led_thread, "LED BLINK DEMO", led_blink_app, 0,
            first_unused_memory, LED_BLINK_THREAD_STACK_SIZE, 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    if (ret != TX_SUCCESS) {
        printf("failed to create led blink demo thread\r\n");
    }
}
