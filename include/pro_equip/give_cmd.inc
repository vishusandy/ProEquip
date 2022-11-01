#if defined _pro_equip_give_cmd_included
    #endinput
#endif
#define _pro_equip_give_cmd_included

/*
    Abandon all hope ye who enter here (ye as in thou, not Kanye)
*/

#include <pro_equip/constants.inc>
#include <pro_equip/weapon_utils.inc>


void give_plugin_start() {
    RegServerCmd("sm_give_server", Command_GiveServer, "Give item to target (server command)");
    RegAdminCmd("sm_give", Command_Give, ADM_LEVEL, "Give item to target");
}

enum giveType {
    GiveType_None, // used for a specific weapon - todo: change this behavior
    GiveType_Weapon,
    GiveType_All, // only valid for GiveMode_Remove
    GiveType_Secondary, // currently only valid for GiveMode_Remove - removes secondary weapon
    GiveType_Primary, // currently only valid for GiveMode_Remove - removes primary weapon
    GiveType_Grenades, // currently only valid for GiveMode_Remove - removes all grenades
    GiveType_RandomGun,
    GiveType_RandomPrimary,
    GiveType_RandomSecondary,
    GiveType_RandomGrenade,
    GiveType_RandomAny, // same as RandomGun but includes grenades
}

enum giveMode {
    GiveMode_Add,
    GiveMode_Remove,
    GiveMode_Reset,
}

// only used with GiveMode_Add
enum struct GiveRemovals {
    bool primary;
    bool secondary;
    bool knife;
    bool grenades;
    bool slot(int s) {
        switch(s) {
            case 0: return this.primary;
            case 1: return this.secondary;
            case 2: return this.knife;
            case 3: return this.grenades;
            default: return false;
        }
    }
    void debug() {
        #if DEBUGGING
            LogToFileEx(EquipLogFile, "GiveRemovals: knife=%i secondary=%i primary=%i grenades=%i", this.knife, this.secondary, this.primary, this.grenades);
        #endif
    }
}

enum struct GiveOpts {
    giveType type;
    giveMode mode;
    GiveRemovals removals;
    int primary_pref;
    int secondary_pref;
    bool force_primary;
    bool force_secondary;
    int save;
    int quantity;
    char weapon[32];
    
    void debug() {
        #if DEBUGGING
            LogToFileEx(EquipLogFile, "GiveOpts: type=%i mode=%i primary_pref=%i secondary_pref=%i force_primary=%i force_secondary=%i save=%i weapon=%s", this.type, this.mode, this.primary_pref, this.secondary_pref, this.force_primary, this.force_secondary, this.save, this.weapon);
        #endif
        this.removals.debug();
    }
    
    bool show_secondary_menu() {
        if(this.mode == GiveMode_Add) {
            int slot = -1;
            if(this.type == GiveType_RandomGun || this.type == GiveType_RandomAny) {
                return false;
            } else if(weapon_slots.GetValue(this.weapon, slot) && slot == 1) {
                return false;
            }
        } else {
            switch(this.type) {
                case GiveType_All, GiveType_Secondary:
                    return false;
                default:
                    return true;
            }
        }
        return true;
    }
    
    bool show_primary_menu() {
        if(this.mode == GiveMode_Add) {
            int slot = -1;
            if(this.type == GiveType_RandomGun || this.type == GiveType_RandomAny) {
                return false;
            } else if(weapon_slots.GetValue(this.weapon, slot) && slot == 0) {
                return false;
            }
        } else if(this.mode == GiveMode_Remove) {
            switch(this.type) {
                case GiveType_All, GiveType_Primary:
                    return false;
                default:
                    return true;
            }
        }
        return true;
    }
}


public bool Give(int client, GiveOpts opts) {
    if(!IsClientInGame(client) || !IsPlayerAlive(client)) {
        return false;
    }
    
    
    #if DEBUGGING
        opts.debug();
    #endif
    
    if(opts.mode == GiveMode_Reset) {
        #if DEBUGGING
            LogToFileEx(EquipLogFile, "Reseting %N", client);
        #endif
        ProEquip_ResetEquip(client);
        CreateTimer(0.1, EquipLowestSlotDelayed, client);
        return true;
    }
    
    if(opts.save != 0) {
        if(opts.primary_pref != WEAPON_PREF_FAIL) {
            ProEquip_SetSlotPreference(client, 0, opts.primary_pref);
            if(opts.save == 2) {
                ProEquip_DisablePrimaryMenu(client);
            }
        }
        if(opts.secondary_pref != WEAPON_PREF_FAIL) {
            ProEquip_SetSlotPreference(client, 1, opts.secondary_pref);
            if(opts.save == 2) {
                ProEquip_DisableSecondaryMenu(client);
            }
        }
    }
    int slot = -1;
    switch (opts.type) {
        case GiveType_None: {
            if(StrEqual(opts.weapon, "weapon_knife")) {
                slot = 2;
                if(opts.save != 0) {
                    ProEquip_SetKnife(client, (opts.mode == GiveMode_Add)? true: false)
                }
            } else if(strlen(opts.weapon) > 0) {
                weapon_slots.GetValue(opts.weapon, slot);
            }
        }
        case GiveType_Primary: {
            slot = 0;
        }
        case GiveType_Secondary: {
            slot = 1;
        }
        case GiveType_Grenades: {
            slot = 3;
        }
        case GiveType_RandomGun: {
            slot = (GetURandomInt() % (sizeof(primaries) + sizeof(secondaries)) >= sizeof(primaries))? 1: 0;
            if(opts.mode == GiveMode_Add) {
                RandomWeaponAnyGun(opts.weapon, sizeof(opts.weapon), slot);
                if(opts.save != 0) {
                    ProEquip_SetSlotPreference(client, 0, RANDOM_WEAPON_AVAILABLE);
                    ProEquip_SetSlotPreference(client, 1, RANDOM_WEAPON_AVAILABLE);
                }
            }
        }
        case GiveType_RandomPrimary: {
            slot = 0;
            if(opts.mode == GiveMode_Add) {
                RandomPrimary(opts.weapon, sizeof(opts.weapon));
                if(opts.save != 0) {
                    ProEquip_SetSlotPreference(client, 0, RANDOM_WEAPON_AVAILABLE);
                }
            }
        }
        case GiveType_RandomSecondary: {
            slot = 1;
            RandomSecondary(opts.weapon, sizeof(opts.weapon));
            if(opts.save != 0) {
                ProEquip_SetSlotPreference(client, 1, RANDOM_WEAPON_AVAILABLE);
            }
        }
        case GiveType_RandomGrenade: {
            slot = 3;
            RandomGrenade(opts.weapon, sizeof(opts.weapon));
        }
        case GiveType_RandomAny: {
            slot = GetAnyExistingRandomSlot(client);
            switch(slot) {
                case 0:
                    RandomPrimary(opts.weapon, sizeof(opts.weapon));
                case 1:
                    RandomSecondary(opts.weapon, sizeof(opts.weapon));
                case 2:
                    opts.weapon = "weapon_knife";
                case 3: {
                    RandomGrenade(opts.weapon, sizeof(opts.weapon));
                }
            }
        }
    }
    
    if(opts.mode == GiveMode_Add) {
        return GiveProcessAdd(client, slot, opts);
    } else {
        return GiveProcessRemove(client, slot, opts);
    }
}



