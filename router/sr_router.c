/**********************************************************************
 * file:  sr_router.c
 * date:  Mon Feb 18 12:50:42 PST 2002
 * Contact: casado@stanford.edu
 *
 * Description:
 *
 * This file contains all the functions that interact directly
 * with the routing table, as well as the main entry method
 * for routing.
 *
 **********************************************************************/

#include <stdio.h>
#include <assert.h>


#include "sr_if.h"
#include "sr_rt.h"
#include "sr_router.h"
#include "sr_protocol.h"
#include "sr_arpcache.h"
#include "sr_utils.h"

/*---------------------------------------------------------------------
 * Method: sr_init(void)
 * Scope:  Global
 *
 * Initialize the routing subsystem
 *
 *---------------------------------------------------------------------*/

void sr_init(struct sr_instance* sr)
{
    /* REQUIRES */
    assert(sr);

    /* Initialize cache and cache cleanup thread */
    sr_arpcache_init(&(sr->cache));

    pthread_attr_init(&(sr->attr));
    pthread_attr_setdetachstate(&(sr->attr), PTHREAD_CREATE_JOINABLE);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_t thread;

    pthread_create(&thread, &(sr->attr), sr_arpcache_timeout, sr);
    
    /* Add initialization code here! */

} /* -- sr_init -- */

/*---------------------------------------------------------------------
 * Method: sr_handlepacket(uint8_t* p,char* interface)
 * Scope:  Global
 *
 * This method is called each time the router receives a packet on the
 * interface.  The packet buffer, the packet length and the receiving
 * interface are passed in as parameters. The packet is complete with
 * ethernet headers.
 *
 * Note: Both the packet buffer and the character's memory are handled
 * by sr_vns_comm.c that means do NOT delete either.  Make a copy of the
 * packet instead if you intend to keep it around beyond the scope of
 * the method call.
 *
 *---------------------------------------------------------------------*/

void sr_handlepacket(struct sr_instance* sr,
        uint8_t * packet/* lent */,
        unsigned int len,
        char* interface/* lent */)
{
    /* REQUIRES */
    assert(sr);
    assert(packet);
    assert(interface);

    printf("*** -> Received packet of length %d \n",len);

    /* fill in code here */
    struct sr_if* sr_interface = sr_get_interface(sr, interface);
    
    switch(ethertype(packet)) {
        case ethertype_arp: /* hex: 0x0806, dec: 2054 */
            sr_handle_arp_packet(sr, packet, len, sr_interface);
            break;
        case ethertype_ip: /* hex: 0x0800, dec: 2048 */
            sr_handle_ip_packet(sr, packet, len, sr_interface);
            break;
        default:
            Debug("Unknown Packet\n");
    }

}/* end sr_ForwardPacket */

void sr_handle_arp_packet(struct sr_instance* sr,
        uint8_t* packet/* lent */,
        unsigned int len,
        struct sr_if* interface/* lent */)
{
    sr_ethernet_hdr_t* ethernet_hdr = (sr_ethernet_hdr_t*) packet;
    sr_arp_hdr_t* arp_hdr = (sr_arp_hdr_t*)(packet + sizeof(sr_ethernet_hdr_t));
    
    switch(arp_hdr->ar_op) {
        case arp_op_request: /* 0x0001 */
            Debug("Requesting!!!!!\n");
            break;
        case arp_op_reply: /* 0x0002 */
            Debug("Replying!!!!!!!\n");
    }
}

void sr_handle_ip_packet(struct sr_instance* sr,
        uint8_t* packet/* lent */,
        unsigned int len,
        struct sr_if* interface/* lent */)
{
    Debug("IP Packet\n");
}
