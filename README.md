adminserv
=========

A service for atheme offering "psuedo-oper" services.

Compatability
-------------

This has only been tested with Charybdis and relies on the
modules/protocols/charybdis module for WHOWAS requests. If you try to load
modules/contrib/adminserv/whowas without first loading the charybdis module,
module loading will fail. Only load the whowas module after loading the
Charybdis module and only if you are actually connected to Charybdis.

Building & Installation
-----------------------

Clone the repository inside of atheme and run make:

````
cd atheme/modules/contrib
git clone https://github.com/brennie/adminserv.git
cd adminserv
make
make install
````

To install the help information (for the HELP command), the `help/adminserv`
directory should be copied into the atheme help directory:

````
cd atheme/
cp -a modules/contrib/adminserv/help/default/adminserv help/default/
make install
````

Privileges
----------

AdminServ adds the following privileges:

 * `adminserv:akill`
 * `adminserv:akill-anymask`
 * `adminserv:kill`
 * `adminserv:inviteme`
 * `adminserv:mode`
 * `adminserv:whois`
 * `adminserv:whowas`

Commands
--------

###HELP
Provides help information to the user requesting it.

###AKILL
Manage network bans. This command is exactly the same as the OperServ AKILL
command, except that it requires the `adminserv:akill` (instead of
`operserv:akill`) permission and adding an AKILL for any mask (i.e. very
general ones) requires `adminserv:akill-anymask` (instead of
`operserv:akill-anymask`).

###KILL
Kill users. This command requires the `adminserv:kill` privilege. A user cannot
kill another user which is an IRC operator or network admin or has equal or
higher adminserv privileges.

###INVITEME
Be invited to channels. This command requires the `adminserv:inviteme`
privilege.

###MODE
Put modes on channels. This command requires teh `adminserv:mode` privilege.

###WHOIS
View detailed WHOIS information about users. This command requires the
`adminserv:whois` privilege.

###WHOWAS
View detailed WHOWAS information about users. This command requires the
`adminserv:whowas` privilege. This command relies on the server supporting the
charybdis protocol for WHOWAS.

OperServ SPECS
--------------

To update OperServ to add privilege information from AdminServ, you must patch
OperServ to include the privileges. This patch is `operserv/specs.patch` and
can be applied as follows:

````
cd atheme/modules/operserv
patch specs.c ../contrib/adminserv/operserv/specs.patch
make specs.so
make install
````

Don't forget to reload the module in atheme afterwards.
