/*
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 * Modified by Federico Ruiz Ugalde <memeruiz@gmail.com> ARCOS-Lab
 * Universidad de Costa Rica.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This file implements a the USB Communications Device Class - Abstract
 * Control Model (CDC-ACM) as defined in CDC PSTN subclass 1.2.
 *
 * The device's unique id is used as the USB serial number string.
 */

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/cm3/nvic.h>
#include <stdlib.h>
#include <libopencm3-plus/utils/misc.h>
#include <libopencm3-plus/stm32f4discovery/leds.h>
#include <libopencm3-plus/utils/data_structs.h>
#include <libopencm3-plus/cdcacm_one_serial/cdcacm_common.h>
#include <libopencm3-plus/utils/common.h>

#define CDCACM_PACKET_SIZE 	64
#define CDCACM_READ_BUF_SIZE CDCACM_PACKET_SIZE*4

usbd_device * usbdev;

cbuf_t cdc_cbuf_in;

static int configured=0;

static const struct usb_endpoint_descriptor comm_endp[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x83,
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
	.wMaxPacketSize = 16,
	.bInterval = 255,
}};

char serial_no[9];

static const char *usb_strings[] = {
	"ARCOS-lab UCR",
	"CDC-ACM UART Interface",
	serial_no,
};

static const struct usb_endpoint_descriptor data_endp[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x01,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = CDCACM_PACKET_SIZE,
	.bInterval = 1,
}, {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x82,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = CDCACM_PACKET_SIZE,
	.bInterval = 1,
}};

static const struct {
	struct usb_cdc_header_descriptor header;
	struct usb_cdc_call_management_descriptor call_mgmt;
	struct usb_cdc_acm_descriptor acm;
	struct usb_cdc_union_descriptor cdc_union;
} __attribute__((packed)) cdcacm_functional_descriptors = {
	.header = {
		.bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_HEADER,
		.bcdCDC = 0x0110,
	},
	.call_mgmt = {
		.bFunctionLength =
			sizeof(struct usb_cdc_call_management_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
		.bmCapabilities = 0,
		.bDataInterface = 1,
	},
	.acm = {
		.bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_ACM,
		.bmCapabilities = 2,  /* SET_LINE_CODING supported */
	},
	.cdc_union = {
		.bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_UNION,
		.bControlInterface = 0,
		.bSubordinateInterface0 = 1,
	 }
};

static const struct usb_interface_descriptor comm_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_CDC,
	.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
	.bInterfaceProtocol = USB_CDC_PROTOCOL_AT,
	.iInterface = 0,

	.endpoint = comm_endp,

	.extra = &cdcacm_functional_descriptors,
	.extralen = sizeof(cdcacm_functional_descriptors)
}};

static const struct usb_interface_descriptor data_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 1,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_DATA,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = data_endp,
}};

static const struct usb_interface ifaces[] = {{
	.num_altsetting = 1,
	.altsetting = comm_iface,
}, {
	.num_altsetting = 1,
	.altsetting = data_iface,
}};

static const struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = USB_CLASS_CDC,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x0483,
	.idProduct = 0x5740,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

static const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 2,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0x80,
	.bMaxPower = 0x32,

	.interface = ifaces,
};

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];

static int cdcacm_control_request(usbd_device *usbd_dev, 
				  struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
				  void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;
	(void)len;

	switch (req->bRequest) {
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
		/*
		 * This Linux cdc_acm driver requires this to be implemented
		 * even though it's optional in the CDC spec, and we don't
		 * advertise it in the ACM functional descriptor.
		 */
		return 1;
		}
	case USB_CDC_REQ_SET_LINE_CODING:
		if (*len < sizeof(struct usb_cdc_line_coding))
			return 0;
		return 1;
	}
	return 0;
}

static void cdcacm_callback_in(usbd_device *usbd_dev, uint8_t ep)
{
  (void)ep;
  static char buf[CDCACM_PACKET_SIZE];
  int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, CDCACM_PACKET_SIZE);
  if (len > 0) {
    cbuf_append(&cdc_cbuf_in, buf,  len);
  }
}

int cdcacm_sent=true;

static void cdcacm_callback_out(NOT_USED usbd_device* usbd_dev,NOT_USED uint8_t ep)
{
  //printled(1, LORANGE);
  cdcacm_sent=true;
}

int cdcacm_get_config(void)
{
	return configured;
}

