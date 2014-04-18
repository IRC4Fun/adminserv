/*
 * Copyright (c) 2005-2006 William Pitcock, et al.
 * Modified  (c) 2014 Barret Rennie for incorporation into AdminServ.
 * Rights to this code are as documented in doc/LICENSE.
 *
 * This file contains functionality which implements the OService MODE command.
 *
 */

#include "atheme.h"

DECLARE_MODULE_V1
(
	"operserv/mode", false, _modinit, _moddeinit,
	PACKAGE_STRING,
	"Atheme Development Group <http://www.atheme.org>"
);

static void as_cmd_mode(sourceinfo_t *si, int parc, char *parv[]);

command_t as_mode = { "MODE", N_("Changes modes on channels."), ADMINSERV_CAN_MODE, 2, as_cmd_mode, { .path = "adminserv/mode" } };

void _modinit(module_t *m)
{
	service_named_bind_command(ADMINSERV, &as_mode);
}

void _moddeinit(module_unload_intent_t intent)
{
	service_named_unbind_command(ADMINSERV, &as_mode);
}

static void as_cmd_mode(sourceinfo_t *si, int parc, char *parv[])
{
	char *channel = parv[0];
	char *mode = parv[1];
	channel_t *c;
	int modeparc;
	char *modeparv[256];

	if (!channel || !mode)
	{
		command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "MODE");
		command_fail(si, fault_needmoreparams, _("Syntax: MODE <channel> <parameters>"));
		return;
	}

	c = channel_find(channel);
	if (!c)
	{
		command_fail(si, fault_nosuch_target, _("Channel \2%s\2 does not exist."), channel);
		return;
	}

	wallops("\2%s\2 is using MODE on \2%s\2 (set: \2%s\2)", get_oper_name(si), channel, mode);
	logcommand(si, CMDLOG_ADMIN, "MODE: \2%s\2 on \2%s\2", mode, channel);
	command_success_nodata(si, _("Setting modes \2%s\2 on \2%s\2."), mode, channel);

	modeparc = sjtoken(mode, ' ', modeparv);

	channel_mode(si->service->me, c, modeparc, modeparv);
}

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */