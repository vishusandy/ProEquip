#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <cstrike>

#include <pro_equip/constants.inc>
#include <pro_equip/vars.inc>
#include <pro_equip/db.inc>
#include <pro_equip/menu_extras.inc>
#include <pro_equip/prefs.inc>
#include <pro_equip/available_weapons.inc>
#include <pro_equip/equip_pref.inc>
#include <pro_equip/client_data.inc>
#include <pro_equip/weapons.inc>
#include <pro_equip/menus.inc>
#include <pro_equip/dm_config.inc>
#include <pro_equip/snapshots.inc>

#include <pro_equip/natives.inc>
#include <pro_equip/hp_cmd.inc>
#include <pro_equip/nv_cmd.inc>
#include <pro_equip/give_cmd.inc>
#include <pro_equip/equip_cmd.inc>

#include <pro_equip/ProEquip.inc>

public Plugin myinfo = { name = "Pro Equip", author = "Vishus", description = "Equipment and weapons", version = "0.1.2", url = "https://github.com/vishusandy/ProEquip" };


public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max) {
    late_loaded = late;
    RegPluginLibrary("pro_equip");
    CreateNative("ProEquip_ResetEquip", Native_ResetEquip);
    CreateNative("ProEquip_ResetEquipAll", Native_ResetEquipAll);
    CreateNative("ProEquip_ResetWeaponPrefs", Native_ResetWeaponPrefs);
    CreateNative("ProEquip_UpdateUserWeapons", Native_UpdateUserWeapons);
    CreateNative("ProEquip_ResetGrenadeOverrides", Native_ResetGrenadeOverrides);
    CreateNative("ProEquip_OverrideGrenades", Native_OverrideGrenades);
    CreateNative("ProEquip_SetGrenade", Native_SetGrenade);
    CreateNative("ProEquip_GetGrenadeQuantity", Native_GetGrenadeQuantity);
    CreateNative("ProEquip_GetCurGrenadeQuantity", Native_GetCurGrenadeQuantity);
    CreateNative("ProEquip_GiveGrenade", Native_GiveGrenade);
    CreateNative("ProEquip_SetKnife", Native_SetKnife);
    CreateNative("ProEquip_SetEquipHP", Native_SetEquipHP);
    CreateNative("ProEquip_SetEquipArmor", Native_SetEquipArmor);
    CreateNative("ProEquip_SetEquipNightvision", Native_SetEquipNightvision);
    CreateNative("ProEquip_DisplayPlayerNightvision", Native_DisplayPlayerNightvision);
    CreateNative("ProEquip_SetPlayerNightvisionOn", Native_SetPlayerNightvisionOn);
    CreateNative("ProEquip_SetEquipDefuseKit", Native_SetEquipDefuseKit);
    CreateNative("ProEquip_EnablePrimaryMenu", Native_EnablePrimaryMenu);
    CreateNative("ProEquip_DisablePrimaryMenu", Native_DisablePrimaryMenu);
    CreateNative("ProEquip_EnableSecondaryMenu", Native_EnableSecondaryMenu);
    CreateNative("ProEquip_DisableSecondaryMenu", Native_DisableSecondaryMenu);
    CreateNative("ProEquip_SetAvailableWeapon", Native_SetAvailableWeapon);
    CreateNative("ProEquip_EnableWeapons", Native_EnableWeapons);
    CreateNative("ProEquip_DisableWeapons", Native_DisableWeapons);
    CreateNative("ProEquip_WeaponsEnabled", Native_WeaponsEnabled);
    CreateNative("ProEquip_ReloadConfigs", Native_ReloadConfigs);
    CreateNative("ProEquip_SetMenuState", Native_SetMenuState);
    CreateNative("ProEquip_SetNextMenuState", Native_SetNextMenuState);
    CreateNative("ProEquip_SetPrevMenuState", Native_SetPrevMenuState);
    CreateNative("ProEquip_ResetSlotPreference", Native_ResetSlotPreference);
    CreateNative("ProEquip_ResetWeaponSlotAvailability", Native_ResetWeaponSlotAvailability);
    CreateNative("ProEquip_SetGunPreference", Native_SetGunPreference);
    CreateNative("ProEquip_SetSlotPreference", Native_SetSlotPreference);
    CreateNative("ProEquip_GetSlotPreference", Native_GetSlotPreference);
    CreateNative("ProEquip_GetWeaponIndex", Native_GetWeaponIndex);
    CreateNative("ProEquip_GetWeaponSlot", Native_GetWeaponSlot);
    CreateNative("ProEquip_FirstAvailableWeapon", Native_FirstAvailableWeapon);
    CreateNative("ProEquip_DisplayMainMenu", Native_DisplayMainMenu);
    CreateNative("ProEquip_DisplayPrimaryMenu", Native_DisplayPrimaryMenu);
    CreateNative("ProEquip_DisplaySecondaryMenu", Native_DisplaySecondaryMenu);
    CreateNative("ProEquip_SetSlotAmmo", Native_SetSlotAmmo);
    CreateNative("ProEquip_HasAnyInfiniteGrenades", Native_HasAnyInfiniteGrenades);
    CreateNative("ProEquip_CreateSnapshot", Native_CreateSnapshot);
    CreateNative("ProEquip_DeleteSnapshot", Native_DeleteSnapshot);
    CreateNative("ProEquip_RestoreSnapshot", Native_RestoreSnapshot);
    CreateNative("ProEquip_RestoreDeleteSnapshot", Native_RestoreDeleteSnapshot);
    return APLRes_Success;
}


public void OnPluginStart() {
    SetLogFile();
    give_weapons = true;
    weapon_menu.reset();
    weapon_indexes = new StringMap();
    weapon_slots = new StringMap();
    menu_extras = new ArrayList(sizeof(MenuExtra));
    
    Database.Connect(DbConnCallback, "pro_menu");
    
    initialize_weapon_array();
    initialize_weapon_hashmaps();
    initialize_equip_defaults();
    reset_client_data();
    load_menu_extras();
    
    hp_plugin_start();
    nv_plugin_start();
    give_plugin_start();
    equip_plugin_start();
    snapshots_plugin_start();
    
    HookEvent("weapon_fire", EventWeaponFire);
    HookEvent("player_connect", EventPlayerConnect);
    HookEvent("player_disconnect", EventPlayerDisconnect);
    HookEvent("player_spawn", EventPlayerSpawn);
    HookEvent("player_team", EventTeamChange);
    
    RegConsoleCmd("menu", MainMenuCommand);
    RegConsoleCmd("weapons", MainMenuCommand);
    RegConsoleCmd("guns", MainMenuCommand);
    RegConsoleCmd("rifles", PrimaryMenuCommand);
    RegConsoleCmd("pistols", SecondaryMenuCommand);
    RegConsoleCmd("say", CommandSay);
    RegConsoleCmd("say_team", CommandSay);
    
    RegAdminCmd("dbg_equip", DbgEquip, ADMFLAG_BAN);
    
    // cv_respawn_time = FindConVar("cssdm_respawn_wait");
    // HookConVarChange(cv_respawn_time, respawn_time_changed);
    // respawn_time = (cv_respawn_time != INVALID_HANDLE)? GetConVarFloat(cv_respawn_time): 0.0;
    
    if(late_loaded) {
        for(int i=1; i < MaxClients; i++) {
            if(IsClientInGame(i)) {
                SDKHook(i, SDKHook_WeaponEquip, HookOnWeaponEquip);
            }
        }
        late_loaded = false;
    }
    
}

public void SetLogFile() {
    char date[sizeof(log_file_date)];
    FormatTime(date, sizeof(date), "%Y-%m-%d");
    
    char file_buffer[PLATFORM_MAX_PATH];
    FormatEx(EquipLogFile, sizeof(EquipLogFile), "%s_%s.log", EQUIP_LOG_FILE, date);
    BuildPath(Path_SM, file_buffer, PLATFORM_MAX_PATH, EquipLogFile);
    EquipLogFile = file_buffer;
    
    if(!StrEqual(date, log_file_date)) {
        strcopy(log_file_date, sizeof(log_file_date), date);
        LogToFile(EquipLogFile, "Equip log file: %s", EquipLogFile);
    }
}


public Action DbgEquip(int client, int args) {
    LogToFileEx(EquipLogFile, "Player Equip");
    player_equip.debug(EQUIP_PLAYERS);
    LogToFileEx(EquipLogFile, "Bot Equip");
    bot_equip.debug(EQUIP_BOTS);
    LogToFileEx(EquipLogFile, "Player Weapons");
    player_weapons.debug();
    LogToFileEx(EquipLogFile, "Bot Weapons");
    bot_weapons.debug();
    LogToFileEx(EquipLogFile, "\nPLAYER DATA");
    for (int i=1; i < MaxClients; i++) {
        if (IsClientInGame(i)) {
            player_data[i].debug(i);
        }
    }
    ReplyToCommand(client, "Dumped equipment data to log");
}

public void OnPluginEnd() {
    for(int i=1; i < MaxClients; i++) {
        if(IsClientInGame(i)) {
            SDKUnhook(i, SDKHook_WeaponEquip, HookOnWeaponEquip);
        }
    }
}

public void OnAllPluginsLoaded() {
    nv_loaded = LibraryExists("pro_nightvision");
}





public void OnConfigsExecuted() {
    load_map_configs();
    initial_weapon_prefs();
}

public void OnMapStart() {
    SetLogFile();
    weapon_menu.weapon_menu_enabled = true;
    for(int i=1; i < MaxClients; i++) {
        if(!IsClientInGame(i) || IsFakeClient(i)) {
            player_data[i].reset(-1, -1);
        } else {
            player_data[i].reset();
        }
    }
}




public Action EventPlayerSpawn(Handle event, const char[] name, bool dontBroadcast) {
    int client = GetClientOfUserId(GetEventInt(event, "userid"));
    if(!IsClientInGame(client)) {
        return;
    }
    if(give_weapons) {
        strip_all_weapons(client);
    }
    equip_client(client);
    if(!IsFakeClient(client)) {
        player_data[client].last_silencer_slot = -1;
        if(player_data[client].menu_state.show_menu != ShowMenu_None) {
            give_guns_pref(client);
            CreateTimer(0.1, TimerDelayedMenu, client);
        } else {
            give_guns_pref(client);
        }
    } else {
        give_guns_pref(client);
    }
}

public Action EventWeaponFire(Handle event, const char[] name, bool dontBroadcast) {
    int client = GetClientOfUserId(GetEventInt(event, "userid"));
    if(!player_data[client].has_any_infinite_ammo()) {
        return Plugin_Continue;
    }
    char weapon[32];
    char weapon_buff[32] = "weapon_";
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    strcopy(weapon_buff[7], sizeof(weapon_buff)-7, weapon);
    int weapon_id = -1;
    int slot = -1;
    if(!weapon_indexes.GetValue(weapon_buff, weapon_id) || !weapon_slots.GetValue(weapon_buff, slot)) {
        return Plugin_Continue;
    }
    DataPack pack;
    CreateDataTimer(0.9, TimerInfiniteAmmo, pack, TIMER_DATA_HNDL_CLOSE);
    pack.WriteCell(client);
    pack.WriteCell(slot);
    pack.WriteCell(weapon_id);
    return Plugin_Continue;
}

public Action TimerInfiniteAmmo(Handle timer, DataPack pack) {
    pack.Reset();
    int client = pack.ReadCell();
    if(!IsClientConnected(client) || !IsPlayerAlive(client)) {
        return Plugin_Continue;
    }
    int slot = pack.ReadCell();
    int weapon_id = pack.ReadCell();
    switch(slot) {
        case 0:
            player_data[client].fire_infinite_ammo(client, slot, primaries[weapon_id]);
        case 1: 
            player_data[client].fire_infinite_ammo(client, slot, secondaries[weapon_id]);
        case 3:
            player_data[client].fire_infinite_ammo(client, slot, grenades[weapon_id]);
    }
    return Plugin_Continue;
}

public Action EventTeamChange(Handle event, const char[] name, bool dontBroadcast) {
    int client = GetClientOfUserId(GetEventInt(event, "userid"));
    if(IsFakeClient(client)) {
        player_data[client].guns.rifle = RANDOM_WEAPON_AVAILABLE;
        player_data[client].guns.pistol = RANDOM_WEAPON_AVAILABLE;
        player_data[client].equip.knife = bot_equip.knife;
        return;
    } else {
        if( (player_weapons.num_primaries < 2 && player_weapons.num_secondaries < 2)
            || !weapon_menu.weapon_menu_enabled 
            || (!weapon_menu.primary_menu_enabled && !weapon_menu.secondary_menu_enabled)
            || !player_data[client].menu.weapon_menu_enabled
            || (!player_data[client].menu.primary_menu_enabled && !player_data[client].menu.secondary_menu_enabled)
        ) {
            player_data[client].menu_state.show_menu = ShowMenu_None;
            return;
        }
        if(GetEventInt(event, "oldteam") <= CS_TEAM_SPECTATOR && GetEventInt(event, "team") >= CS_TEAM_T) {
            player_data[client].menu_state.show_menu = ShowMenu_Main;
        }
    }
}

public Action HookOnWeaponEquip(int client, int weapon) {
    char buffer[32];
    GetEdictClassname(weapon, buffer, sizeof(buffer));
    int slot = WEAPON_LOOKUP_FAIL;
    if(weapon_slots.GetValue(buffer, slot)) {
        if(strcmp(buffer, "weapon_m4a1") == 0 || strcmp(buffer, "weapon_usp") == 0) {
            SetEntProp(weapon, Prop_Send, "m_bSilencerOn", player_data[client].equip.silencers);
            SetEntProp(weapon, Prop_Send, "m_weaponMode", player_data[client].equip.silencers);
        }
    }
}


public Action EventPlayerConnect(Handle event, const char[] name, bool dontBroadcast) {
    int client = GetClientOfUserId(GetEventInt(event, "userid"));
    if(client <= 0) {
        return;
    }
    player_data[client].reset(WEAPON_NONE, WEAPON_NONE);
}

public Action EventPlayerDisconnect(Handle event, const char[] name, bool dontBroadcast) {
    int client = GetClientOfUserId(GetEventInt(event, "userid"));
    player_data[client].reset();
}

public void OnClientConnected(int client) {
    bool is_bot = IsFakeClient(client);
    player_data[client].guns.rifle = first_available_weapon(!is_bot, 0);
    player_data[client].guns.pistol = first_available_weapon(!is_bot, 1);
}

public void OnClientPutInServer(int client) {
    SDKHook(client, SDKHook_WeaponEquip, HookOnWeaponEquip);
}

public void OnClientDisconnect(int client) {
    SDKUnhook(client, SDKHook_WeaponEquip, HookOnWeaponEquip);
}

public Action CommandSay(int client, int args) {
    char buffer[64];
    GetCmdArg(1, buffer, sizeof(buffer));
    if(StrEqual(buffer, "guns", false) || StrEqual(buffer, "menu", false) || StrEqual(buffer, "weapons", false)) {
        return display_main_menu(client, args);
    } else if(StrEqual(buffer, "rifles", false) || StrEqual(buffer, "rifle", false)) {
        return display_primary_menu(client, args);
    } else if(StrEqual(buffer, "pistols", false) || StrEqual(buffer, "pistol", false)) {
        return display_secondary_menu(client, args);
    }
    return Plugin_Continue;
}


 // todo: use this to add a delay to the equipment menu when joining the server
// public void respawn_time_changed(ConVar convar, const char[] oldValue, const char[] newValue) {
//     respawn_time = StringToFloat(newValue);
// }
