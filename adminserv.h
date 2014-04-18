#ifndef INCLUDE_ADMINSERV_H
#define INCLUDE_ADMINSERV_H

/* The service's name. */
#define ADMINSERV "AdminServ"

/* The individual module names. */
#define ADMINSERV_MAIN_MODULE_NAME "contrib/adminserv/main"
#define ADMINSERV_AKILL_MODULE_NAME "contrib/adminserv/akill"
#define ADMINSERV_INVITEME_MODULE_NAME "contrib/adminserv/inviteme"
#define ADMINSERV_KILL_MODULE_NAME "contrib/adminserv/kill"
#define ADMINSERV_MODE_MODULE_NAME "contrib/adminserv/mode"
#define ADMINSERV_WHOIS_MODULE_NAME "contrib/adminserv/whois"
#define ADMINSERV_WHOWAS_MODULE_NAME "contrib/adminserv/whowas"

/* Privileges for the various adminserv commands. */
#define ADMINSERV_CAN_AKILL "adminserv:akill"
#define ADMINSERV_CAN_AKILL_ANYMASK "adminserv:akill-anymask"
#define ADMINSERV_CAN_KILL "adminserv:kill"
#define ADMINSERV_CAN_INVITEME "adminserv:inviteme"
#define ADMINSERV_CAN_MODE "adminserv:mode"
#define ADMINSERV_CAN_WHOIS "adminserv:whois"
#define ADMINSERV_CAN_WHOWAS "adminserv:whowas"

/* IRC Numerics for WHOWAS replies. */
#define RPL_WHOWAS_USER "314"
#define RPL_WHOWAS_ACTUALLY "338"
#define RPL_WHOWAS_SERVER "312"
#define RPL_WHOWAS_LOGGEDIN "330"
#define RPL_ENDOF_WHOWAS "369"
#define RPL_ERR_WASNOSUCHNICK "406"

#endif