bool GiveProcessAdd(int client, int slot, GiveOpts opts) {
    bool affected = false;
    bool removals[4]; // store in array for compactness and ability to loop through removals
    char current_weapon[32];
    GetClientWeapon(client, current_weapon, sizeof(current_weapon));
    removals[0] = opts.removals.primary || slot == 0; // remove primary
    removals[1] = opts.removals.secondary || slot == 1; // remove secondary
    removals[2] = opts.removals.knife || slot == 2; // remove knife
    removals[3] = opts.removals.grenades; // don't remove grenades if slot == 3 because we just add a grenade if the weapon is a grenade
    
    #if DEBUGGING
        LogToFileEx(EquipLogFile, "GiveProcessAdd() - slot=%i save=%i", slot, opts.save);
        LogToFileEx(EquipLogFile, "GiveProcessAdd() - Removals[] = {%i, %i, %i, %i}", removals[0], removals[1], removals[2], removals[3]);
    #endif
    
    if(opts.save != 0) {
        if(removals[3]) {
            if(slot == 3 && !StrEqual(opts.weapon[7], "hegrenade")) {
                ProEquip_OverrideGrenades(client, "hegrenade", 0); // remove the specific grenade
            } else if(slot == 3 && ProEquip_GetCurGrenadeQuantity(client, "hegrenade") == 0) {
                ProEquip_GiveGrenade(client, "hegrenade", -1); // restore default number of the specific grenade
            }
            if(slot == 3 && !StrEqual(opts.weapon[7], "flashbang")) {
                ProEquip_OverrideGrenades(client, "flashbang", 0); // remove the specific grenade
            } else if(slot == 3 && ProEquip_GetCurGrenadeQuantity(client, "flashbang") == 0) {
                ProEquip_GiveGrenade(client, "flashbang", -1); // restore default number of the specific grenade
            }
            if(slot == 3 && !StrEqual(opts.weapon[7], "smokegrenade")) {
                ProEquip_OverrideGrenades(client, "smokegrenade", 0); // remove the specific grenade
            } else if(slot == 3 && ProEquip_GetCurGrenadeQuantity(client, "smokegrenade") == 0) {
                ProEquip_GiveGrenade(client, "smokegrenade", -1); // restore default number of the specific grenade
            }
        }
        
        if(slot == 2) {
            ProEquip_SetKnife(client, true);
        } else {
            ProEquip_SetKnife(client, !removals[2]);
        }
        if(removals[0] && slot != 0) {
            ProEquip_SetSlotPreference(client, 0, WEAPON_NONE);
        }
        if(removals[1] && slot != 1) {
            ProEquip_SetSlotPreference(client, 1, WEAPON_NONE);
        }
        if(opts.save == 2 && removals[0]) {
            ProEquip_DisablePrimaryMenu(client);
        }
        if(opts.save == 2 && removals[1]) {
            ProEquip_DisableSecondaryMenu(client);
        }
    }
    
    opts.debug();
    
    // process weapon removals
    if(RemoveSlotItems(client, removals)) {
        affected = true;
    }
    
    // process grenade removals
    if(removals[3]) {
        affected = RemoveAllGrenades(client) | affected;
    }
    
    // Handle individual grenades, also check for infinite grenade ammo (no non-grenade weapons)
    if(slot == 3 && opts.weapon[0] != '\0') {
        GivePlayerItem(client, opts.weapon);
        if(!has_weapon_slot(client, 0) && !has_weapon_slot(client, 1) && !has_weapon_slot(client, 2)) {
            if(!IsFakeClient(client)) {
                #if DEBUGGING
                    LogToFileEx(EquipLogFile, "!give - infinite ammo - setting %s to %i", opts.weapon, INFINITE_AMMO);
                #endif
            }
            ProEquip_SetGrenade(client, opts.weapon, INFINITE_AMMO);
            give_grenades(client, opts.weapon, 1, false);
        }
        ClientCommand(client, "slot%i", slot+1);
        affected = true;
    }
    // special case for GiveType_Grenades - give one of each grenade
    else if(opts.type == GiveType_Grenades) {
        for(int i=0; i < sizeof(grenades); i++) {
            GivePlayerItem(client, grenades[i]);
            affected = true;
        }
    } else if(slot != -1 && strlen(opts.weapon) > 0) {
        GivePlayerItem(client, opts.weapon);
        ClientCommand(client, "slot%i", slot+1);
        affected = true;
    } else {    // If the user isn't being given a weapon and their current weapon was removed then show an appropriate weapon
        int tmp = -1;
        weapon_slots.GetValue(current_weapon, tmp);
        if(tmp != -1 && removals[tmp]) {
            CreateTimer(0.1, EquipLowestSlotDelayed, client);
        }
    }
    return affected;
}


bool GiveProcessRemove(int client, int slot, GiveOpts opts) {
    if(opts.type == GiveType_All) {
        bool removals[4] = {true, true, true, true};
        return RemoveSlotItems(client, removals);
    }
    char current_weapon[32];
    int cur_slot = -1;
    GetClientWeapon(client, current_weapon, sizeof(current_weapon));
    weapon_slots.GetValue(current_weapon, cur_slot);
    int s = -1;
    if(strlen(opts.weapon) > 0 && !weapon_slots.GetValue(opts.weapon, s)) {
        return false;
    }
    if(slot == 3 && strlen(opts.weapon) > 0) {
        if(remove_specific_grenade(client, opts.weapon) != 0) {
            if(s == slot) {
                CreateTimer(0.1, EquipLowestSlotDelayed, client);
            }
            return true;
        } else {
            return false;
        }
    }
    bool removals[4];
    for(int i=0; i < sizeof(removals); i++) {
        removals[i] = opts.removals.slot(i);
    }
    removals[slot] = true;
    #if DEBUGGING
        LogToFileEx(EquipLogFile, "Processing removals: 0=%i 1=%i 2=%i 3=%i", removals[0], removals[1], removals[2], removals[3]);
    #endif
    // If --save/--force is specified knives will be removed earlier on and 
    // RemoveSlotItems will return false, so check for that
    if((RemoveSlotItems(client, removals) || !removals[0] & !removals[1] & removals[2] & !removals[3])) {
        #if DEBUGGING
            LogToFileEx(EquipLogFile, "RemoveSlotItems processed successfully");
        #endif
        if(s == slot || opts.removals.slot(cur_slot)) {
            CreateTimer(0.1, EquipLowestSlotDelayed, client);
        }
        return true;
    }
    #if DEBUGGING
        LogToFileEx(EquipLogFile, "RemoveSlotItems returned false");
    #endif
    return false;
}


public Action Command_GiveServer(int args) {
    Command_Give(0, args);
}

public Action Command_Give(int client, int args) {
    if(args < 2) {
        char temp[16];
        GetCmdArg(1, temp, sizeof(temp));
        if(StrEqual(temp, "list", false)) {
            ListValidWeapons(client);
            return Plugin_Continue;
        } else if(StrEqual(temp, "examples", false)) {
            GiveExamples(client);
            return Plugin_Continue;
        } else {
            GiveHelp(client);
            return Plugin_Continue;
        }
    }
    // set a max limit on arguments
    if(args > MAX_ARGS) {
        args = MAX_ARGS;
    }
    
    char pattern[MAX_NAME_LENGTH];
    GetCmdArg(1, pattern, sizeof(pattern));
    
    char buffer[32];
    char buffer2[32];
    int current_arg = 2;
    bool strip = false;
    
    GiveOpts opts;
    opts.type = GiveType_None;
    opts.mode = GiveMode_Add;
    opts.primary_pref = WEAPON_PREF_FAIL;
    opts.secondary_pref = WEAPON_PREF_FAIL;
    
    // Process strip/only and strip flags
    GetCmdArg(current_arg, buffer, sizeof(buffer));
    
    if(StrEqual(pattern, "reset", false)) {
        // swap `pattern` and `buffer`
        strcopy(buffer2, sizeof(buffer2), buffer);
        strcopy(buffer, sizeof(buffer), pattern);
        strcopy(pattern, sizeof(pattern), buffer2);
        buffer2 = "";
    }
    if(StrEqual(buffer, "reset", false)) {
        opts.mode = GiveMode_Reset;
        current_arg = args+1;
        buffer = "";
        if(strlen(pattern) == 0 || StrEqual(pattern, "all", false)) {
            pattern = "@all";
        }
    } else if(StrEqual(buffer, "strip", false) || StrEqual(buffer, "only", false)) {
        current_arg++;
        int j = 0;
        bool leaveknife = (StrEqual(buffer, "strip", false))? true: false;
        strip = leaveknife;
        opts.removals.knife = (!leaveknife)? true: false;
        opts.removals.secondary = true;
        opts.removals.primary = true;
        opts.removals.grenades = true;
        
        // hacky but succinct way to process strip options.  The `j++ < 5` increments j *after* checking against 5
        while(j++ < 5 && StripFlags(current_arg, opts.removals)) {}
    } else if(current_arg == 2 && StrEqual(buffer, "remove", false)) {
        // Check for remove mode
        opts.mode = GiveMode_Remove;
        current_arg++;
    }
    
    // Check for random
    GetCmdArg(current_arg, buffer, sizeof(buffer));
    if(StrEqual(buffer, "random", false)) {
        current_arg++;
        opts.type = GiveType_RandomGun;
    }
    
    
    // Weapon or Weapon Group
    if(args >= current_arg) {
        StringToLower(buffer);
        GetCmdArg(current_arg, buffer, sizeof(buffer));
        
        if(StrEqual(buffer, "knife")) {
            opts.weapon = "weapon_knife";
        } else if(StrEqual(buffer, "he")) {
            buffer = "hegrenade";
        } else if(StrEqual(buffer, "flash")) {
            buffer = "flashbang";
        } else if(StrEqual(buffer, "smoke")) {
            buffer = "smokegrenade";
        }
        
        if(StrEqual(buffer, "gun") || StrEqual(buffer, "guns")) {
            opts.type = GiveType_RandomGun; // running `!give <target> gun` should just give a random gun
            if(opts.mode == GiveMode_Remove) {
                opts.removals.primary = true;
                opts.removals.secondary = true;
            }
            current_arg++;
        } else if (StrEqual(buffer, "pistol") || StrEqual(buffer, "pistols")) {
            if(opts.type == GiveType_RandomGun && opts.mode == GiveMode_Remove) {
                ReplyToCommand(client, "Cannot execute '!give remove random pistol' - try '!give remove pistol' instead");
                return Plugin_Continue;
            }
            if(opts.type != GiveType_RandomGun) {
                if(opts.mode != GiveMode_Remove) {
                    ReplyToCommand(client, "Either 'remove' or 'random' must be specified with 'pistol': !give @all remove pistol");
                    return Plugin_Continue;
                }
                opts.removals.secondary = true;
                opts.type = GiveType_Secondary;
            } else {
                opts.type = GiveType_RandomSecondary;
            }
            current_arg++;
        } else if (StrEqual(buffer, "rifle") || StrEqual(buffer, "rifles")) {
            if(opts.type == GiveType_RandomGun && opts.mode == GiveMode_Remove) {
                ReplyToCommand(client, "Cannot execute '!give remove random rifle' - try '!give remove rifle' instead");
                return Plugin_Continue;
            }
            if(opts.type != GiveType_RandomGun) {
                if(opts.mode != GiveMode_Remove) {
                    ReplyToCommand(client, "Either 'remove' or 'random' must be specified with 'rifle': !give <target> remove rifle");
                    return Plugin_Continue;
                }
                opts.removals.primary = true;
                opts.type = GiveType_Primary;
            } else {
                opts.type = GiveType_RandomPrimary;
            }
            current_arg++;
        } else if (StrEqual(buffer, "grenade") || StrEqual(buffer, "grenades")) {
            if(opts.mode == GiveMode_Remove) {
                opts.type = GiveType_Grenades;
                opts.removals.grenades = true;
            } else {
                opts.type = (opts.type == GiveType_RandomGun)? GiveType_RandomGrenade: GiveType_Grenades;
            }
            current_arg++;
        } else if (StrEqual(buffer, "none")) {
            opts.type = GiveType_None;
            if(opts.mode == GiveMode_Add) {
                opts.removals.knife = true;
                opts.removals.secondary = true;
                opts.removals.primary = true;
                opts.removals.grenades = true;
            }
            current_arg++;
        } else if (StrEqual(buffer, "all") || StrEqual(buffer, "any")) {
            if(opts.type == GiveType_RandomGun) {
                opts.type = GiveType_RandomAny;
            } else if(opts.mode != GiveMode_Remove) {
                    ReplyToCommand(client, "The 'all' group can only be used with 'remove': !give <target> remove all");
                    return Plugin_Handled;
            } else {
                opts.type = GiveType_All;
            }
            current_arg++;
        } else {
            Format(buffer2, sizeof(buffer2), "weapon_%s", buffer);
            bool is_grenade = StrEqual(buffer2, "weapon_hegrenade") || StrEqual(buffer2, "weapon_flashbang") || StrEqual(opts.weapon, "weapon_smokegrenade");
            bool valid = is_valid_weapon(buffer2) || StrEqual(buffer2, "weapon_knife") || is_grenade;
            if(valid && opts.type == GiveType_RandomGun) {
                ReplyToCommand(client, "Cannot specify 'random' AND a specific weapon - please choose one.");
                return Plugin_Continue;
            }
            
            if(!valid && !strip) {
                ReplyToCommand(client, "Invalid weapon '%s'", buffer);
                ListValidWeapons(client);
                return Plugin_Continue;
            }
            if(valid) {
                strcopy(opts.weapon, sizeof(opts.weapon), buffer2);
                current_arg++;
            }
        }
    } else if(!strip && opts.mode != GiveMode_Reset) {
        ReplyToCommand(client, "No weapon or weapon group specified");
        return Plugin_Continue;
    }
    
    while(args >= current_arg) {
        GetCmdArg(current_arg, buffer, sizeof(buffer));
        StringToLower(buffer);
        if(StrEqual(buffer, "--save", false) || StrEqual(buffer, "--force", false)) {
            opts.save = 1;
            bool force = false;
            if(StrEqual(buffer, "--force", false)) {
                force = true;
                opts.save = 2;
            }
            switch(opts.type) {
                case GiveType_None: {
                    if(strlen(opts.weapon) > 8) {
                        int pref = ProEquip_GetWeaponIndex(opts.weapon[7]);
                        int slot = ProEquip_GetWeaponSlot(opts.weapon[7]);
                        if(pref != -1 && slot != -1) {
                            switch(slot) {
                                case 0: {
                                    opts.primary_pref = pref;
                                    if(force) {
                                        opts.force_primary = true;
                                    }
                                }
                                case 1: {
                                    opts.secondary_pref = pref;
                                    if(force) {
                                        opts.force_secondary = true;
                                    }
                                }
                            }
                        }
                    } else if (opts.weapon[0] == '\0') {
                        #if DEBUGGING
                            LogToFileEx(EquipLogFile, "!give --save - no weapon specified");
                        #endif
                    }
                }
                case GiveType_RandomPrimary: {
                    opts.primary_pref = RANDOM_WEAPON_AVAILABLE;
                    if(force) {
                        opts.force_primary = true;
                    }
                }
                case GiveType_RandomSecondary: {
                    opts.secondary_pref = RANDOM_WEAPON_AVAILABLE;
                    if(force) {
                        opts.force_secondary = true;
                    }
                }
                case GiveType_RandomGrenade: {}
                case GiveType_Secondary: {
                    if(opts.mode == GiveMode_Add) {
                        opts.secondary_pref = RANDOM_WEAPON_AVAILABLE;
                    }
                    opts.force_secondary = true;
                }
                case GiveType_Primary: {
                    
                }
                default: {
                    ReplyToCommand(client, "Give currently only supports using --save with a specific weapon or random pistol/primary (not rifles/pistols)");
                }
            }
        }
        current_arg++;
    }
    
    process_give_targets(client, opts, pattern);
    
    return Plugin_Continue;
}





// return value is whether looping over 'remove' options can continue
bool StripFlags(int &arg, GiveRemovals removals) {
    char buffer[32];
    GetCmdArg(arg, buffer, sizeof(buffer));
    if(is_opt_switch(buffer[0]) && StrEqual(buffer[1], "all", false)) {
        process_removal_arg(removals.knife, buffer[0]);
        process_removal_arg(removals.secondary, buffer[0]);
        process_removal_arg(removals.primary, buffer[0]);
        process_removal_arg(removals.grenades, buffer[0]);
        arg++;
        return true;
    } else if (is_opt_switch(buffer[0]) && StrEqual(buffer[1], "guns", false)) {
        process_removal_arg(removals.secondary, buffer[0]);
        process_removal_arg(removals.primary, buffer[0]);
        arg++;
        return true;
    } else if ((is_opt_switch(buffer[0])) && StrEqual(buffer[1], "grenades", false)) {
        process_removal_arg(removals.grenades, buffer[0]);
        arg++;
        return true;
    } else if ((is_opt_switch(buffer[0])) && StrEqual(buffer[1], "knife", false)) {
        process_removal_arg(removals.knife, buffer[0]);
        arg++;
        return true;
    } else if (is_opt_switch(buffer[0]) && (StrEqual(buffer[1], "pistol", false) || StrEqual(buffer[1], "pistols", false))) {
        process_removal_arg(removals.secondary, buffer[0]);
        arg++;
        return true;
    } else if (is_opt_switch(buffer[0]) && (StrEqual(buffer[1], "rifle", false) || StrEqual(buffer[1], "rifles", false))) {
        process_removal_arg(removals.primary, buffer[0]);
        arg++;
        return true;
    }
    
    return false;
}

