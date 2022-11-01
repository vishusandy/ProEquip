#if defined _pro_equip_nv_cmd_included
    #endinput
#endif
#define _pro_equip_nv_cmd_included

#include <pro_equip/utils.inc>
#undef REQUIRE_PLUGIN
#include <pro_equip/ProEquip.inc>
#include <ProNightvision.inc>
#define REQUIRE_PLUGIN


void nv_plugin_start() {
    RegAdminCmd("setnv", Command_SetNightvision, ADM_LEVEL, "Set nightvision for a player");
    RegAdminCmd("setnightvision", Command_SetNightvision, ADM_LEVEL, "Set nightvision for a player");
}

public void OnLibraryRemoved(const char[] name) {
    if(StrEqual(name, "pro_nightvision")) {
        nv_loaded = false;
    }
}
public void OnLibraryAdded(const char[] name) {
    if(StrEqual(name, "pro_nightvision")) {
        nv_loaded = true;
    }
}

public void NightvisionCommandHelp(client) {
    ReplyToCommand(client, "Usage: !setnv <target> on|normal|off|<filter_name>");
    ReplyToCommand(client, "Usage: !setnv list");
}

public Action Command_SetNightvision(client, args) {
    if(!nv_loaded) {
        ReplyToCommand(client, "Nightvision is not installed or is disabled");
        return Plugin_Continue;
    }
    if(args == 0) {
        NightvisionCommandHelp(client);
        return Plugin_Continue;
    }
    
    char pattern[MAX_NAME_LENGTH];
    GetCmdArg(1, pattern, sizeof(pattern));
    
    if(StrEqual(pattern, "help", false)) {
        NightvisionCommandHelp(client);
        return Plugin_Continue;
    } else if(StrEqual(pattern, "list", false)) {
        ProNightvision_ListFilters(client);
        return Plugin_Continue;
    }
    
    char name[64];
    
    GetCmdArg(2, name, sizeof(name));
    StringToLower(name);
    bool off = StrEqual(name, "off", false);
    int filter_id;
    if(!off) {
        filter_id = ProNightvision_GetFilterId(name);
        if(filter_id < 0) {
            ReplyToCommand(client, "Filter '%s' not found", name);
            ProNightvision_ListFilters(client);
            return Plugin_Continue;
        }
        LogToFileEx(EquipLogFile, "Found filter %i", filter_id);
    }
    
    
    
    char targ_buffer[MAX_NAME_LENGTH];
    int[] targets = new int[MaxClients];
    bool ml = false;
    int count = ProcessTargetString(pattern, client, targets, MaxClients, COMMAND_FILTER_CONNECTED, targ_buffer, sizeof(targ_buffer), ml);
    
    int processed = 0;
    if(count <= 0) {
        ReplyToCommand(client, (count < 0)? "Bad target": "No matching targets found");
        return Plugin_Continue;
    } else {
        for(int i=0; i<count; i++) {
            if(!off) {
                if(ProNightvision_SetFilter(targets[i], filter_id, true)) {
                    processed += 1;
                }
            } else {
                if(ProNightvision_DisplayNightvision(targets[i], false)) {
                    processed += 1;
                }
            }
        }
    }
    
    ReplyToCommand(client, "Affected %i players", processed);
    return Plugin_Continue;
}

