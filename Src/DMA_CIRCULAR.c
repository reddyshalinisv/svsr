/*  To enable circular buffer, you have to enable IDLE LINE DETECTION interrupt 

__HAL_UART_ENABLE_ITUART_HandleTypeDef *huart, UART_IT_IDLE);   // enable idle line interrupt
__HAL_DMA_ENABLE_IT (DMA_HandleTypeDef *hdma, DMA_IT_TC);  // enable DMA Tx cplt interrupt

also enable RECEIVE DMA

HAL_UART_Receive_DMA (UART_HandleTypeDef *huart, DMA_RX_Buffer, 64);


IF you want to transmit the received data uncomment lines 91 and 101


PUT THE FOLLOWING IN THE MAIN.c

#define DMA_RX_BUFFER_SIZE          64
uint8_t DMA_RX_Buffer[DMA_RX_BUFFER_SIZE];

#define UART_BUFFER_SIZE            256
uint8_t UART_Buffer[UART_BUFFER_SIZE];


*/

#include "DMA_CIRCULAR.h"
#include "string.h"

#define DMA_RX_BUFFER_SIZE         64
extern uint8_t DMA_RX_Buffer[DMA_RX_BUFFER_SIZE];

#define UART_BUFFER_SIZE        256
extern uint8_t UART_Buffer[UART_BUFFER_SIZE];

#define temp_buffer_size   64
extern uint8_t buffer[temp_buffer_size];
//static int o;
size_t Write;
size_t len, tocopy;
uint8_t* ptr;
//uint8_t* ptr2;
extern uint16_t number;
////static uint8_t i;
//int cnt=0;
uint8_t k;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern UART_HandleTypeDef huart2;

//uint16_t esp_waitfor (char *string) 
//{
//  uint8_t so_far = 0;
//  char received;
//  uint16_t counter = 0;
//  for (int i=Write; i<len; i++)
//	{
//				if (UART_Buffer[i] == string[so_far])
//				{
//					counter=i;
//					so_far++;
//				}
//				else 
//					so_far = 0;
//	}
//  
//	return counter;
//}

void USART_IrqHandler (UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma)
{
//HAL_UART_Transmit(&huart2,"UART_IRQ_INN\r\n",14,1000);
	if (huart->Instance->SR & UART_FLAG_IDLE)           /* if Idle flag is set */
	{
		volatile uint32_t tmp;                  /* Must be volatile to prevent optimizations */
        tmp = huart->Instance->SR;                       /* Read status register */
        tmp = huart->Instance->DR;                       /* Read data register */
		hdma->Instance->CR &= ~DMA_SxCR_EN;       /* Disabling DMA will force transfer complete interrupt if enabled */       
	}
           
      
    
}

void DMA_IrqHandler (DMA_HandleTypeDef *hdma)
{
 
//HAL_UART_Transmit(&huart2,"DMA_IRQ_IN\r\n",12,1000);
	typedef struct
	{
		__IO uint32_t ISR;   /*!< DMA interrupt status register */
		__IO uint32_t Reserved0;
		__IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
	} DMA_Base_Registers;

	DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;
	
	if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TC) != RESET)   // if the source is TC
	{
		/* Clear the transfer complete flag */
      regs->IFCR = DMA_FLAG_TCIF1_5 << hdma->StreamIndex;
	  
	     /* Get the length of the data */
	  len = DMA_RX_BUFFER_SIZE - hdma->Instance->NDTR;  
	  
	  /* Get number of bytes we can copy to the end of buffer */
	  tocopy = UART_BUFFER_SIZE - Write;      
	  
	  /* Check how many bytes to copy */
       if (tocopy > len) 
				{
            tocopy = len;
        }
		
		 /* Write received data for UART main buffer for manipulation later */
        ptr = DMA_RX_Buffer;
        memcpy(&UART_Buffer[Write], ptr, tocopy);   /* Copy first part */
        
	//memcpy(&buffer,ptr,tocopy);
       //call_store_buffer( tocopy,Write);
//				number = esp_waitfor ("OK");
				
		/* Correct values for remaining data */
        Write += tocopy;
        len -= tocopy;
        ptr += tocopy;
	//call_store_buffer( tocopy,Write);	
		/* UNCOMMENT BELOW TO transmit the data via uart */
	 memcpy(&buffer,&UART_Buffer[Write-tocopy], tocopy);			
				

		HAL_UART_Transmit(&huart2, &UART_Buffer[Write-tocopy], tocopy, 10);
		
		/* If still data to write for beginning of buffer */
       if (len) 
				{
            memcpy(&UART_Buffer[0], ptr, len);      /* Don't care if we override Read pointer now */
            Write = len;
			
						/* UNCOMMENT BELOW TO transmit the data via uart */
					
//					number = esp_waitfor ("OK");
			
//						HAL_UART_Transmit(&huart2, UART_Buffer, len, 10);  // transmit the remaining data
         }
		
		/* Prepare DMA for next transfer */
        /* Important! DMA stream won't start if all flags are not cleared first */
 //HAL_UART_Transmit(&huart2,"DMA RESET_AGN\r\n",14,1000);
        regs->IFCR = 0x3FU << hdma->StreamIndex; // clear all interrupts
				hdma->Instance->M0AR = (uint32_t)DMA_RX_Buffer;   /* Set memory address for DMA again */
        hdma->Instance->NDTR = DMA_RX_BUFFER_SIZE;    /* Set number of bytes to receive */
        hdma->Instance->CR |= DMA_SxCR_EN;            /* Start DMA transfer */
	}
}
/*
void call_store_buffer(uint8_t val,uint8_t val2)
{
  
 
  for(i=val2;i<64;i++)
  {
    //if(cnt<val)
    {
    
    if(DMA_RX_Buffer[k]=='\r')
    {
      buffer[k]='\0';
      i=0;
      k=0;
    }
    else
    {
      
      buffer[k]=UART_Buffer[val2];
      k++;
    }
    }
    //cnt++;
}

}*/

