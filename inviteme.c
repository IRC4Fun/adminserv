/*
 * Copyright (c) 2014 Barret Rennie
 * Rights to this code are as documented in LICENSE
 *
 * A service for atheme offering "psuedo-oper" services.
 *
 * The inviteme module allows the user to be invited to channels.
 */

#include "atheme.h"

#include "adminserv.h"

DECLARE_MODULE_V1(
	ADMINSERV_INVITEME_MODULE_NAME, false, _modinit, _moddeinit,
	"0.1",
	"Barret Rennie <https://www.brennie.ca>"
);

static void as_cmd_inviteme(sourceinfo_t *si, int parc, char *parv[]);

command_t as_inviteme =
{
	"INVITEME", "Be invited to channel.",
	AC_NONE, 2, as_cmd_inviteme, { .path = "adminserv/inviteme" }
};

static void as_cmd_inviteme(sourceinfo_t *si, int parc, char *parv[])
{
	const char *target = parv[0];
	channel_t *channel = NULL;

	if (!si->su)
		command_fail(si, fault_noprivs, _("\2%s\2 can only be excuted via IRC."), "INVITEME");

	else if (!target)
	{
		command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "INVITEME");
		command_fail(si, fault_needmoreparams, "Syntax: INVITEME <#channel>");
	}
	else if ((channel = channel_find(target)) == NULL)
		command_fail(si, fault_nosuch_target, "No such channel: %s", target);
	
	else
	{
		invite_sts(si->service->me, si->su, channel);
		logcommand(si, CMDLOG_DO, "INVITEME: \2%s\2", channel->name);
		command_success_nodata(si, "You have been invited to %s", channel->name);
	}
}

void _modinit(module_t *module)
{
	service_named_bind_command(ADMINSERV, &as_inviteme);
};

void _moddeinit(module_unload_intent_t intent)
{
	service_named_unbind_command(ADMINSERV, &as_inviteme);
}

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */
