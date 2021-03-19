/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "params.h"
#include "queue.h"

void vGreenTask()
{
    for(;;){
        vTaskDelay(pdMS_TO_TICKS(500));
        Cy_GPIO_Write(GREENLED_0_PORT,GREENLED_0_NUM,0);
        vTaskDelay(pdMS_TO_TICKS(500));
        Cy_GPIO_Write(GREENLED_0_PORT,GREENLED_0_NUM,1);
    }
}
SemaphoreHandle_t bouton_semph;
int i=0;
void isr_bouton(void)
{   
    i++;
    xSemaphoreGiveFromISR(bouton_semph,NULL);
    Cy_GPIO_ClearInterrupt(BOUTON_0_PORT,BOUTON_0_NUM);
    NVIC_ClearPendingIRQ(Bouton_ISR_cfg.intrSrc);    
}
void bouton_task(){
    for(;;){
        xSemaphoreTake(bouton_semph,portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(20));
        if (i%2 !=0){
        UART_PutString("Bouton appuye");
        }
        else{
        UART_PutString("Bouton relache");
        }
        
    } 
    xSemaphoreGive(bouton_semph);
}


int main(void)
{
    bouton_semph = xSemaphoreCreateBinary();
    __enable_irq(); /* Enable global interrupts. */
    
    UART_Start();
    Cy_SysInt_Init(&Bouton_ISR_cfg, isr_bouton);
    NVIC_EnableIRQ(Bouton_ISR_cfg.intrSrc);
    NVIC_ClearPendingIRQ(Bouton_ISR_cfg.intrSrc);
    

    xTaskCreate(vGreenTask,"green",80,NULL,3,NULL);
    xTaskCreate(bouton_task,"Etat",80,NULL,3,NULL);
    vTaskStartScheduler();
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
