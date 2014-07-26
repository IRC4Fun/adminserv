/*
 * Copyright (c) 2014 Barret Rennie
 * Rights to this code are as documented in LICENSE
 *
 * adminserv: A service for atheme offering "psuedo-oper" services.
 *
 * The whowas module allows forwarding of remote WHOWAS queries to users with
 * the adminserv:whowas privilege.
 */

#include <stdio.h>

#include "atheme.h"
#include "uplink.h"
#include "pmodule.h"

#include "adminserv.h"

DECLARE_MODULE_V1(
	ADMINSERV_WHOWAS_MODULE_NAME, false, _modinit, _moddeinit,
	"0.1",
	"Barret Rennie <https://www.brennie.ca>"
);

static service_t *adminserv;

static void as_cmd_whowas(sourceinfo_t *si, int parc, char *parv[]);

static command_t as_whowas =
{
	"WHOWAS", "Get information about a past user.",
	ADMINSERV_CAN_WHOWAS, 1, as_cmd_whowas, { .path = "adminserv/whowas" }
};

/* A WHOWAS request entry. We need to keep these around so that we know who to
 * forward the results to and so that we can handle more than one WHOWAS
 * request at once.
 */
typedef struct
{
	const char *origin; /* The user who requested the numeric. */
	const char *target; /* The target of the request. */
	unsigned count;
} whowas_request_t;

/* The list of outstanding WHOWAS requests, with the oldest request at the
 * head of the list.
 */
static mowgli_list_t *whowas_requests = NULL;

/* A heap for allocating multiple whowas_request_t. */
static mowgli_heap_t *request_heap = NULL;

static void free_whowas_request(whowas_request_t **request)
{
	if (!request || !*request)
		return;

	mowgli_free((void *)(*request)->origin);
	mowgli_free((void *)(*request)->target);
	mowgli_heap_free(request_heap, *request);
	*request = NULL;
}

static void handle_whowas_user(sourceinfo_t *si, int parc, char *parv[])
{
	if (!whowas_requests)
		slog(LG_DEBUG, ADMINSERV_WHOWAS_MODULE_NAME " received RPL_WHOWASUSER, but whowas_requests = NULL");

	else if (!whowas_requests->head)
		slog(LG_DEBUG, ADMINSERV_WHOWAS_MODULE_NAME " recieved RPL_WHOWASUSER, but whowas_requests->head = NULL");

	else
	{
		mowgli_node_t *head = whowas_requests->head;
		whowas_request_t *request = head->data;
		user_t *origin = user_find_named(request->origin);

		request->count++;

		if (origin)
		{
			const char *nick = parv[1];
			const char *user = parv[3];
			const char *host = parv[2];
			const char *gecos = parv[5];

			if (request->count == 1)
				notice(adminserv->nick, origin->nick, _("WHOWAS Results for %s:"), request->target);
			else
				notice(adminserv->nick, origin->nick, " ");

			notice(adminserv->nick, origin->nick, "%d: %s!%s@%s (%s)", request->count, nick, host, user, gecos);
		}
	}
}

static void handle_whowas_actually(sourceinfo_t *si, int parc, char *parv[])
{
	if (!whowas_requests)
		slog(LG_DEBUG, ADMINSERV_WHOWAS_MODULE_NAME " received RPL_WHOWASACTUALY, but whowas_requests = NULL");

	else if (!whowas_requests->head)
		slog(LG_DEBUG, ADMINSERV_WHOWAS_MODULE_NAME " recieved RPL_WHOWASACTUALLY, but whowas_requests->head = NULL");

	else
	{
		mowgli_node_t *head = whowas_requests->head;
		whowas_request_t *request = head->data;
		user_t *origin = user_find_named(request->origin);

		if (origin)
			notice(adminserv->nick, origin->nick, _("%d: Actually using host %s"), request->count, parv[2]);
	}
}

static void handle_whowas_server(sourceinfo_t *si, int parc, char *parv[])
{
	if (!whowas_requests)
		slog(LG_DEBUG, ADMINSERV_WHOWAS_MODULE_NAME " received RPL_WHOWASSERVER, but whowas_requests = NULL");

	else if (!whowas_requests->head)
		slog(LG_DEBUG, ADMINSERV_WHOWAS_MODULE_NAME " recieved RPL_WHOWASSERVER, but whowas_requests->head = NULL");

	else
	{
		mowgli_node_t *head = whowas_requests->head;
		whowas_request_t *request = head->data;
		user_t *origin = user_find_named(request->origin);

		const char *server = parv[2];
		const char *ts = parv[3];

		if (origin)
			notice(adminserv->nick, origin->nick, _("%d: Disconnected from %s at %s UTC"), request->count, server, ts);
	}
}

static void handle_whowas_loggedin(sourceinfo_t *si, int parc, char *parv[])
{
	if (!whowas_requests)
		slog(LG_DEBUG, ADMINSERV_WHOWAS_MODULE_NAME " received RPL_WHOWAS_LOGGEDIN, but whowas_requests = NULL");

	else if (!whowas_requests->head)
		slog(LG_DEBUG, ADMINSERV_WHOWAS_MODULE_NAME " recieved RPL_WHOWAS_LOGGEDIN, but whowas_requests->head = NULL");

	else
	{
		mowgli_node_t *head = whowas_requests->head;
		whowas_request_t *request = head->data;
		user_t *origin = user_find_named(request->origin);

		if (origin)
			notice(adminserv->nick, origin->nick, _("%d: Was logged in as %s"), request->count, parv[2]);
	}
}

