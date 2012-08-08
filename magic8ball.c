#include <unistd.h> 
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

#include "debug.h"
#include "signals.h"
#include "util.h"
#include "version.h"
#include "cmds.h"
#include "conversation.h"

#define ME "8Ball-plugin"
#define MAXLENGTH 1024
#define MAGIC8BALL_PLUGIN_VERSION "I am 8-ball Plugin 1.0 written by Ron with special thanks to dark_drake."

static GaimCmdId cmd;

static char *responses[] = {
								"Magic 8-Ball says: Signs point to yes.",        /* 1  */
								"Magic 8-Ball says: Yes.",                       /* 2  */
								"Magic 8-Ball says: Most likely.",               /* 3  */
								"Magic 8-Ball says: Without a doubt.",           /* 4  */
								"Magic 8-Ball says: Yes - definitely.",          /* 5  */
								"Magic 8-Ball says: As I see it, yes.",          /* 6  */
								"Magic 8-Ball says: You may rely on it.",        /* 7  */
								"Magic 8-Ball says: Outlook good.",              /* 8  */
								"Magic 8-Ball says: It is certain.",             /* 9  */
								"Magic 8-Ball says: It is decidedly so.",        /* 10 */
								"Magic 8-Ball says: Reply hazy, try again.",     /* 11 */
								"Magic 8-Ball says: Better not tell you now.",   /* 12 */
								"Magic 8-Ball says: Ask again later.",           /* 13 */
								"Magic 8-Ball says: Concentrate and ask again.", /* 14 */
								"Magic 8-Ball says: Cannot predict now.",        /* 15 */
								"Magic 8-Ball says: My sources say no.",         /* 16 */
								"Magic 8-Ball says: Very doubtful.",             /* 17 */
								"Magic 8-Ball says: My reply is no.",            /* 18 */
								"Magic 8-Ball says: Outlook not so good.",       /* 19 */
								"Magic 8-Ball says: Don't count on it."          /* 20 */
							};
#define COUNT 20

void write_conversation(GaimConversation *conv, const char *message);
gboolean magic8ball(GaimConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data);


void write_conversation(GaimConversation *conv, const char *message)
{
	gaim_conversation_write(conv,
				"",
				message,
				GAIM_MESSAGE_NO_LOG,
				time( NULL ));
}

gboolean magic8ball(GaimConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data)
{
	int i = (int) (rand() % COUNT);
	char *msg = responses[i];

	gaim_conv_im_send(GAIM_CONV_IM(conv), msg);

    return GAIM_CMD_STATUS_OK;
}

static gboolean plugin_load(GaimPlugin *plugin)
{
	cmd = gaim_cmd_register("8", "", GAIM_CMD_P_DEFAULT, GAIM_CMD_FLAG_IM, NULL, (GaimCmdFunc)magic8ball, "/8", NULL);
	
	return TRUE;
}

static gboolean plugin_unload(GaimPlugin *plugin)
{
	gaim_cmd_unregister (cmd);

	return TRUE;
}

static GaimPluginInfo info =
{
	GAIM_PLUGIN_MAGIC,
	GAIM_MAJOR_VERSION,
	GAIM_MINOR_VERSION,
	GAIM_PLUGIN_STANDARD,                    /** type */
	NULL,                                    /** ui_requirement */
	0,                                       /** flags */
	NULL,                                    /** dependencies   */
	GAIM_PRIORITY_DEFAULT,                   /** priority    */
	NULL,                                    /** id   */
	N_("Magic8Ball"),                        /** name    */
	VERSION,                                 /** version */
	N_("Does magic 8-ball responses."),      /** summary */
	N_("Does magic 8-ball responses."),      /** description */
	"Ron <ron@skullsecurity.com>",           /** author  */
	"http://www.javaop.com",                 /** homepage    */
	plugin_load,                             /** load    */
	plugin_unload,                           /** unload  */
	NULL,                                    /** destroy */
	NULL,                                    /** ui_info */
	NULL,                                    /** extra_info  */
	NULL,
	NULL
};


static void init_plugin(GaimPlugin *plugin)
{
	srand(time((time_t*) 0));
}

GAIM_INIT_PLUGIN(magic8ball, init_plugin, info)


