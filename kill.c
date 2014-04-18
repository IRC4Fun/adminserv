/*
 * Copyright (c) 2014 Barret Rennie
 * Rights to this code are as documented in LICENSE
 *
 * adminserv: A service for atheme offering "psuedo-oper" services.
 *
 * The kill module offers kill capabilities to users with the
 * adminserv:kill privilege.
 */

#include "atheme.h"

#include "adminserv.h"

DECLARE_MODULE_V1(
	ADMINSERV_KILL_MODULE_NAME, false, _modinit, _moddeinit,
	"0.1",
	"Barret Rennie <https://www.brennie.ca>"
);

service_t *adminserv;

static void as_cmd_kill(sourceinfo_t *si, int parc, char *parv[]);

static command_t as_kill =
{
	"KILL", "Kill a user.",
	ADMINSERV_CAN_KILL, 3, as_cmd_kill, { .path = "adminserv/kill" }
};

static void as_cmd_kill(sourceinfo_t *si, int parc, char *parv[])
{
	const char *target_name = parv[0];
	const char *reason = parv[1];
	user_t *target = NULL;
	user_t *source = si->su;

	if (!source)
		command_fail(si, fault_noprivs, _("\2%s\2 can only be excuted via IRC."), "KILL");

	else if (!target_name || !reason)
	{
		command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "KILL");
		command_fail(si, fault_needmoreparams, _("Syntax: KILL <target> <reason>"));
	}
	else if ((target = user_find_named(target_name)) == NULL)
		command_fail(si, fault_nosuch_target, _("No such user: %s"), target_name);

	else if (service_find_nick(target_name) != NULL)
		command_fail(si, fault_noprivs, _("Cannot KILL: %s is a network service."), target_name);

	else if (is_admin(target) || is_ircop(target))
		command_fail(si, fault_noprivs, _("Cannot KILL: %s is an administrator or operator."), target_name);

	/* A user can only use KILL on a user that has less AdminServ privileges
	 * than the user.
	 */
	else if (has_priv_user(target, ADMINSERV_CAN_KILL)
		&& !(has_priv_user(source, ADMINSERV_CAN_AKILL) && !has_priv_user(target, ADMINSERV_CAN_AKILL)))

		command_fail(si, fault_noprivs, _("Cannot KILL: you have insufficient privileges to kill %s."), target_name);

	else
	{
		kill_user(adminserv->me, target, "%s (Requested by %s.)", reason, source->nick);
		logcommand(si, CMDLOG_DO, "KILL: \2%s\2", target_name);
		command_success_nodata(si, "Killed %s.", target_name);
	}
}


void _modinit(module_t *module)
{
	adminserv = service_find(ADMINSERV);

	service_bind_command(adminserv, &as_kill);
}

void _moddeinit(module_unload_intent_t intent)
{
	if (adminserv)
	{
		service_unbind_command(adminserv, &as_kill);
		adminserv = NULL;
	}
}

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */
