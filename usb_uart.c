#include <linux/console.h>
#include <linux/platform_device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <asm/irq.h>
#include <asm/io.h>

#define USB_UART_MAJOR 200 /*you have to get this assigned*/ 
#define USB_UART_MINOR_START 70 //start minor numbering here
#define USB_UART_PORTS 2 //the phone has 2 USB_UART
#define PORT_USB_UART 30 // UART type, add this to include/linux/serial_core.h

#define USB_UART1_BASE 0xe8000000 // memory base for USB_UART1
#define USB_UART2_BASE 0xe9000000 // memory base for USB_UART2

#define USB_UART_REGISTER_SPACE 0x03

#define USB_UART_TX_FULL 0x20 //TX FIFO is full
#define USB_UART_RX_EMPTY 0x10 // TX FIFO is empty
#define USB_UART_STATUS 0x0F // parity flame overruns

#define USB_UART1_IRQ  3//USB_UART1 IRQ
#define USB_UART@_IRQ  4//USB_UART2 IRQ

#define USB_UART_FIFO_SIZE 32 //FIFO size

#define USB_UART_CLK_FREQ 16000000

static struct uart_port usb_uart_port[]; //defined later on

//write a character to the port
static void usb_uart_putc(struct uart_port *port, unsigned char c){
	/*wait until there is space in the TX FIFO,check the USB_TX_FULL bit of
	sttus register*/
	while(__raw_readb(port->membase)&USB_UART_TX_FULL);
	//write the character to the data port
	__raw_writeb(c, (port->(membase+1));
}



// read a character from the port

static unsigned char usb_uart_getc(struct uart_port *port){
	//wait until data is available in the RX_FIFO
	while(__raw_readb(port->membase)&USB_UART_RX_EMPTY);

	//obtain the data
	return(__raw_readb(port->membase+2));
}


//obtain the port status

static unsigned char usb_uart_status(struct uart_port *port){
	return (__raw_readb(port->membase)&USB_UART_STATUS);
}



//claim the memory region attached to the port
static int usb_uart_request_port(struct uart_port *port){
	if(!request_mem_region(port->mapbase, USB_UART_REGISTER_SPACE,
		"usb_uart")){
		return -EBUSY;
	}
	return 0;
}	


//release the memory region atached to the port
static void usb_uart_release_port(struct uart_port *port){
	release_mem_region(port->mapbase, USB_UART_REGISTER_SPACE);
}


//configure the port
static void usb_uart_config_port(struct uart_port *port, int flags){
	if(flags &UART_CONFIG_TYPE && usb_uart_request_port(port) == 0){
		port->type=PORT_USB_UART;
	}

}


//receive interrupt handler
static irqreturn_t usb_uart_rxint(int irq, void *dev_id){
	struct uart_port *port = (struct uart_port *)dev_id;
	struct tty_struct *tty = port->info->tty;

	unsigned int status, data;

	do{
		//read data
		data = usb_uart_getc(port);
		//normal, overrun,parity,frame error
		status= usb_uart_status(port);
		// dispatch to the tty layer
		tty_insert_flip_char(tty, data, status);

	}while(more_chars_to_be_read());//more chars
	
	tty_flip_buffer_push(tty);

	return IRQ_HANDLED;
}


static int usb_uart_startup(struct uart_port *port){
	int retval = 0;
	// request IRQ
	if((retval = request_irq(port->irq, usb_uart_rxint, 0, "usb_uart",
		(void *)port))){
		return retval;
	}

	return retval;
}


//called when an application closes  a usb
static void usb_uart_shutdown(struct uart_port *port){
	//free IRQ
	free_irq(port->irq, port);
}



//set uart type
static const char* usb_uart_type(struct uart_port *port){
	return port->type == PORT_USB_UART ? "USB_UART" : NULL;
}

//start transmitting bytes
static void usb_uart_start_tx(struct uart_port *port){
	while(1){
		usb_uart_putc(port, port->info->xmit.buf[port->info->xmit.tail]);
		port->info->xmit.tail = (port->info->xmit.tail+1)&(UART_XMIT_SIZE-1);
		port->icount.tx++;

		if(uart_circ_empty(&port->info->xmit))
			break;
	}
}




//uart operations structure
static struct uart_ops usb_uart_ops = {
	.start_tx = usb_uart_start_tx,//start transmitting
	.startup = usb_uart_startup,//app opens USB_UART
	.shutdown = usb_uart_shutdown,//app closes USB_UART
	.type =usb_uart_type, //set Uart type
	.config_port = usb_uart_config_port, //configure when driver
	.request_port = usb_uart_request_port,//claim resources
	.release_port = usb_uart_release_port,//release resources

	#if 0
	#endif
}



static struct uart_driver usb_uart_reg = {
	.owner = THIS_MODULE, //owner
	.driver_name = "usb_uart", //driver name
	.dev_name = "ttyUU", //node name
	.major = USB_UART_MAJOR, //major number
	.minor = USB_UART_MINOR_START, //mior number start
	.nr = USB_UART_PORTS,//number of uart ports
	.cons = &usb_uart_console,//pointer to the console
};


//called when the platform driver is unregistered
static int usb_uart_remove(struct platform_device *dev){
	platform_set_drvdata(dev, NULL);
	uart_remove_one_port(&usb_uart_reg, &usb_uart_port[dev->id]);
	return 0;
}


//suspend power management event
static int usb_uart_suspend(struct platform_device *dev, pm_message_t state){
	uart_suspend_port(&usb_uart_reg, &usb_uart_port[dev->id]);
	return 0;
}


//resume after a previous suspend
static int usb_uart_resume(struct platform-device *dev){
	uart_resume_port(&usb_uart_reg, &usb_uart_port[dev->id]);
	return 0;
}


//parameters of each supported port
static struct uart_port usb_uart_port[] = {
	{	
	.mapbase = (unsigned int) USB_UART1_BASE,
	.iotype = UPIO_MEM, // memory mapped
	.irq = USB_UART1_IRQ,	// irq num
	.uartclk = USB_UART_CLK_FREQ, //clock hz
	.fifosize = USB_UART_FIFO_SIZE, //size of fifo
	.ops = &usb_uart_ops, // uart operations
	.flags = UPF_BOOT_AUTOCONF,
	.line= 0,	// port number
	}

	{
	.mapbase = (unsigned int) USB_UART2_BASE,
        .iotype = UPIO_MEM, // memory mapped
        .irq = USB_UART1_IRQ,   // irq num
        .uartclk = USB_UART_CLK_FREQ, //clock hz
        .fifosize = USB_UART_FIFO_SIZE, //size of fifo
        .ops = &usb_uart_ops, // uart operations
        .flags = UPF_BOOT_AUTOCONF,
        .line= 1,       // port number
	}
}




//platform driver probe
static int __init usb_uart_probe(struct platform_device *dev){
	//add a usb_uart port
	uart_add_one_port(&usb_uart_reg, &usb_uart_port[dev->id]);
	platform_set_drvdata(dev, &usb_uart_port[dev->id]);
	return 0;
}


struct platform_device *usb_uart_plat_device1; // platform device for uart 1
struct platform_device *usb_uart_plat_device2; // platform device for usrt 2

static struct platform_driver usb_uart_driver ={
	.probe = usb_uart_probe, // probe method
	.romve = __exit_p(usb_uart_remove), //detach method
	.suspend = usb_uart_suspend, //power suspend
	.resume = usb_uart_resume, //resume after a suspend
	.driver ={
		.name = "usb_uart", //driver name
	},
};


//driver initialization

static int __init usb_uart_init(void){
	int retval;
	//register the usb_uart driver with the serial core
	if((retval = uart_register_driver(&usb_uart_reg))){
		return retval;
	}
	//register platform device for usb_uart 1
	usb_uart_plat_device1=platform_device_register_simple("usb_uart", 0 , NULL, 0 );

	if(IS_ERR(usb_uart_plat_device1)){
		uart_unregister_driver(&usb_uart_reg);
		platform_device_unregister(usb_uart_plat_device1);
		return PTR_ERR(usb_uart_plat_device1);
	}

        //register platform device for usb_uart 1
        usb_uart_plat_device1=platform_device_register_simple("usb_uart", 1 , NULL, 0 );

        if(IS_ERR(usb_uart_plat_device2)){
                uart_unregister_driver(&usb_uart_reg);
		platform_device_unregister(usb_uart_plat_device1);
		platform_device-unregister(usb_uart_plat_device2);
                return PTR_ERR(usb_uart_plat_device1);
        }
	return 0;

}



//driver exit

static void __exit usb_uart-exit(void){
	platform_driver_unregister(&usb_uart_driver);

	platform_device_unregister(usb_uart_plat_device1);
	platform_device_unregister(usb_uart_plat_device2);

	uart_unregister_driver(&usb_uart-reg);

}



module_init(usb_uart_init);
module_exit(usb_uart_exit);


