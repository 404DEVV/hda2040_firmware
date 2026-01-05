#include <hardware/pio.h>
#include "hardware/gpio.h"
#include <hardware/dma.h>

#include "pio_hda_clk.pio.h"
#include "pio_hda_sync.pio.h"
#include "pio_hda_sdi_init.pio.h"

#define RST 3

uint32_t buf_sync[4096] = {0};
uint32_t buf_dt[4096] = {0};
volatile uint buf_sync_addr, buf_dt_addr;

PIO PIO_INST = pio0;
PIO PIO_INST_2 = pio1;
uint pio_sms[3];
uint pio_offsets[3];

void setup()
{
  //gpio for ledssss
  for(uint i = 8;i!=12;i++)
  {
    gpio_init(i);
    gpio_set_dir(i, true);
    gpio_put(i, false);
  }
  pinMode(2,OUTPUT);
  tone(2,666);
  gpio_init(13);gpio_set_dir(13,true);
  gpio_put(13,false);

  gpio_init(RST);
  gpio_set_dir(RST,true);
  gpio_put(RST,true);
  gpio_init(5);
  gpio_set_dir(5, false);
  buf_sync[31] = 0x000000FF;
  buf_sync[62] = 0x0000FF00;
  buf_sync[540] = 0x000000FF;
  buf_sync[550] = 0x000000FF;
  
  // pio_claim_free_sm_and_add_program(
  //   &hda_sdi_init_program,
  //   &PIO_INST,
  //   &pio_sms[2],
  //   &pio_offsets[2]
  // );
  // 
  pio_claim_free_sm_and_add_program(
    &hda_clk_program,
    &PIO_INST,
    &pio_sms[0],
    &pio_offsets[0]
  );

  hda_clk_program_init(PIO_INST, pio_sms[0], pio_offsets[0], 6, 17, 1.0);

  pio_claim_free_sm_and_add_program(
    &hda_sync_program,
    &PIO_INST,
    &pio_sms[1],
    &pio_offsets[1]
  );
  hda_sync_program_init(PIO_INST, pio_sms[1], pio_offsets[1], 6, 4, 1.0);

  pio_claim_free_sm_and_add_program(
    &hda_sdi_init_program,
    &PIO_INST,
    &pio_sms[2],
    &pio_offsets[2]
  );

hda_sdi_init_program_init(PIO_INST, pio_sms[2], pio_offsets[2], 4, 5, 1.0);
  pio_sm_set_enabled(PIO_INST, pio_sms[1], true);
  pio_sm_set_enabled(PIO_INST, pio_sms[0], true);
  pio_sm_put(PIO_INST, pio_sms[2], 0x0005);
  pio_sm_put(PIO_INST, pio_sms[2], 0x0000);
  gpio_put(8,1);
  delay(100);
  gpio_put(8,0);


  pio_sm_put_blocking(PIO_INST, pio_sms[0], 0);
  pio_sm_put_blocking(PIO_INST, pio_sms[1], 0);
  pio_sm_put_blocking(PIO_INST, pio_sms[0], 0);
  pio_sm_put_blocking(PIO_INST, pio_sms[1], 0);
  gpio_put(13,true);
  gpio_put(13,false);
  gpio_put(13,true);
  gpio_put(13,false);
  while(1)
  {
    if(!pio_sm_is_tx_fifo_full(PIO_INST, pio_sms[0]))
    {
      pio_sm_put(PIO_INST, pio_sms[0], 0);
      
      gpio_put(9,1);
    }
    else
    {
      gpio_put(9,0);
    }
    if(!pio_sm_is_tx_fifo_full(PIO_INST, pio_sms[1]))
    {

      pio_sm_put(PIO_INST, pio_sms[1], buf_sync[buf_sync_addr]);
      gpio_put(10,1);
      buf_sync_addr++;
      if(buf_sync_addr > 4095)
      {
        //buf_sync_addr = 0;
        while(1)
        {gpio_put(8, !gpio_get(8));delay(100);}
      }
    }
    else
    {
      gpio_put(10,0);
    }
    if(buf_sync_addr == 100) gpio_put(RST,false);
    if(buf_sync_addr == 101) 
    {
      delayMicroseconds(100);
    }
    
    //if(buf_sync_addr == 549) gpio_put(5, false);

  }
}
void setup1()
{
  while(1)
  {
if(buf_sync_addr ==540) gpio_put(RST,true);
    //if(buf_sync_addr == 550) gpio_put(5, true);
    if(buf_sync_addr == 590) {
      // pio_sm_set_consecutive_pindirs(PIO_INST, pio_sms[2], 5, 1, true);
      // pio_sm_set_enabled(PIO_INST, pio_sms[2], true);
    }
    //if(buf_sync_addr == 556) gpio_put(5, false);gpio_set_dir(5, false);
    // if(buf_sync_addr == 557)
  }
}
void loop()
{

}