static void handle_endof_whowas(sourceinfo_t *si, int parc, char *parv[])
{
	if (!whowas_requests)
		slog(LG_DEBUG, ADMINSERV_WHOWAS_MODULE_NAME " received RPL_ENDOF_WHOWAS, but whowas_requests = NULL");

	else if (!whowas_requests->head)
		slog(LG_DEBUG, ADMINSERV_WHOWAS_MODULE_NAME " received RPL_ENDOF_WHOWAS, but whowas_requests->head = NULL");

	else
	{
		mowgli_node_t *head = whowas_requests->head;
		whowas_request_t *request = head->data;
		user_t *origin = user_find_named(request->origin);

		if (origin)
			notice(adminserv->nick, origin->nick, _("End of WHOWAS for %s (%u results)"), request->target, request->count);

		free_whowas_request(&request);
		mowgli_node_delete(head, whowas_requests);
		mowgli_node_free(head);
	}
}

static void as_cmd_whowas(sourceinfo_t *si, int parc, char *parv[])
{
	char *target_name = parv[0];
	whowas_request_t *request;
	mowgli_node_t *node;
	
	if (!si->su)
		command_fail(si, fault_noprivs, _("\2%s\2 can only be excuted via IRC."), "INVITEME");

	else
	{
		/* Send a WHOWAS request from AdminServ to the current uplink. */
		sts(":%s WHOWAS %s 5 %s", CLIENT_NAME(adminserv->me), target_name, curr_uplink->name);

		request = mowgli_heap_alloc(request_heap);
		node = mowgli_node_create();
		
		request->origin = mowgli_strdup(si->su->nick);
		request->target = mowgli_strdup(target_name);
		request->count = 0;

		mowgli_node_add(request, node, whowas_requests);


		logcommand(si, CMDLOG_DO, "WHOWAS: \2%s\2", target_name);
	}
}

void handle_err_wasnosuchnick(sourceinfo_t *si, int parc, char *parv[])
{
	if (whowas_requests && whowas_requests->head)
	{
		whowas_request_t *request = whowas_requests->head->data;
		user_t *origin = user_find_named(request->origin);

		if (!origin)
			return;

		notice(adminserv->nick, origin->nick, _("No such nick: %s"), request->target);
	}
}

void _modinit(module_t *module)
{
	if (!module_find_published("protocol/charybdis"))
	{
		slog(LG_ERROR, "module modules/protocol/charybdis is not loaded; modules/contrib/adminserv/whowas requires this module.");
		slog(LG_ERROR, "either load this module (if you are connected to charybdis) or remove modules/contrib/adminserv/whowas from your atheme.conf");
		module->mflags = MODTYPE_FAIL;

		return;
	}

	adminserv = service_find(ADMINSERV);

	service_bind_command(adminserv, &as_whowas);

	request_heap = sharedheap_get(sizeof(whowas_request_t));
	whowas_requests = mowgli_list_create();

	/* We hook into IRC numerics. AdminServ will send a WHOWAS request to the
	 * uplink and we listen for the response with these handlers.
	 */
	pcommand_add(RPL_WHOWAS_USER, handle_whowas_user, 6, MSRC_SERVER);
	pcommand_add(RPL_WHOWAS_ACTUALLY, handle_whowas_actually, 4, MSRC_SERVER);
	pcommand_add(RPL_WHOWAS_SERVER, handle_whowas_server, 4, MSRC_SERVER);
	pcommand_add(RPL_WHOWAS_LOGGEDIN, handle_whowas_loggedin, 4, MSRC_SERVER);
	pcommand_add(RPL_ENDOF_WHOWAS, handle_endof_whowas, 0, MSRC_SERVER);
	pcommand_add(RPL_ERR_WASNOSUCHNICK, handle_err_wasnosuchnick, 0, MSRC_SERVER);
}

void _moddeinit(module_unload_intent_t intent)
{
	if (adminserv)
	{
		mowgli_node_t *node = whowas_requests ? whowas_requests->head : NULL;

		service_unbind_command(adminserv, &as_whowas);

		pcommand_delete(RPL_WHOWAS_USER);
		pcommand_delete(RPL_WHOWAS_ACTUALLY);
		pcommand_delete(RPL_WHOWAS_SERVER);
		pcommand_delete(RPL_WHOWAS_LOGGEDIN);
		pcommand_delete(RPL_ENDOF_WHOWAS);
		pcommand_delete(RPL_ERR_WASNOSUCHNICK);

		/* Free all remaining requests. */
		while (node)
		{
			mowgli_node_t *toDelete = node;
			whowas_request_t *request = node->data;

			free_whowas_request(&request);

			mowgli_node_delete(node, whowas_requests);

			node = node->next;

			mowgli_node_free(toDelete);
		}

		mowgli_list_free(whowas_requests);

		adminserv = NULL;
	}
}

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */
