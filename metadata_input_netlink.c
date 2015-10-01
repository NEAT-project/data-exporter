/* Copyright (c) 2015, Celerway, Kristian Evensen <kristrev@celerway.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <libmnl/libmnl.h>
#include JSON_LOC
#include <getopt.h>

#include "metadata_exporter.h"
#include "metadata_input_netlink.h"
#include "netlink_helpers.h"
#include "backend_event_loop.h"

#include "lib/minmea.h"

static uint8_t md_input_netlink_parse_conn_event(struct md_conn_event *mce,
        struct json_object *meta_obj)
{
    json_object_object_foreach(meta_obj, key, val) {
        if (!strcmp(key, "md_seq"))
            mce->sequence = (uint16_t) json_object_get_int(val);

        if (!strcmp(key, "timestamp"))
            mce->tstamp = json_object_get_int64(val);

        if (!strcmp(key, "event_type"))
            mce->event_type = (uint8_t) json_object_get_int(val);

        if (!strcmp(key, "event_param"))
            mce->event_param = (uint8_t) json_object_get_int(val);

        if (!strcmp(key, "event_value"))
            mce->event_value = (uint8_t) json_object_get_int(val);

        if (!strcmp(key, "event_value_str"))
            mce->event_value_str = json_object_get_string(val);

        if (!strcmp(key, "interface_id_type"))
            mce->interface_id_type = (uint8_t) json_object_get_int(val);

        if (!strcmp(key, "interface_id"))
            mce->interface_id = json_object_get_string(val);

        if (!strcmp(key, "interface_type"))
            mce->interface_type = (uint8_t) json_object_get_int(val);

        if (!strcmp(key, "network_address_family"))
            mce->network_address_family = (uint8_t) json_object_get_int(val);

        if (!strcmp(key, "network_address"))
            mce->network_address = json_object_get_string(val);

        if (!strcmp(key, "network_provider_type"))
            mce->network_provider_type = (uint8_t) json_object_get_int(val);

        if (!strcmp(key, "network_provider"))
            mce->network_provider = json_object_get_int(val);

        if (!strcmp(key, "l3_session_id"))
            mce->l3_session_id = (uint64_t) json_object_get_int64(val);

        if (!strcmp(key, "l4_session_id"))
            mce->l4_session_id = (uint64_t) json_object_get_int64(val);

        if (!strcmp(key, "signal_strength"))
            mce->signal_strength = (int8_t) json_object_get_int(val);
    }

    if (!mce->tstamp || !mce->sequence ||
        !mce->l3_session_id || !mce->event_param ||
        !mce->interface_type || !mce->network_address_family ||
        !mce->network_address || !mce->interface_id ||
        !mce->interface_id_type) {
        fprintf(stderr, "Missing required argument in JSON\n");
        return RETVAL_FAILURE;
    }

    //We need to update the value in case of a connection event update, since it
    //is a string
    //TODO: Implement a more elegant technique if we get more cases like this
    if (mce->event_param == CONN_EVENT_META_UPDATE && !mce->event_value_str) {
        fprintf(stderr, "Missing event value for connection update\n");
        return RETVAL_FAILURE;
    }

    return RETVAL_SUCCESS;
}

static void md_input_netlink_handle_conn_event(struct md_input_netlink *min,
        struct json_object *obj)
{
    uint8_t retval = 0;

    memset(min->mce, 0, sizeof(struct md_conn_event));
    min->mce->md_type = META_TYPE_CONNECTION;
    //255 is reserved value used to indicate that there is no value to export
    //(look at writers)
    min->mce->event_value = UINT8_MAX;
    retval = md_input_netlink_parse_conn_event(min->mce, obj);
    
    if (retval == RETVAL_FAILURE)
        return;

    mde_publish_event_obj(min->parent, (struct md_event*) min->mce);
}

static void md_input_netlink_handle_gps_event(struct md_input_netlink *min,
                                              struct json_object *json_obj)
{
    struct md_gps_event gps_event;
    uint8_t retval = 0;
    int8_t sentence_id = 0;

    union {
        struct minmea_sentence_gga gga;
        struct minmea_sentence_rmc rmc;
    } gps;

    memset(&gps_event, 0, sizeof(struct md_gps_event));
    gps_event.md_type = META_TYPE_POS;

    json_object_object_foreach(json_obj, key, val) {
        if (!strcmp(key, "md_seq"))
            gps_event.sequence = (uint16_t) json_object_get_int(val);

        if (!strcmp(key, "timestamp"))
            gps_event.tstamp = json_object_get_int64(val);

        if (!strcmp(key, "nmea_string"))
            gps_event.nmea_raw = json_object_get_string(val);
    }

    if (!gps_event.sequence || !gps_event.nmea_raw)
        return;

    sentence_id = minmea_sentence_id(gps_event.nmea_raw, 0);

    if (sentence_id <= 0)
        return;
    
    gps_event.minmea_id = sentence_id;

    //We can ignore NMEA checksum
    switch (sentence_id) {
    case MINMEA_SENTENCE_GGA:
        retval = minmea_parse_gga(&gps.gga, gps_event.nmea_raw);
        if (retval && !gps.gga.fix_quality) {
            retval = 0;
        } else {
            gps_event.time = gps.gga.time;
            gps_event.latitude = minmea_tocoord(&gps.gga.latitude);
            gps_event.longitude = minmea_tocoord(&gps.gga.longitude);
            gps_event.altitude = minmea_tofloat(&gps.gga.altitude);
            gps_event.satellites_tracked = gps.gga.satellites_tracked;
        }
        break;
    case MINMEA_SENTENCE_RMC:
        retval = minmea_parse_rmc(&gps.rmc, gps_event.nmea_raw);

        if (retval && !gps.rmc.valid) {
            retval = 0;
        } else {
            gps_event.time = gps.rmc.time;
            gps_event.latitude = minmea_tocoord(&gps.rmc.latitude);
            gps_event.longitude = minmea_tocoord(&gps.rmc.longitude);
            gps_event.speed = minmea_tofloat(&gps.rmc.speed);
        }
        break;
    default:
        break;
    }

    if (!retval)
        return;

    mde_publish_event_obj(min->parent, (struct md_event *) &gps_event);
}

static uint8_t md_input_netlink_add_json_key_value(const char *key,
        int32_t value, struct json_object *obj)
{
    struct json_object *obj_add = NULL;
    obj_add = json_object_new_int(value);

    if (!obj_add)
        return RETVAL_FAILURE;

    json_object_object_add(obj, key, obj_add);

    return RETVAL_SUCCESS;
}

static void md_input_netlink_handle_event(void *ptr, int32_t fd, uint32_t events)
{
    struct md_input_netlink *min = ptr;
    uint8_t rcv_buf[MNL_SOCKET_BUFFER_SIZE];
    ssize_t retval;
    struct nlmsghdr *nlh = (struct nlmsghdr*) rcv_buf;
    const char *nlh_payload;
    struct json_object *nlh_obj = NULL, *json_event = NULL;
    uint8_t event_type = 0;

    memset(rcv_buf, 0, sizeof(rcv_buf));

    //TODO: Consider adding support for fragmented data
    retval = mnl_socket_recvfrom(min->metadata_sock, rcv_buf,
            MNL_SOCKET_BUFFER_SIZE);

    if (retval <= 0)
        return;

    nlh_payload = mnl_nlmsg_get_payload(nlh);
    nlh_obj = json_tokener_parse(nlh_payload);

    if (!nlh_obj) {
        fprintf(stderr, "Received invalid JSON object on Netlink socket\n");
        return;
    }

    //We are inserting version and sequence number. Version is so that the
    //application handling this data knows if it supports the format or not.
    //Sequence is so that we can see the order in which events arrived at the
    //metadata exporter, making it easier to correlate events between
    //applications. The different applications publishing data might also insert
    //their own sequence number
    if (md_input_netlink_add_json_key_value("md_seq", mde_inc_seq(min->parent), nlh_obj) ||
        md_input_netlink_add_json_key_value("md_ver", MDE_VERSION, nlh_obj)) {
        json_object_put(nlh_obj);
        return;
    }

    if (!json_object_object_get_ex(nlh_obj, "event_type", &json_event)) {
        fprintf(stderr, "Missing event type\n");
        json_object_put(nlh_obj);
        return;
    }

    event_type = (uint8_t) json_object_get_int(json_event);

    if (!(event_type & min->md_nl_mask))
        return;

    fprintf(stdout, "Got JSON %s\n", json_object_to_json_string(nlh_obj));

    switch (event_type) {
    case META_TYPE_INTERFACE:
        fprintf(stderr, "Interface event type is not implemented yet\n");
        break;
    case META_TYPE_CONNECTION:
        md_input_netlink_handle_conn_event(min, nlh_obj);
        break;
    case META_TYPE_POS:
        md_input_netlink_handle_gps_event(min, nlh_obj);
        break;
    default:
        fprintf(stderr, "Unknown event type\n");
        break;
    }

    json_object_put(nlh_obj);
}

static uint8_t md_input_netlink_config(struct md_input_netlink *min)
{

    //TODO: This code will be refactored to different importers, similar to
    //outputs
    min->metadata_sock = nlhelper_create_socket(NETLINK_USERSOCK,
            (1 << (METADATA_NL_GROUP - 1)));

    if (min->metadata_sock == NULL)
        return RETVAL_FAILURE;

    if(!(min->event_handle = backend_create_epoll_handle(min,
                    mnl_socket_get_fd(min->metadata_sock),
                    md_input_netlink_handle_event)))
        return RETVAL_FAILURE;

    backend_event_loop_update(min->parent->event_loop, EPOLLIN, EPOLL_CTL_ADD,
        mnl_socket_get_fd(min->metadata_sock), min->event_handle);

    //TODO: Move to handler
    min->mce = calloc(sizeof(struct md_conn_event), 1);
    if (min->mce == NULL)
        return RETVAL_FAILURE;
    
    return RETVAL_SUCCESS;
}

static uint8_t md_input_netlink_init(void *ptr, int argc, char *argv[])
{
    struct md_input_netlink *min = ptr;
    uint32_t md_nl_mask = 0;

    int c, option_index = 0;

    static struct option gpsd_options[] = {
        {"nl_conn", no_argument,  0,  0},
        {"nl_pos",  no_argument,  0,  0},
        {0,                   0,  0,  0}};
    
    while (1) {
        //No permuting of array here as well
        c = getopt_long_only(argc, argv, "--", gpsd_options, &option_index);

        if (c == -1)
            break;
        else if (c)
            continue;

        if (!strcmp(gpsd_options[option_index].name, "nl_conn"))
            md_nl_mask |= META_TYPE_CONNECTION;
        else if (!strcmp(gpsd_options[option_index].name, "nl_pos"))
            md_nl_mask |= META_TYPE_POS;
    }

    if (!md_nl_mask) {
        fprintf(stderr, "At least one netlink event type must be present\n");
        return RETVAL_FAILURE;
    }

    min->md_nl_mask = md_nl_mask;

    //Netlink has no arguments (for now), so just go right to config
    return md_input_netlink_config(min);
}

static void md_netlink_usage()
{
    fprintf(stderr, "Netlink input (at least one event type must be present):\n");
    fprintf(stderr, "--nl_conn: Receive netlink connection events\n");
    fprintf(stderr, "--nl_pos: Receive netlink position events\n");
}

void md_netlink_setup(struct md_exporter *mde, struct md_input_netlink *min)
{
    min->parent = mde;
    min->init = md_input_netlink_init;
    min->usage = md_netlink_usage;
}
