#include <unistd.h> 
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <xmms/xmmsctrl.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned int xmms_session = 0;
#define MAX_LENGTH 1024

int write_file();

int main(int argc, char *argv[])
{
	while(1)
	{
		if(write_file())
		{
			system("scp xmms.html darkside:public_html");
		}
		sleep(1);
	}

	return 0;
}

int write_file()
{
	char result[MAX_LENGTH];
	if(xmms_remote_is_running(xmms_session))
	{
		int position = xmms_remote_get_playlist_pos(xmms_session);
		int time = xmms_remote_get_output_time(xmms_session);
		int length = xmms_remote_get_playlist_time(xmms_session, position);
		char *title = xmms_remote_get_playlist_title(xmms_session, position);

		int rate = 0;
		int freq = 0;
		int nch = 0;

		xmms_remote_get_info(xmms_session, &rate, &freq, &nch);

		snprintf(result, MAX_LENGTH, "Now playing: %d. %s (%d:%02d / %d:%02d %dkbps)\n", 
				position, 
				title, 
				time / 60000, 
				(time % 60000) / 1000, 
				length / 60000, 
				(length % 60000) / 1000,
				rate / 1000);
	}
	else
	{
		strncpy(result, "Xmms isn't running.\n", MAX_LENGTH);
	}

	FILE * out = fopen("xmms.html", "w");

	if(out == NULL)
	{
		fprintf(stderr, "Error opening file for writing.\n");
		return FALSE;
	}

	fprintf(out, "<html><head>\n");
	fprintf(out, "<title>iago's Xmms Playing Thing!</title>");
	fprintf(out, "<META HTTP-EQUIV=\"Refresh\" CONTENT=\"10; URL=xmms.html\">");
	fprintf(out, "</head>");
	fprintf(out, "<body>");
	fprintf(out, "%s", result);
	fprintf(out, "</body>");
	fprintf(out, "</html>");

	fclose(out);

	return TRUE;
}
