#include <FAME/FAME.h>
#include <stdlib.h>
#include <string.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <ctype.h>
#include "global_defines.h"
#include "struct_ex.h"
#include <proto/utility.h>
#include <clib/utility_protos.h>
#include "proto.h"
#include "struct_ex.h"

BOOL 	Read_Caller(void);
long 	makeallsec(char *,char *);	/* Uebergabe : HH:MM:SS */

#define X2  45
#define V90	46

/*
 *  This function parses the clog of the last user and collect all important
 *  values.
 */

BOOL Read_Caller(void)
	{
	char	buf[200],temp[80],workbuf[200],*token=NULL;
	BOOL	local=FALSE,broker=TRUE;
	int	i=0;

	STATIC char *cps[47]={" 1.2", " 2.4", " 4.8", " 7.2", " 9.6", "12.0", "14.4", "16.8",
   	             				"19.2", "21.6", "24.0", "26.4", "28.8", "31.2", "33.6", "36.0",
								 				"TNET", "40.8", "43.2", "45.4", "48.0", "50.4", "52.0", "55.2",
       	         				"57.6", "60.0", "62.4", "ISDN", "64.8", "67.2", "69.6", "72.0",
								 				"74.4", "76.8", "79.2", "81.6", "84.0", "86.4", "88.8", "91.2",
           	     				"93.6", "96.0", "98.4", "115K", "230K",	" X2 ",	"V.90"};

 	if(!(FGets(loc,buf,199))) return(FALSE);
	if((FAMEStrStr(buf,"LOCAL"))||(FAMEStrStr(buf,"SYSOP"))) local=TRUE;
  if(!(FGets(loc,buf,199))) return(FALSE);
	strcpy(workbuf,buf);		// Wegen Strtok
	if(!(token=strtok(workbuf,":"))) return(FALSE);
	while(broker==TRUE)
		{
		if(!(Stricmp(token,"SYSPWFAIL"))||(!(Stricmp(token,"USER PASSWORD FAIL"))))
			{
			if(!(FGets(loc,buf,199))) return(FALSE);
			strcpy(workbuf,buf);
      if(!(token=strtok(workbuf,":"))) return(FALSE);
			lc.Flags[0]='H';
			continue;
			}
		else broker=FALSE;
		};
	if(!(Stricmp(token,"BEGINNING AS NEWUSER AT")))
   	{
		if(!(FGets(loc,buf,199))) return(FALSE);
		strcpy(workbuf,buf);
		if(!(token=strtok(workbuf,":"))) return(FALSE);
		lc.NewUser=TRUE;
		lcheader.TodayNew++;
		};
	if(Stricmp(token,"ONDATE")) return(FALSE);
	FAMEStrMid(buf,ondate,9,8);
	FAMEStrMid(buf,lc.Conntime,26,8);
	lc.Conntime[8]='\0';
	FAMEStrMid(buf,temp,45,10);
	temp[strlen(temp)-1]='\0';
	if(MyAtol(temp)==1) { Print("\nLast Caller was the Sysop, updating aborted !\n\n");return(FALSE);};
	if(!(FGets(loc,buf,199))) return(FALSE); 		// HANDLE & LOCATION
	if(!(Strnicmp(buf,"ONDATEY2K:",10)))
		{
		if(!(FGets(loc,buf,199))) return(FALSE); 	// Found Y2K FAME, skip this ;)
		}
  token=NULL;
	if(!(token=strtok(buf," "))) return(FALSE);
  *temp='\0';
	while(1)
		{
   	if(!(token=strtok(NULL," "))) return(FALSE);
		if(!(Stricmp(token,"LOCATION:"))) break;
		strcat(temp,token);
		strcat(temp," ");
    }
  strncpy(lc.Handle,temp,16);
	lc.Handle[strlen(lc.Handle)-1]='\0';
	*temp='\0';
	if(!(Stricmp(lc.Handle,"FAME"))) return(FALSE);	// FUER STRIDER ! :)
	while(token=strtok(NULL," ")) {strcat(temp,token);strcat(temp," ");};
	temp[strlen(temp)-2]='\0';
	strncpy(lc.Location,temp,20);
 	if(!(FGets(loc,buf,199))) return(FALSE);
	*temp='\0';
  if(telnet==TRUE) strcpy(lc.Baud,"INET");
	else if(local==TRUE) strcpy(lc.Baud,"LOCL");
	else
		{
		if(!(token=strtok(buf," "))) return(FALSE);
    for(i=0;i<2;i++) if(!(token=strtok(NULL," "))) return(FALSE);
		strcpy(temp,token);
		while(temp[i++]!='/');
   	temp[i-1]='\0';
		switch(MyAtol(temp))
			{
			case  1200:		i=0;
										break;
			case  2400:   i=1;
										break;
			case  4800:		i=2;
										break;
			case  7200:		i=3;
										break;
			case  9600:		i=4;
										break;
			case 12000:		i=5;
										break;
			case 14400:		i=6;
										break;
			case 16800:		i=7;
										break;
			case 19200:   i=8;
										break;
			case 21600:		i=9;
										break;
			case 24000:		i=10;
										break;
			case 26400:		i=11;
										break;
			case 28000:		i=V90;			// V.90 Connect String (V0.61)
                    break;
			case 28800:		i=12;
										break;
			case 29333:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 30666:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 31200:		i=13;
										break;
			case 32000:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 33333:		i=45;				// X2 Connect String (V0.60)
										break;
			case 33600:		i=14;
										break;
			case 34666:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 36000:		i=X2;
										break;
      case 37333:   i=X2;				// X2 Connect String (V0.60)
										break;
			case 38400:		i=16;
										break;
			case 38666:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 40000:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 40800:		i=17;
										break;
			case 41333:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 42666:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 43200:		i=18;
										break;
			case 44000:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 45333:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 45600:		i=19;
										break;
			case 46666:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 48000:		i=20;
										break;
			case 49333:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 50400:		i=21;
										break;
			case 50666:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 52000:		i=22;
										break;
			case 53333:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 54666:		i=V90;			// V.90 Connect String (V0.61)
										break;
      case 55200:		i=23;
										break;
			case 56000:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 57333:		i=V90;			// V.90 Connect String (V0.61)
										break;
			case 57600:		i=24;
										break;
			case 60000:		i=25;
										break;
			case 62400:		i=26;
										break;
      case 64000:		i=27;				// ISDN!
										break;
			case 64800:		i=28;
										break;
			case 67200:		i=29;
										break;
      case 69600:		i=30;
										break;
			case 72000:		i=31;
										break;
      case 74400:		i=32;
										break;
			case 76800:		i=33;
										break;
			case 79200:		i=34;
										break;
			case 81600:		i=35;
										break;
			case 84000:		i=36;
										break;
			case 86400:		i=37;
										break;
			case 88800:		i=38;
										break;
			case 91200:		i=39;
										break;
			case 93600:		i=40;
										break;
			case 96000:		i=41;
										break;
			case 98400:		i=42;
										break;
      case 115200:  i=43;
										break;
			case 230400:	i=44;
                    break;
			default:			i=27;				// Defaults to ISDN
			};
		strcpy(lc.Baud,cps[i]);
		};
	if(!(FGets(loc,buf,199))) return(FALSE);
	strcpy(workbuf,buf);
	if(!(token=strtok(workbuf,":"))) return(FALSE);
	if(!(Stricmp(token,"SYSPWFAIL"))||(!(Stricmp(token,"USER PASSWORD FAIL")))) lc.Flags[0]='H';
  return(TRUE);
  }

/*
 *  Calculates the difference between start and endtime
 */

long makeallsec(char *start,char *end)	/* Uebergabe : HH:MM:SS */
	{
	char 	puffer[4];
	register long	s=0,z=0;
	BOOL	t=FALSE;

	FAMEStrMid(start,puffer,1,2);
	puffer[3]='\0';
	if(!(s=MyAtol(puffer))) t=TRUE;
	s=(s*60)*60;											// Stunden in Sek.
	FAMEStrMid(start,puffer,4,2);
	s=((MyAtol(puffer))*60)+s;				// Minuten in Sek.
	FAMEStrMid(start,puffer,7,2);
	s=s+MyAtol(puffer);
	FAMEStrMid(end,puffer,1,2);
	puffer[3]='\0';
	z=MyAtol(puffer);
	if(!z&&t==FALSE) z=24;						// Wenn Ziel=00 dann auf 24 Uhr aendern!
	z=(z*60)*60;
	FAMEStrMid(end,puffer,4,2);
	z=((MyAtol(puffer))*60)+z;
	FAMEStrMid(end,puffer,7,2);
	z=z+MyAtol(puffer);
	if(z>s)	return((z-s)/60);
	else return(((86399-s)+z)/60);
	}