static void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	configured = wValue;
	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, CDCACM_PACKET_SIZE, cdcacm_callback_in);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, CDCACM_PACKET_SIZE, cdcacm_callback_out);
	usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);
	usbd_register_control_callback(
				usbd_dev,
				USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				cdcacm_control_request);
	/* Notify the host that DCD is asserted.
	 * Allows the use of /dev/tty* devices on *BSD/MacOS
	 */
	char buf[10];
	struct usb_cdc_notification *notif = (void*)buf;
	/* We echo signals back to host as notification */
	notif->bmRequestType = 0xA1;
	notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
	notif->wValue = 0;
	notif->wIndex = 0;
	notif->wLength = 2;
	buf[8] = 3; /* DCD | DSR */
	buf[9] = 0;
	usbd_ep_write_packet(usbd_dev, 0x83, buf, 10);
}

static char *get_dev_unique_id(char *s)
{
#if defined(STM32F4)
  volatile uint32_t *unique_id_p = (volatile uint32_t *)0x1FFF7A10;
#elif defined(STM32F3)
  volatile uint32_t *unique_id_p = (volatile uint32_t *)0x1FFFF7AC;
#else
  volatile uint32_t *unique_id_p = (volatile uint32_t *)0x1FFFF7E8;
#endif
  uint32_t unique_id = *unique_id_p +
                       *(unique_id_p + 1) +
                       *(unique_id_p + 2);
  int i;

  /* Fetch serial number from chip's unique ID */
  for(i = 0; i < 8; i++) {
    s[7-i] = ((unique_id >> (4*i)) & 0xF) + '0';
  }
  for(i = 0; i < 8; i++)
    if(s[i] > '9')
      s[i] += 'A' - '9' - 1;
  s[8] = 0;
  return s;
}

int cdcacm_open(NOT_USED const char *path, NOT_USED int flags, NOT_USED int mode) {
  return(0);
}

int cdcacm_close(NOT_USED int fd) {
  return(0);
}

void cdcacm_write_now(char* buf, int len) {
  //printled(1, LGREEN);
  //while (cdcacm_sent == false) {};
  //nvic_disable_irq(NVIC_OTG_FS_IRQ);
  //cdcacm_sent=false;
  //__asm__("CPSID i;");
  while (usbd_ep_write_packet(usbdev, 0x82, buf, len) ==0);
  //cdcacm_sent=false;
  //nvic_enable_irq(NVIC_OTG_FS_IRQ);
  //__asm__("CPSIE i;");
  //printled(2, LGREEN);
}

long cdcacm_write(NOT_USED int fd, const char *ptr, int len) {
  //printled2(1, 10, LGREEN);
  int index;
  static char buf[CDCACM_PACKET_SIZE];
  static int buf_pos=0;
  /* For example, output string by UART */
  for(index=0; index<len; index++)
    {
      buf[buf_pos]=ptr[index];
      buf_pos+=1;
      if (buf_pos == 1) {
	//if (buf_pos == CDCACM_PACKET_SIZE/2) {
	cdcacm_write_now(buf, buf_pos);
	buf_pos=0;
      }	
      if (ptr[index] == '\n')
	{
	  buf[buf_pos]='\r';
	  buf_pos+=1;
	  cdcacm_write_now(buf, buf_pos);
	  buf_pos=0;
	}
    }

  return len;
}

long cdcacm_read(NOT_USED int fd, char *ptr, int len) {
  //printf("read len %d\n", len);
  while (cbuf_used(&cdc_cbuf_in) < len) {
  };
  return(cbuf_pop(&cdc_cbuf_in, ptr, len));
}

void cdcacm_reset(void);
void cdcacm_reset(void) {
}

int cdcacm_in_poll(int fd) {
  if (fd == 0) { //std
    return(cbuf_used(&cdc_cbuf_in));
  } else {
    return(-1);
  }
}

void cdcacm_usb_init(void) {

  //receive buffer setup
  if (cbuf_init(&cdc_cbuf_in, CDCACM_READ_BUF_SIZE) != 0) { //couldn't initialize buffer for usb
    while(1) {
      printled(5, LRED);
    }
  }
  //usb setup
  get_dev_unique_id(serial_no);
#ifdef STM32F4
  usbdev = usbd_init(&otgfs_usb_driver, &dev, &config, usb_strings, sizeof(usb_strings)/sizeof(char *), usbd_control_buffer, sizeof(usbd_control_buffer));
#else
  usbdev = usbd_init(&st_usbfs_v1_usb_driver, &dev, &config, usb_strings, sizeof(usb_strings)/sizeof(char *), usbd_control_buffer, sizeof(usbd_control_buffer));
#endif
  usbd_register_set_config_callback(usbdev, cdcacm_set_config);
  usbd_register_reset_callback(usbdev, cdcacm_reset);
}

void cdcacm_poll(void) {
  usbd_poll(usbdev);
}


