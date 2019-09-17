void main() 
{ 
   setup_adc_ports(NO_ANALOGS); 
   enable_interrupts(INT_RA1_H2L); 
   enable_interrupts(GLOBAL); 

   while(1){ 
      if(flag){       
         output_toggle(LED); 
         delay_ms(500); 
         flag = 0; 
      } 
   } 
} 

#INT_RA 
void int_ioc() 
{ 
   if(!input(BUTTON)) 
      flag = 1; 
} 

///correccion

#byte IOCAF = getenv("SFR:IOCAF") 

#INT_RA 
void int_ioc() 
{ 
   bit_clear(IOCAF,1); //For RA1 
   if(!input(BUTTON)) 
      flag = 1; 
} 