public bool is_opt_switch(char c) {
    return c == '+' || c == '-' || c == '~';
}

void process_removal_arg(bool &removal, char opt_switch) {
    switch(opt_switch) {
        case '+': { // +grenades will keep grenades
            removal = false;
        }
        case '-': { // -grenades will remove grenades
            removal = true;
        }
        case '~': { // ~grenades will toggle grenades
            removal = !removal;
        }
    }
}


int process_give_targets(int client, GiveOpts opts, const char[] pattern) {
    char name_buffer[MAX_NAME_LENGTH];
    int targets[MAXPLAYERS+1];
    bool ml = false;
    int count = ProcessTargetString(pattern, client, targets, sizeof(targets), COMMAND_FILTER_ALIVE, name_buffer, sizeof(name_buffer), ml);
    if(count <= 0) {
        ReplyToCommand(client, (count < 0)? "\x07%06X[give] Invalid target\x01": "\x07%06X[give] No target(s) found\x01", COMMAND_ERROR_COLOR);
        return 0;
    }
    int processed = 0;
    for(int i=0; i < count; i++) {
        if(Give(targets[i], opts)){
            processed++;
        }
    }
    ReplyToCommand(client, "Give affected %i users", processed);
    return processed;
}

bool is_valid_weapon(const char[] weapon) {
    int slot = 0;
    return weapon_slots.GetValue(weapon, slot);
}







bool RemoveAllGrenades(int client) {
    int ent = -1;
    bool affected = false;
    while((ent = GetPlayerWeaponSlot(client, 3)) > 0 && IsValidEdict(ent)) {
        RemovePlayerItem(client, ent);
        RemoveEntity(ent);
        affected = true;
    }
    return affected;
}




bool RemoveSlotItems(int client, bool removals[4]) {
    bool affected = false;
    for(int i=0; i < 3; i++) {
        if(removals[i]) {
            #if DEBUGGING
                LogToFileEx(EquipLogFile, "Processing slot %i", i);
            #endif
            int ent = GetPlayerWeaponSlot(client, i);
            if(ent > 0 && IsValidEdict(ent)) {
                #if DEBUGGING
                    LogToFileEx(EquipLogFile, "Removing slot %i", i);
                #endif
                RemovePlayerItem(client, ent);
                RemoveEntity(ent);
                affected = true;
            }
        }
    }
    if(removals[3]) {
        affected = affected | RemoveAllGrenades(client);
    }
    return affected;
}

// Currently only works correctly for human players
// looks at all slots that a player has an existing weapon in and chooses a slot based on how many potential weapons could be in that slot
// in other words: if you want to give someone a random weapon from ANY class of weapons, use this to choose the slot fairly instead of a 1 in 4 chance for each slot
int GetAnyExistingRandomSlot(int client) {
    int map[sizeof(primaries) + sizeof(secondaries) + 1 + sizeof(grenades)];
    int slots[4];
    int total = 0;
    int pos = 0;
    for(int i=0; i < 4; i++) {
        slots[i] = GetPlayerWeaponSlot(client, i);
        if(slots[i] > 0 && IsValidEdict(slots[i])) {
            total += weapons_in_slot[i];
            for(int j=0; j < weapons_in_slot[i]; j++) {
                map[pos++] = i;
            }
        }
    }
    return map[GetURandomInt() % total];
}

int RandomWeaponAnyGun(char[] weapon, int size, int slot) {
    if(slot == 0) {
        strcopy(weapon, size, primaries[GetURandomInt() % sizeof(primaries)]);
    } else {
        strcopy(weapon, size, secondaries[GetURandomInt() % sizeof(secondaries)]);
    }
    return slot;
}

void RandomPrimary(char[] weapon, int size) {
    strcopy(weapon, size, primaries[GetURandomInt() % sizeof(primaries)]);
}

void RandomSecondary(char[] weapon, int size) {
    strcopy(weapon, size, secondaries[GetURandomInt() % sizeof(secondaries)]);
}

void RandomGrenade(char[] weapon, int size) {
    strcopy(weapon, size, grenades[GetURandomInt() % sizeof(grenades)]);
}

void GiveHelp(int client) {
    ReplyToCommand(client, "Give Usage:")
    ReplyToCommand(client, "!give <target> <weapon> [--save|--force]");
    ReplyToCommand(client, "!give <target> random [rifle|pistol|gun|grenade] [--save|--force]");
    ReplyToCommand(client, "!give <target> remove [rifle|pistol|knife|<weapon>] [--save|--force]");
    ReplyToCommand(client, "!give <target> only <weapon> [--save|--force]");
    ReplyToCommand(client, "!give <target> strip [+|-grenades|rifles|pistols|knife] [<weapon>] [--save|--force]");
    ReplyToCommand(client, "!give reset <target>");
    ReplyToCommand(client, "!give help|list|examples");
}

void GiveExamples(int client) {
    ReplyToCommand(client, "GIVE EXAMPLES");
    ReplyToCommand(client, "\x07ffffffBasic usage:\x01");
    ReplyToCommand(client, "  !give Vishus scout");
    ReplyToCommand(client, "\x07ffffffGive weapon and disable changing it:\x01");
    ReplyToCommand(client, "  !give @all scout --force");
    ReplyToCommand(client, "\x07ffffffRandom weapon\x01");
    ReplyToCommand(client, "  !give @bots only random any");
    ReplyToCommand(client, "Strip all players (normally only removes guns -  'grenades' removes grenades too) and give a random rifle:");
    ReplyToCommand(client, "  !give @all strip -grenades random rifle");
    ReplyToCommand(client, "Strip all players (down to a knife and grenades) and give a random pistol:");
    ReplyToCommand(client, "  !give @all strip random pistol");
    ReplyToCommand(client, "Reset everyone to defaults:");
    ReplyToCommand(client, "  !give reset @all");
}

void ListValidWeapons(int client) {
    ReplyToCommand(client, "\x07%06X%s\x01", 0xDDDDDD, valid_weapons_msg[0]);
    ReplyToCommand(client, "\x07%06X%s\x01", 0xDDDDDD, valid_weapons_msg[1]);
}
