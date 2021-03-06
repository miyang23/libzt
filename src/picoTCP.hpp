/*
 * ZeroTier SDK - Network Virtualization Everywhere
 * Copyright (C) 2011-2017  ZeroTier, Inc.  https://www.zerotier.com/
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
 *
 * --
 *
 * You can be released from the requirements of the license by purchasing
 * a commercial license. Buying such a license is mandatory as soon as you
 * develop commercial closed-source software that incorporates or links
 * directly against ZeroTier software without disclosing the source code
 * of your own application.
 */

#ifndef ZT_PICOTCP_HPP
#define ZT_PICOTCP_HPP

#include "pico_eth.h"
#include "pico_stack.h"
#include "pico_ipv4.h"
#include "pico_icmp4.h"
#include "pico_dev_tap.h"
#include "pico_protocol.h"
#include "pico_socket.h"
#include "pico_device.h"
#include "pico_ipv6.h"

#include "SocketTap.hpp"

/****************************************************************************/
/* PicoTCP API Signatures (See libzt.h for the API an app should use)       */
/****************************************************************************/

#define PICO_IPV4_TO_STRING_SIG char *ipbuf, const uint32_t ip
#define PICO_TAP_CREATE_SIG char *name
#define PICO_IPV4_LINK_ADD_SIG struct pico_device *dev, struct pico_ip4 address, struct pico_ip4 netmask
#define PICO_DEVICE_INIT_SIG struct pico_device *dev, const char *name, uint8_t *mac
#define PICO_STACK_RECV_SIG struct pico_device *dev, uint8_t *buffer, uint32_t len
#define PICO_ICMP4_PING_SIG char *dst, int count, int interval, int timeout, int size, void (*cb)(struct pico_icmp4_stats *)
#define PICO_TIMER_ADD_SIG pico_time expire, void (*timer)(pico_time, void *), void *arg
#define PICO_STRING_TO_IPV4_SIG const char *ipstr, uint32_t *ip
#define PICO_STRING_TO_IPV6_SIG const char *ipstr, uint8_t *ip
#define PICO_SOCKET_SETOPTION_SIG struct pico_socket *s, int option, void *value
#define PICO_SOCKET_SEND_SIG struct pico_socket *s, const void *buf, int len
#define PICO_SOCKET_SENDTO_SIG struct pico_socket *s, const void *buf, int len, void *dst, uint16_t remote_port
#define PICO_SOCKET_RECV_SIG struct pico_socket *s, void *buf, int len
#define PICO_SOCKET_RECVFROM_SIG struct pico_socket *s, void *buf, int len, void *orig, uint16_t *remote_port
#define PICO_SOCKET_OPEN_SIG uint16_t net, uint16_t proto, void (*wakeup)(uint16_t ev, struct pico_socket *s)
#define PICO_SOCKET_BIND_SIG struct pico_socket *s, void *local_addr, uint16_t *port
#define PICO_SOCKET_CONNECT_SIG struct pico_socket *s, const void *srv_addr, uint16_t remote_port
#define PICO_SOCKET_LISTEN_SIG struct pico_socket *s, const int backlog
#define PICO_SOCKET_READ_SIG struct pico_socket *s, void *buf, int len
#define PICO_SOCKET_WRITE_SIG struct pico_socket *s, const void *buf, int len
#define PICO_SOCKET_CLOSE_SIG struct pico_socket *s
#define PICO_SOCKET_SHUTDOWN_SIG struct pico_socket *s, int mode
#define PICO_SOCKET_ACCEPT_SIG struct pico_socket *s, void *orig, uint16_t *port
#define PICO_IPV6_LINK_ADD_SIG struct pico_device *dev, struct pico_ip6 address, struct pico_ip6 netmask

namespace ZeroTier
{
	/*
	 * Send raw frames from the stack to the ZeroTier virtual wire
	 */
	int pico_eth_send(struct pico_device *dev, void *buf, int len);

	/*
	 * Read raw frames from RX frame buffer into the stack 
	 */
	int pico_eth_poll(struct pico_device *dev, int loop_score);

	class SocketTap;
	struct Connection;

	class picoTCP
	{		
	public:

		/*
		 * Set up an interface in the network stack for the SocketTap
		 */
		bool pico_init_interface(ZeroTier::SocketTap *tap, const ZeroTier::InetAddress &ip);

		/*
		 * Main stack loop
		 */
		void pico_loop(SocketTap *tap);

		/*
		 * Read bytes from the stack to the RX buffer (prepare to be read by app)
		 */
		static void pico_cb_tcp_read(SocketTap *tap, struct pico_socket *s);

		/*
		 * Read bytes from the stack to the RX buffer (prepare to be read by app)
		 */
		static void pico_cb_udp_read(SocketTap *tap, struct pico_socket *s);

		 /*
		 * Write bytes from TX buffer to stack (prepare to be sent to ZT virtual wire)
		 */
		static void pico_cb_tcp_write(SocketTap *tap, struct pico_socket *s);

		/*
		 * Write bytes from TX buffer to stack (prepare to be sent to ZT virtual wire)
		 */
		static void pico_cb_socket_activity(uint16_t ev, struct pico_socket *s);

		/*
		 * Packets from the ZeroTier virtual wire enter the stack here
		 */
		void pico_rx(SocketTap *tap, const ZeroTier::MAC &from,const ZeroTier::MAC &to,unsigned int etherType,const void *data,unsigned int len);
		
		/*
		 * Creates a stack-specific "socket" or "connection object"
		 */
		int pico_Socket(struct pico_socket **p, int socket_family, int socket_type, int protocol);

		/*
		 * Connect to remote host via userspace network stack interface - Called from SocketTap
		 */
		int pico_Connect(Connection *conn, int fd, const struct sockaddr *addr, socklen_t addrlen);
		
		/*
		 * Bind to a userspace network stack interface - Called from SocketTap
		 */
		int pico_Bind(Connection *conn, int fd, const struct sockaddr *addr, socklen_t addrlen);
		
		/*
		 * Listen for incoming connections - Called from SocketTap
		 */
		int pico_Listen(Connection *conn, int fd, int backlog);

		/*
		 * Accept an incoming connection - Called from SocketTap
		 */
		Connection* pico_Accept(Connection *conn);

		/*
		 * Read from RX buffer to application - Called from SocketTap
		 */
		int pico_Read(SocketTap *tap, ZeroTier::PhySocket *sock,Connection *conn,bool stack_invoked);

		/*
		 * Write to userspace network stack - Called from SocketTap
		 */
		int pico_Write(Connection *conn, void *data, ssize_t len);

		/*
		 * Close a Connection - Called from SocketTap
		 */
		int pico_Close(Connection *conn);

		/*
		 * Converts picoTCP error codes to pretty string
		 */
		static char *beautify_pico_error(int err);

		/*
		 * Converts picoTCP socket states into pretty string
		 */
		static char *beautify_pico_state(int state);
	};
}

#endif