/*
 * Copyright (c) 2014 Barret Rennie
 * Rights to this code are as documented in LICENSE
 *
 * A service for atheme offering "psuedo-oper" services.
 */

#include "atheme.h"

#include "adminserv.h"

DECLARE_MODULE_V1(
	ADMINSERV_MAIN_MODULE_NAME, false, _modinit, _moddeinit,
	"0.1",
	"Barret Rennie <https://www.brennie.ca>"
);

static service_t *adminserv;

static void as_cmd_help(sourceinfo_t *si, int parc, char *parv[]);

command_t as_help =
{
	"HELP", "Displays contextual help information.",
	AC_NONE, 2, as_cmd_help, { .path = "help" }
};

static void as_cmd_help(sourceinfo_t *si, int parc, char *parv[])
{
	const char *command = parv[0];

	if (!has_any_privs(si))
	{
		command_fail(si, fault_noprivs, _("You are not authorized to use %s."), si->service->nick);
		return;
	}

	if (!command)
	{
		command_success_nodata(si, _("***** \2%s Help\2 *****"), si->service->nick);
		command_success_nodata(si, _("\2%s\2 provides pseudo-oper services for adminstrators."), si->service->nick);
		command_success_nodata(si, " ");
		command_success_nodata(si, _("For information on a command, type:"));
		command_success_nodata(si, "\2/%s%s help <command>\2", (ircd->uses_rcommand == false) ? "msg " : "", si->service->disp);
		command_success_nodata(si, " ");

		command_help(si, si->service->commands);

		command_success_nodata(si, _("***** \2End of Help\2 *****"));
		return;
	}

	help_display(si, si->service, command, si->service->commands);
}

void _modinit(module_t *module)
{
	adminserv = service_add("AdminServ", NULL);

	service_bind_command(adminserv, &as_help);
};

void _moddeinit(module_unload_intent_t intent)
{
	service_unbind_command(adminserv, &as_help);

	if (adminserv)
	{
		service_delete(adminserv);
		adminserv = NULL;
	}
}

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */
