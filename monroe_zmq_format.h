#pragma once

#define MONROE_ZMQ_DATA_VERSION 1
#define MONROE_ZMQ_DATA_ID_SYSEVENT      "MONROE.META.NODE.EVENT"
#define MONROE_ZMQ_DATA_ID_SENSOR        "MONROE.META.NODE.SENSOR"
#define MONROE_ZMQ_DATA_ID_MODEM         "MONROE.META.DEVICE.MODEM"
#define MONROE_ZMQ_DATA_ID_CONNECTIVITY  "MONROE.META.CONNECTIVITY"
#define MONROE_ZMQ_DATA_ID_GPS           "MONROE.META.DEVICE.GPS"

#define MONROE_ZMQ_TOPIC_SYSEVENT        "MONROE.META.NODE.EVENT"
#define MONROE_ZMQ_TOPIC_SENSOR          "MONROE.META.NODE.SENSOR"
#define MONROE_ZMQ_TOPIC_MODEM           "MONROE.META.DEVICE.MODEM"
#define MONROE_ZMQ_TOPIC_CONNECTIVITY    "MONROE.META.DEVICE.CONNECTIVITY"
#define MONROE_ZMQ_TOPIC_GPS             "MONROE.META.DEVICE.GPS"

#define MONROE_ZMQ_TOPIC_MODEM_STATE            "STATE"
#define MONROE_ZMQ_TOPIC_MODEM_MODE             "MODE"
#define MONROE_ZMQ_TOPIC_MODEM_SIGNAL           "SIGNAL"
#define MONROE_ZMQ_TOPIC_MODEM_LTE_BAND         "LTEBAND"
#define MONROE_ZMQ_TOPIC_MODEM_ISP_NAME         "ISPNAME"
#define MONROE_ZMQ_TOPIC_MODEM_UPDATE           "UPDATE"
#define MONROE_ZMQ_TOPIC_MODEM_IP_ADDR          "IPADDR"
#define MONROE_ZMQ_TOPIC_MODEM_LOC_CHANGE       "LOCCHANGE"
#define MONROE_ZMQ_TOPIC_MODEM_NW_MCCMNC_CHANGE "NWMCCMNCCHANGE"

#define ZMQ_KEY_SEQ            "SequenceNumber"
#define ZMQ_KEY_TSTAMP         "Timestamp"
#define ZMQ_KEY_DATAID         "DataId"
#define ZMQ_KEY_DATAVERSION    "DataVersion"

#define ZMQ_KEY_CID            "CID"
#define ZMQ_KEY_DEVICE_MODE    "DeviceMode"
#define ZMQ_KEY_DEVICE_SUBMODE "DeviceSubmode"
#define ZMQ_KEY_DEVICE_STATE   "DeviceState"
#define ZMQ_KEY_ECIO           "ECIO"
#define ZMQ_KEY_ENODEB_ID      "ENODEBID"
#define ZMQ_KEY_ICCID          "ICCID"
#define ZMQ_KEY_IF_NAME        "InterfaceName"
#define ZMQ_KEY_IMSI           "IMSI"
#define ZMQ_KEY_IMSI_MCCMNC    "IMSIMCCMNC"
#define ZMQ_KEY_IMEI           "IMEI"
#define ZMQ_KEY_IP_ADDR        "IPAddress"
#define ZMQ_KEY_INTERNAL_IP_ADDR "InternalIPAddress"
#define ZMQ_KEY_ISP_NAME       "Operator"
#define ZMQ_KEY_LAC            "LAC"
#define ZMQ_KEY_LTE_RSRP       "LTERSRP"
#define ZMQ_KEY_LTE_FREQ       "LTEFrequency"
#define ZMQ_KEY_LTE_RSSI       "LTERSSI"
#define ZMQ_KEY_LTE_RSRQ       "LTERSRQ"
#define ZMQ_KEY_LTE_BAND       "LTEBand"
#define ZMQ_KEY_LTE_PCI        "LTEPCI"
#define ZMQ_KEY_MODE           "Mode"
#define ZMQ_KEY_NW_MCCMNC      "NWMCCMNC"
#define ZMQ_KEY_RSCP           "RSCP"
#define ZMQ_KEY_RSSI           "RSSI"
#define ZMQ_KEY_SIGNAL         "SignalStrength"

#define ZMQ_KEY_INTERFACEID    "InterfaceId"
#define ZMQ_KEY_INTERFACENAME  "InterfaceName"
#define ZMQ_KEY_OPERATOR       "Operator"
#define ZMQ_KEY_MODE           "Mode"
#define ZMQ_KEY_SIGNAL         "SignalStrength"

#define ZMQ_KEY_ALTITUDE       "Altitude"
#define ZMQ_KEY_LONGITUDE      "Longitude"
#define ZMQ_KEY_LATITUDE       "Latitude"
#define ZMQ_KEY_NUMSAT         "NumberOfSatellites"
#define ZMQ_KEY_NMEA           "NMEA"
#define ZMQ_KEY_SPEED          "Speed"

#define ZMQ_TOPIC_RADIO_CELL_LOCATION_GERAN "RADIO.CELL_LOCATION_GERAN"
#define ZMQ_TOPIC_RADIO_GSM_RR_CELL_SEL_RESEL_PARAM "RADIO.GSM_RR_CELL_SEL_RESEL_PARAM"
#define ZMQ_TOPIC_RADIO_GRR_CELL_RESEL "RADIO.GRR_CELL_RESEL"
#define ZMQ_TOPIC_RADIO_GSM_RR_CIPHER_MODE "RADIO.GSM_RR_CIPHER_MODE"
#define ZMQ_TOPIC_RADIO_GSM_RR_CHANNEL_CONF "RADIO.GSM_RR_CHANNEL_CONF"

#define ZMQ_KEY_RADIO_CELL_ID               "cell_id"
#define ZMQ_KEY_RADIO_PLMN                  "plmn"
#define ZMQ_KEY_RADIO_LAC                   "lac"
#define ZMQ_KEY_RADIO_ARFCN                 "arfcn"
#define ZMQ_KEY_RADIO_BSIC                  "bsic"
#define ZMQ_KEY_RADIO_TIMING_ADVANCE        "timing_advance"
#define ZMQ_KEY_RADIO_RX_LEV                "rx_lev"
#define ZMQ_KEY_RADIO_CELL_GERAN_INFO_NMR   "cell_geran_info_nmr"

#define ZMQ_KEY_RADIO_SERVING_BCCH_ARFCN        "serving_bcch_arfcn"
#define ZMQ_KEY_RADIO_SERVING_PBCCH_ARFCN       "serving_pbcch_arfcn"
#define ZMQ_KEY_RADIO_SERVING_PRIORITY_CLASS    "serving_priority_class"
#define ZMQ_KEY_RADIO_SERVING_RXLEV_AVG         "serving_rxlev_avg"
#define ZMQ_KEY_RADIO_SERVING_C1                "serving_c1"
#define ZMQ_KEY_RADIO_SERVING_C2                "serving_c2"
#define ZMQ_KEY_RADIO_SERVING_C31               "serving_c31"
#define ZMQ_KEY_RADIO_SERVING_C32               "serving_c32"
#define ZMQ_KEY_RADIO_SERVING_FIVE_SECOND_TIMER "serving_five_second_timer"
#define ZMQ_KEY_RADIO_CELL_RESELET_STATUS       "cell_reselet_status"
#define ZMQ_KEY_RADIO_RECENT_CELL_SELECTION     "recent_cell_selection"
#define ZMQ_KEY_RADIO_GRR_CELL_NEIGHBORS        "grr_cell_neighbors"
