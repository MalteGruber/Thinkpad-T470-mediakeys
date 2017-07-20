/* Malte Gruber 2017 */

#include <stdio.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

//Needed for this to run: apt-get install libxtst-dev
#include <X11/extensions/XTest.h>

#define LINE_BUFFER_SIZE 1024
#define START_HOLDOFF_SEC 2

#define SIMPLE_MAP 0
#define EXE_SCRIPT 1

#define MEDIA_NEXT 171
#define MEDIA_PAUSE 172
#define MEDIA_PREV 173

typedef struct {
	int from;
	int action;
	int to;
	int scriptName;

} custom_keymapping_t;

/*Define mappings here, make sure to specify the number of maps.*/
int numberOfMaps=3;
custom_keymapping_t maps[]={
		{.from=1311,.to=MEDIA_NEXT,.action=SIMPLE_MAP,.scriptName=NULL},
		{.from=1314,.to=MEDIA_PREV,.action=SIMPLE_MAP,.scriptName=NULL},
		{.from=1315,.to=MEDIA_PAUSE,.action=SIMPLE_MAP,.scriptName=NULL}
};


void clickHandler(int click){
	int keycode=40;

	Display *display=XOpenDisplay(0);

	for(int i=0;i<numberOfMaps;i++){
		if(maps[i].from==click){
			switch(maps[i].action){
			case SIMPLE_MAP:
				/*Simulate the key press of the mapped key*/
				keycode=maps[i].to;
				printf("Remapping from %d to %d\n",maps[i].from,maps[i].to);
				XTestFakeKeyEvent(display, keycode, True, 0);
				XTestFakeKeyEvent(display, keycode, False, 0);
				XFlush(display);
				break;

			case EXE_SCRIPT:
				printf("TODO!\n");
				break;
			}
		}
	}

}

void parseLine(char *line){
	/*Find the message about the keys*/
	char *result=strstr(line, "unhandled HKEY event");

	if(result){
		result[23+4]='\0';
		clickHandler(atoi(result+23));
	}
}

void main(int *args){

	printf("Starting thinkpad T470 multimedia key scanner\n");

	unsigned long startTime=(unsigned long)time(NULL);

	FILE *file;
	file=fopen("/dev/kmsg", "r");

	int c;
	char line[LINE_BUFFER_SIZE];
	char *linePtr;

	if(file){
		int readIsGo=0;
		while((c=getc(file))!=EOF){


			/*This ensures that we only read new lines from the file*/
			if(!readIsGo){
				if(((unsigned long)time(NULL)-startTime)>START_HOLDOFF_SEC){
					readIsGo=1;
				}else
					continue;
			}


			/*Extract lines from the incoming bytes*/
			if(c=='\n'){
				*linePtr++='\0';
				parseLine(line);
				linePtr=line;
			}else if((line-linePtr)<LINE_BUFFER_SIZE-1){
				*linePtr++=c;
			}
		}
		fclose(file);
	}

}