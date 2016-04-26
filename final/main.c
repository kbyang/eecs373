#include <stdio.h>
#include <inttypes.h>
#include "drivers/mss_uart/mss_uart.h"
#include "drivers/mss_spi/drivers/mss_spi/mss_spi.h"
#include "drivers/mss_gpio/mss_gpio.h"

#include "mytimer.h"

//Declare some global vars to keep track of score
unsigned PLAYER0_SCORE = 0;
unsigned PLAYER1_SCORE = 50;
unsigned ACCEPT_SECOND_HIT = 0;
uint32_t LATENCY = 1 << 20;

unsigned BLUE = 0;
unsigned RED = 1;
unsigned GREEN = 2;
unsigned YELLOW = 3;
unsigned PURPLE = 4;
unsigned CYAN = 5;

unsigned PLAYER0_COLOR = 0;
unsigned PLAYER1_COLOR = 1;

//FABRIC TIMER INTERRUPT HANDLER
__attribute__ ((interrupt)) void Fabric_IRQHandler( void )
{
    //uint32_t time = MYTIMER_getCounterVal();
    uint32_t status = MYTIMER_getInterrupt_status();

    MYTIMER_disable();

    if(status & 0x01) { //overflow interrupt
        //printf("Overflow interrupt triggered. \n\r");

        //stop accepting second interrupt
        ACCEPT_SECOND_HIT = 0;
        //printf("ACCEPT_SECOND_HIT set to false \n\r");
    }
    if(status & 0x02) {} //compare interrupt

    NVIC_ClearPendingIRQ( Fabric_IRQn );
}

void GPIO0_IRQHandler( void ) {
	PLAYER0_COLOR = PLAYER0_COLOR + 1;
	if (PLAYER0_COLOR == 6) {
		PLAYER0_COLOR = 0;
	}
	printf("Updating player 1 color\n\r");

	MSS_GPIO_clear_irq( MSS_GPIO_0 );
}

void GPIO1_IRQHandler( void ) {
	PLAYER1_COLOR = PLAYER1_COLOR + 1;
	if (PLAYER1_COLOR == 6) {
		PLAYER1_COLOR = 0;
	}
	printf("Updating player 2 color\n\r");

	MSS_GPIO_clear_irq( MSS_GPIO_1 );
}

void GPIO2_IRQHandler( void ) {
	PLAYER0_SCORE = 0;
	PLAYER1_SCORE = 0;

	printf("Resetting scores\n\r");

	MSS_GPIO_clear_irq( MSS_GPIO_2 );
}


//PRINTS NUMBER IN BINARY
void printbin(uint32_t n, unsigned size) {
	int i;
    for (i = size - 1; i >= 0; i = i - 1) {
    	unsigned cmp = (n >> i) & 0x1;
    	if (cmp){
    		printf("1");
    	} else {
    		printf("0");
    	}
    }
    printf("\n\r");
}

//RETURNS player ID from message
uint8_t parseID(uint8_t msg) {
	uint8_t ret = (msg >> 5) & 0x1;
	printf(">Player ID is: ");
	printbin(ret, 1);

	return ret;
}
uint8_t parseAddress(uint8_t msg) {
	uint8_t ret = (msg >> 6) & 0x3;
	printf(">Message address is: ");
	printbin(ret, 1);

	return ret;
}
uint8_t parseArmor(uint8_t msg) {
	uint8_t ret = (msg >> 4) & 0x1;

	if (ret) { //1 for armor
		printf(">Armor hit detected \n\r");
	} else { //0 for lightsaber
		printf(">Lightsaber hit detected \n\r");
	}

	return ret;
}
uint8_t parsePad(uint8_t msg) {
	uint8_t ret = msg & 0xF;
	printf(">Pad ID is: ");
	printbin(ret, 1);

	return ret;
}


//handles UART communication for sound to coocox
#define LIGHTSABER_SOUND 0
#define PAD_SOUND 1

void sendSound(uint32_t soundtype, uint8_t player_id) {
	uint8_t message[1] = {0};

	if (soundtype == LIGHTSABER_SOUND) {
		if (player_id) {
			message[0] = 0b01100000;
		} else {
			message[0] = 0b01000000;
		}
	}

	if (soundtype == PAD_SOUND) {
		if (player_id) {
			message[0] = 0b01100001;
		} else {
			message[0] = 0b01000001;
		}
	}

	MSS_UART_polled_tx( &g_mss_uart1, message, sizeof(message) );
}

