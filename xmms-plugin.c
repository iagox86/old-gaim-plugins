#include <unistd.h> 
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <xmms/xmmsctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "gtkgaim.h"

#include "debug.h"
#include "signals.h"
#include "util.h"
#include "version.h"
#include "cmds.h"
#include "conversation.h"

#include "gtkplugin.h"
#include "gtkutils.h"

#define ME "XMMS-Plugin"
#define MAXLENGTH 1024
#define XMMS_PLUGIN_VERSION "I am XmmsPlugin v1.02 written by iago with special thanks to Ergot."

static int xmms_session = 0;

static GaimCmdId noargcmd;
static GaimCmdId oneargcmd;
static GaimCmdId twoargcmd;

void write_conversation(GaimConversation *conv, const char *message)
{
	gaim_conversation_write(conv,
				"",
				message,
				GAIM_MESSAGE_NO_LOG,
				time( NULL ));
}

gboolean CheckXmmsRunning(GaimConversation *conv)
{
	if(xmms_remote_is_running(xmms_session) == FALSE)
	{
		write_conversation(conv, "Xmms could not be found running.");
		return FALSE;
	}
	return TRUE;
}

/* When /xmms is used with no args[0], the current song is displayed to the conversation */
gboolean XmmsNoParams(GaimConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data)
{
	if(CheckXmmsRunning(conv))
	{
		int position = xmms_remote_get_playlist_pos(xmms_session);
		int time = xmms_remote_get_output_time(xmms_session);
		int length = xmms_remote_get_playlist_time(xmms_session, position);
		char *title = xmms_remote_get_playlist_title(xmms_session, position);

		int rate = 0;
		int freq = 0;
		int nch = 0;

		xmms_remote_get_info(xmms_session, &rate, &freq, &nch);

		char *status = "";
		if(xmms_remote_is_paused(xmms_session))
			status = " [paused]";
		else if(xmms_remote_is_playing(xmms_session) == FALSE)
			status = " [stopped]";

		char newmessage[MAXLENGTH];
		snprintf(newmessage, MAXLENGTH,
				"I am listening to %d. %s (%d:%02d / %d:%02d%s %dkbps)",
				position + 1,
				title,
				time / 60000,
				(time % 60000) / 1000,
				length / 60000,
				(length % 60000) / 1000,
				status,
				rate);

		/* Filter out illegal characters. */
		unsigned int i;
		for(i = 0; i < strlen(newmessage); i++)
			if((unsigned char)newmessage[i] > 0x7F)
				newmessage[i] = '_';

		gaim_conv_im_send(GAIM_CONV_IM(conv), newmessage);
	}


	return GAIM_CMD_STATUS_OK;
}

gboolean XmmsSingleParam(GaimConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data)
{
	if(strcasecmp(args[0], "start") == 0)
	{
		system("xmms &");
	}
	else if(strcasecmp(args[0], "vers") == 0 || strcasecmp(args[0], "version") == 0)
	{
		write_conversation(conv, XMMS_PLUGIN_VERSION);
	}
	else if(CheckXmmsRunning(conv))
	{
		if(strcasecmp(args[0], "play") == 0)
		{
			xmms_remote_play(xmms_session);
		}
		else if(strcasecmp(args[0], "stop") == 0)
		{
			xmms_remote_stop(xmms_session);
		}
		else if(strcasecmp(args[0], "pause") == 0)
		{
			xmms_remote_pause(xmms_session);
		}
		else if(strcasecmp(args[0], "next") == 0)
		{
			xmms_remote_playlist_next(xmms_session);
		}
		else if(strcasecmp(args[0], "prev") == 0 ||
					strcasecmp(args[0], "previous") == 0)
		{
			xmms_remote_playlist_prev(xmms_session);
		}
		else if(strcasecmp(args[0], "quit") == 0)
		{
			xmms_remote_quit(xmms_session);
		}
		else if(strcasecmp(args[0], "mute") == 0)
		{
			xmms_remote_set_volume(xmms_session, 0, 0);
		}
		else
		{
			write_conversation(conv, "Error: command not found.");
		}

	}

	return GAIM_CMD_STATUS_OK;
}

gboolean XmmsDoubleParam(GaimConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data)
{
	if(strcasecmp(args[0], "start") == 0)
	{
	 	char *command = malloc(strlen(args[1])+3);
        sprintf(command, "%s &", args[1]);
        system(command);
		free(command);
	}
	else if(CheckXmmsRunning(conv))
	{
		if(strcasecmp(args[0], "go") == 0)
		{
			int pos = atoi(args[1]);

			if(pos < 1 || pos > xmms_remote_get_playlist_length(xmms_session))
				write_conversation(conv, "Please specify a valid position.");
			else
				xmms_remote_set_playlist_pos(xmms_session, pos - 1);
		}
		else if(strcasecmp(args[0], "vol") == 0 || strcasecmp(args[0], "volums") == 0)
		{
			int vol = atoi(args[1]);

			xmms_remote_set_volume(xmms_session, vol, vol);
		}
		else
		{
			write_conversation(conv, "Error: Command not found.");
		}
	}


	return GAIM_CMD_STATUS_OK;
}

static gboolean plugin_load(GaimPlugin *plugin)
{
	noargcmd = gaim_cmd_register("xmms", "", GAIM_CMD_P_DEFAULT, GAIM_CMD_FLAG_IM, NULL, (GaimCmdFunc)XmmsNoParams, "/xmms", NULL);
	oneargcmd = gaim_cmd_register("xmms", "w", GAIM_CMD_P_DEFAULT, GAIM_CMD_FLAG_IM, NULL, (GaimCmdFunc)XmmsSingleParam, "/xmms", NULL);
	twoargcmd = gaim_cmd_register("xmms", "ww", GAIM_CMD_P_DEFAULT, GAIM_CMD_FLAG_IM, NULL, (GaimCmdFunc)XmmsDoubleParam, "/xmms", NULL);
	
	return TRUE;
}

static gboolean plugin_unload(GaimPlugin *plugin)
{
	gaim_cmd_unregister (noargcmd);
	gaim_cmd_unregister (oneargcmd);
	gaim_cmd_unregister (twoargcmd);

	return TRUE;
}

static GaimPluginInfo info =
{
	GAIM_PLUGIN_MAGIC,
	GAIM_MAJOR_VERSION,
	GAIM_MINOR_VERSION,
	GAIM_PLUGIN_STANDARD,				 /**< type	   */
	NULL,						 /**< ui_requirement */
	0,						/**< flags	  */
	NULL,						 /**< dependencies   */
	GAIM_PRIORITY_DEFAULT,				/**< priority	   */
	NULL,						 /**< id		 */
	N_("XMMS-Plugin"),				/**< name	   */
	VERSION,					  /**< version	*/
							  /**  summary	*/
	N_("Does various XMMS-Related chores."),
							  /**  description	*/
	N_("Does various XMMS-Related chores."),
	"Ron <iago@valhallalegends.com>",		 /**< author	 */
	"http://javaop.clan-e1.net",			  /**< homepage	   */

	plugin_load,					  /**< load	   */
	plugin_unload,					/**< unload	 */
	NULL,						 /**< destroy	*/

	NULL,						 /**< ui_info	*/
	NULL,						 /**< extra_info	 */
	NULL,
	NULL
};


static void init_plugin(GaimPlugin *plugin)
{
}

GAIM_INIT_PLUGIN(XMMSPlugin, init_plugin, info)


