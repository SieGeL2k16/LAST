
struct Library 			*UtilityBase=NULL;
struct FAMELibrary 	*FAMEBase=NULL;

// Structs & Variablen

char 	CPath[200],				// Pfadname -> Callerslog
			ondate[10], 			// Datum des Einloggens
			offdate[10],			// Datum des Ausloggens
			regstring[80];		// Register-String

long  anzusers=NULL;		// Anzahl der anzuzeigenden User
long	lczeit;

BOOL 	telnet=FALSE,			// New for V0.57, if set given Node is TelNet!
      logfile=FALSE;		// New for V0.59, if set LastCallers creates a logfile

BPTR	loc=NULL;					// Lock fuers Callerslog

struct LastCallers
	{
	char	Baud[8],				// BaudRate
				Handle[18],			// Handle des Users
				Location[22],		// Location des Users
				Conntime[11],		// Zeit des Connects
        Totaltime[11],	// Zeit die der User online war
				Flags[8];       // Flags, was der User gemacht hat
  long	Node;						// Node
	long	Ulbytes,        // Uploaded Bytes
				Dlbytes,				// Downloaded Bytes
				Ulfiles,        // Up &  downloaded Files
				Dlfiles;
  BOOL	NewUser;        // TRUE=NEWUSER,FALSE=NORMAL
	};

struct LastCallers lc;
struct LastCallers *last;

// Header-Struct fuer Datafile, danach beginnen die 20 eintraege

struct LCHeader
	{
	char	TodayDate[10];	// Aktuelles Datum
	LONG	BestUpBytes,		// Beste Uploaded bytes
				BestDlBytes,		// Beste Downloaded bytes
				TodayUpBytes,		// Bytes today uploaded
				TodayDlBytes,		// Bytes today downloaded
				TodayUpFiles,		// Files today uploaded
				TodayDlFiles,		// Files today downloaded
				TodayCalls,			// Calls today
				TodayPages,			// Pages today
				TodayLC,				// Lost Carriers
				TodayNew,				// NewUsers today
				TodayBusy,			// Anzahl Stunden Auslastung
				TodayHacks,			// Anzahl hacks today
				LastCalls,			//  -"-  Calls vom Vortag
				BestUpFiles,    // beste upload-files
				BestDlFiles,    // beste download-files
				BestBusy,				//   -"-   Stunden auslastung
				BestCalls;
	}lcheader;

// Record-Struct, wird nur bei Tages-Wechsel eingelesen und ueberprueft!

struct LCRecords
	{
	long 	BestCalls,
				BestPages,
				BestLC,					// Meisten LostCarriers
				BestNew,        //   -"-   Newusers
				BestHacks;			//   -"-   Hacks
	char  UpByteDate[10],	// Datum von Up-Bytes
				DlByteDate[10],	//  -"-  von Dl-Bytes
        UpFileDate[10],	//  -"-  von Up-Files
				DlFileDate[10],	//  -"-  von Dl-Bytes
        CallDate[10],		//  -"-  von Calls
				PageDate[10],		//  -"-  von Pages
				LCDate[10],			//  -"-  von Lost Carriers
				NewDate[10],		//  -"-  von NewUsern
				BusyDate[10],		//  -"-	 von Stunden Auslastung
				HackDate[10],		//  -"-  von Hacks
				ResetDate[10];	//  -"-  von diesem Data-File (Gestartet!)
	}lcrecords;
