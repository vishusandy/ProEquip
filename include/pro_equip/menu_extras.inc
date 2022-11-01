#if defined _pro_equip_menu_extras_included
    #endinput
#endif
#define _pro_equip_menu_extras_included

enum struct MenuExtra {
    int id;
    char title[32];
    char value[32];
    char cmd[32];
    char msg[256];
}

// char menu_extras_titles[][] = {"Store", "Perks", "Sounds", "Maps"};
// char menu_extras_values[][] = {"store", "perks", "sound_perks", "maps"};
// char menu_extras_cmds[][] = {"sm_shop", "perks", "sound_perks", "maplist"};
// char menu_extras_msgs[][] = {};

ArrayList menu_extras;


void load_menu_extras() {
    if(DB == null) { return; }
    DB.Query(LoadMenuExtrasCallback, "SELECT `value`, `title`, `cmd`, `msg` FROM `pro_menu` ORDER BY ordering, id");
}

public void LoadMenuExtrasCallback(Database db, DBResultSet result, const char[] error, any data) {
    if(strlen(error) > 0) {
        LogToFile(EquipLogFile, "Load menu extras callback error: %s", error);
        return;
    }
    
    if(result == null) {
        return;
    }
    
    int processed = 0;
    
    while (processed < (32 - ReservedMenuSlots) && result.FetchRow()) {
        MenuExtra extra;
        extra.id = processed;
        result.FetchString(0, extra.value, 32);
        result.FetchString(1, extra.title, 32);
        result.FetchString(2, extra.cmd, 32);
        result.FetchString(3, extra.msg, 256);
        #if DEBUGGING
            LogToFile(EquipLogFile, "Extra: title=%s value=%s cmd=%s msg=%s", extra.title, extra.value, extra.cmd, extra.msg);
        #endif
        menu_extras.PushArray(extra);
        processed += 1;
    }
}
