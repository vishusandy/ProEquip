#if defined _pro_equip_constants_included
    #endinput
#endif
#define _pro_equip_constants_included

#define DEBUGGING false

// do not add a file extension, a date and .log extension will be appeneded
#define EQUIP_LOG_FILE "logs/pro-equip"

// Admin flag required to use the admin commands
#define ADM_LEVEL ADMFLAG_BAN




// Defaults - only applied when a config file is not read
#define DEFAULT_HP 100
#define DEFAULT_ARMOR 0
#define DEFAULT_KNIFE 1
#define DEFAULT_HEGRENADES 1
#define DEFAULT_FLASHBANGS 0
#define DEFAULT_SMOKEGRENADES 0
#define DEFAULT_NIGHTVISION true
#define DEFAULT_DEFUSEKIT true
#define DEFAULT_SILENCERS true

#define MAX_GRENADES 64

#define MENU_DISPLAY_TIME 20

// Admin command settings
#define MAX_ARGS 32
#define COMMAND_ERROR_COLOR 0xFF4500
#define EXAMPLE_COLOR 0xE0E0E0
#define HELP_COLOR_1 0xD0D0D0
#define HELP_COLOR_2 0xFFFFFF
#define HELP_COLOR_3 0xB0B0B0


// If the number of available weapons is less than or equal to this a compact menu will be
// displayed listing only available weapons instead of greying out invalid ones
#define COMPACT_MENU_THRESHOLD 7



// ---------------------------
// Don't modify anything below
// ---------------------------


#define ReservedMenuSlots 2

// RANDOM_WEAPON_AVAILABLE should always be larger than RANDOM_WEAPON_ALL
#define RANDOM_WEAPON_AVAILABLE -1
#define RANDOM_WEAPON_ALL -2
#define WEAPON_NONE -3
#define WEAPON_RESET -4
#define RANDOM_WEAPON_AVAILABLE_STRING "-1"

#define EQUIP_PLAYERS -1
#define EQUIP_BOTS -2
#define EQUIP_BOTH -3

#define NO_OVERRIDE -1

#define INFINITE_AMMO -2

#define WEAPON_PREF_FAIL -255
#define WEAPON_LOOKUP_FAIL -1

enum showMenu {
    ShowMenu_None,
    ShowMenu_Main,
    ShowMenu_Primary,
    ShowMenu_Secondary,
}