//START MAIN
int main() {
	printf("Program init \n\r");

	//initialize UART
	printf("UART1 init \n\r");
	MSS_UART_init( &g_mss_uart1, MSS_UART_9600_BAUD,
	MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT );
	uint8_t rx_buff[1];

	//initialize SPI
	const uint8_t frame_size = 16;
	uint16_t master_tx_frame = 0x0000;

	MSS_SPI_init( &g_mss_spi1 );
	MSS_SPI_configure_master_mode
	(
		&g_mss_spi1,
		MSS_SPI_SLAVE_0,
		MSS_SPI_MODE1,
		MSS_SPI_PCLK_DIV_256,
		frame_size
	);

	//initialize GPIO interrupts
	MSS_GPIO_config( MSS_GPIO_0, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_NEGATIVE );
	MSS_GPIO_config( MSS_GPIO_1, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_NEGATIVE );
	MSS_GPIO_config( MSS_GPIO_2, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_NEGATIVE );
	MSS_GPIO_enable_irq( MSS_GPIO_0 );
	MSS_GPIO_enable_irq( MSS_GPIO_1 );
	MSS_GPIO_enable_irq( MSS_GPIO_2 );


	//Begin polling for incoming data
	printf("UART1 (xbee) polling for data \n\r");
	while( 1 ) {

		if (PLAYER0_SCORE == 99) { //update this to do some victory condition

		}
		if (PLAYER1_SCORE == 99) { //update this to do some victory condition

		}

		/*
		 * START CODE TO UPDATE DISPLAY
		 */

		//send data for player 0 over spi
		MSS_SPI_set_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );

		master_tx_frame = ((uint32_t) PLAYER0_SCORE);
		master_tx_frame = master_tx_frame << 8;
		master_tx_frame |= (uint32_t) PLAYER0_COLOR;

		printf(">Sending data: \n\r");
		printbin(master_tx_frame, 16);
		MSS_SPI_transfer_frame( &g_mss_spi1, master_tx_frame );

		//send data for player 1 over spi
		master_tx_frame = (uint32_t) PLAYER1_SCORE;
		master_tx_frame = master_tx_frame << 8;
		master_tx_frame |= (uint32_t) PLAYER1_COLOR;

		master_tx_frame |= 0x8000; //update bit 15 to signal player 1 score
		printf(">Sending data: \n\r");
		printbin(master_tx_frame, 16);
		MSS_SPI_transfer_frame( &g_mss_spi1, master_tx_frame );

		MSS_SPI_clear_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );

		/*
		 * END CODE TO UPDATE DISPLAY
		 */

		/*
		 * START CODE TO RECEIVE HITS
		 */
		size_t rx_size = MSS_UART_get_rx( &g_mss_uart1, rx_buff, sizeof(rx_buff) );

		if (rx_size > 0) { //on message received
			uint8_t msg = rx_buff[0];
			printf(">Received data: %c. Parsing... \n\r", msg);
			printbin(msg, 8);
			//Parse incoming data, see documentation for bit encoding
			//Parse message address
			uint8_t msg_address = parseAddress(msg);

			if (msg_address != 0b00) {
				printf("#Wrong address, ignore and continue \n\r");
				continue;
			}

			//Parse out player ID
			uint8_t msg_id = parseID(msg);
			//Parse out lightsaber or armor hit
			uint8_t msg_armor = parseArmor(msg);


			//Start timer to receive other hit
			MYTIMER_init();
			MYTIMER_setOverflowVal(LATENCY);

			//enable overflow interrupt
			MYTIMER_enable_overflowInt();
			MYTIMER_enable_allInterrupts();
			NVIC_EnableIRQ(Fabric_IRQn);

			ACCEPT_SECOND_HIT = 1;
			//printf("ACCEPT_SECOND_HIT set to true \n\r");
			MYTIMER_enable(); //on overflow, ACCEPT_SECOND_HIT=0

			while (ACCEPT_SECOND_HIT) {
				size_t rx_size = MSS_UART_get_rx( &g_mss_uart1, rx_buff, sizeof(rx_buff) );

				if (rx_size > 0) { //if we receive another XBEE communication within the correct latency
					printf(">###Second communication detected within latency! Parsing... \n\r");
					uint8_t second_msg = rx_buff[0];
					printbin(second_msg, 8);

					uint8_t second_msg_address = parseAddress(second_msg);
					if (second_msg_address != 0b00) {
						printf("#Wrong address, ignore and continue \n\r");
						continue;
					}

					uint8_t second_msg_id = parseID(second_msg);
					uint8_t second_msg_armor = parseArmor(second_msg);

					if ((msg_id && !second_msg_id) || (!msg_id && second_msg_id)) { //player ids are different between the two messages
						if ((msg_armor && !second_msg_armor) || (!msg_armor && second_msg_armor) || (!msg_armor && !second_msg_armor)) { //one player has a pad hit, the other has a ls hit, or both ls
							uint8_t pad_id;
							if (msg_armor) { //msg 1 pad got hit
								pad_id = parsePad(msg);
							} else { //msg 2 pad got hit
								pad_id = parsePad(second_msg);
							}
							printf("Pad ID hit is: %u\n\r", pad_id);

							int addend = 1;
							if(pad_id == 0) { addend = 5; }
							if(pad_id == 1) { addend = 4; }
							if(pad_id == 2) { addend = 3; }
							if(pad_id == 3) { addend = 2; }
							if(pad_id == 4) { addend = 1; }

							if (!msg_armor && !second_msg_armor) { //both lightsaber hits
								sendSound(LIGHTSABER_SOUND, msg_id); //msg_id doesn't matter, both lightsabers should make noise

							} else { //not both lightsaber hits, update score
								if (msg_id && msg_armor) { //if message 1 id is player 1 and it is an armor hit
									if (PLAYER0_SCORE < 99) {
										PLAYER0_SCORE = PLAYER0_SCORE + addend;
									}
									sendSound(PAD_SOUND, 1);

								} else { //player 0 pad got hit
									if (PLAYER1_SCORE < 99) {
										PLAYER1_SCORE = PLAYER1_SCORE + addend;
									}
									sendSound(PAD_SOUND, 0);
								}
								printf("Player 0 score is now: %u\n\r", PLAYER0_SCORE);
								printf("Player 1 score is now: %u\n\r", PLAYER1_SCORE);

							}

						}
					}
				}
			}
		}
		/*
		 * END CODE TO RECEIVE HITS
		 */


	}

	return(0);
}
