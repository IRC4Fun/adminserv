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


static void as_cmd_help(sourceinfo_t *si, int parc, char *parvp[])
{
	command_help(si, si->service->commands);
}

void _modinit(module_t *module)
{
	adminserv = service_add("AdminServ", NULL);

	service_bind_command(adminserv, &as_help);
};

void _moddeinit()
{
	service_unbind_command(adminserv, &as_help);

	if (adminserv)
	{
		service_delete(adminserv);
		adminserv = NULL;
	}
}
