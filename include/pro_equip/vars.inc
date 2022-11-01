#if defined _pro_equip_vars_included
    #endinput
#endif
#define _pro_equip_vars_included

// This is not a config file, don't edit it.



// These arrays list the weapons available for the game (in no particular order)
// (only supports CSS)

char primaries[][] = { 
    "weapon_m4a1",
    "weapon_ak47",
    "weapon_aug",
    "weapon_galil",
    "weapon_famas",
    "weapon_m249",
    "weapon_awp",
    "weapon_scout",
    "weapon_sg552",
    "weapon_sg550",
    "weapon_g3sg1",
    "weapon_p90",
    "weapon_mac10",
    "weapon_tmp",
    "weapon_mp5navy",
    "weapon_ump45",
    "weapon_xm1014",
    "weapon_m3"
};

char secondaries[][] = { 
    "weapon_deagle",
    "weapon_p228",
    "weapon_usp",
    "weapon_glock",
    "weapon_fiveseven",
    "weapon_elite"
};

// WARNING: modifying the order of the grenades will break the UpdateUserWeapons() native and anything calling `give_grenades_delayed()`
char grenades[][] = {
    "weapon_hegrenade",
    "weapon_flashbang",
    "weapon_smokegrenade"
};

// When an invalid weapon is entered into a command display this message listing available weapons
char valid_weapons_msg[][] = {
    "ak47, aug, awp, c4, deagle, elite, famas, fiveseven, flashbang, g3sg1, galil, glock, hegrenade, knife, m249, m3, m4a1, mac10, mp5navy, p228, p90, scout, sg550, sg552, smokegrenade,",
    "tmp, ump45, usp, xm1014"
};

// Knife slot has only one item, the knife
int weapons_in_slot[] = {sizeof(primaries), sizeof(secondaries), 1, sizeof(grenades)};

// Given a weapon class name, look up what slot a weapon is assigned
StringMap weapon_slots;

// Given a weapon class name, look up what index a weapon is assigned in its corresponding slot's array above
StringMap weapon_indexes; // these refer to indexes in the appropriate slot array.  NOTE: these will not be unique, they are only unique for weapons of the same slot

// Should weapons be given out?  Can be set with the ProEquip_DisableWeapons/EnableWeapons natives
bool give_weapons;

// This is a WIP but intended for modifying weapon preferences and then reverting, like for fun rounds
StringMap snapshots;

bool late_loaded;

// Is the ProNightvision plugin running?
bool nv_loaded;

// Can't remember why I was tracking respawn time...
// ConVar cv_respawn_time;
// float respawn_time;

// For the HP command
int prop_iHealth;

// Path of the current log file
char EquipLogFile[PLATFORM_MAX_PATH];
char log_file_date[16];

Database DB;
