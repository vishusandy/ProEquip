#if defined _pro_equip_menus_included
    #endinput
#endif
#define _pro_equip_menus_included

#include <pro_equip/constants.inc>
#include <pro_equip/vars.inc>

void display_menu(int client) {
    if(!IsClientInGame(client) || !weapon_menu.weapon_menu_enabled || !player_data[client].menu.weapon_menu_enabled) {
        return;
    }
    switch(player_data[client].menu_state.show_menu) {
        case ShowMenu_Main: {
            display_main_menu(client, 0);
        }
        case ShowMenu_Primary: {
            display_primary_menu(client, 0);
        }
        case ShowMenu_Secondary: {
            display_secondary_menu(client, 0);
        }
    }
}

public Action display_main_menu(int client, int args) {
    if(!IsClientInGame(client)) {
        return Plugin_Handled;
    }
    if(!weapon_menu.weapon_menu_enabled || !player_data[client].menu.weapon_menu_enabled) {
        PrintToChat(client, "Weapons menu is not available");
        return Plugin_Handled;
    }
    Menu main_menu = new Menu(menu_handler_main, MENU_ACTIONS_DEFAULT);
    main_menu.SetTitle("Welcome");
    if(player_weapons.num_primaries > 0 && player_data[client].menu.primary_menu_enabled && give_weapons && weapon_menu.weapon_menu_enabled && weapon_menu.primary_menu_enabled) {
        main_menu.AddItem("primary", "Primary Weapons");
    } else {
        main_menu.AddItem("primary", "Primary Weapons", ITEMDRAW_DISABLED);
    }
    if(player_weapons.num_secondaries > 0 && player_data[client].menu.secondary_menu_enabled && give_weapons && weapon_menu.weapon_menu_enabled && weapon_menu.secondary_menu_enabled) {
        main_menu.AddItem("secondary", "Secondary Weapons");
    } else {
        main_menu.AddItem("secondary", "Secondary Weapons", ITEMDRAW_DISABLED);
    }
    if(player_equip.nightvision && nv_loaded) {
        main_menu.AddItem("nightvision", "Nightvision Settings");
    } else {
        main_menu.AddItem("nightvision", "Nightvision: DISABLED", ITEMDRAW_DISABLED);
    }
    
    for(int i=0; i < menu_extras.Length; i++) {
        MenuExtra extra;
        menu_extras.GetArray(i, extra);
        main_menu.AddItem(extra.value, extra.title);
    }
    // main_menu.AddItem("store", "Store");
    // main_menu.AddItem("perks", "Perks");
    // main_menu.AddItem("sound_perks", "Sounds");
    // main_menu.AddItem("maps", "Maps");
    
    
    if(player_equip.silencers) {
        if(!player_data[client].equip.silencers) {
            main_menu.AddItem("silencers", "Silencers: Off");
        } else {
            main_menu.AddItem("silencers", "Silencers: On");
        }
    } else {
        main_menu.AddItem("silencers", "Silencers: DISABLED", ITEMDRAW_DISABLED);
    }
    main_menu.Display(client, MENU_DISPLAY_TIME);
    return Plugin_Continue;
}

public int menu_handler_main(Menu menu, MenuAction action, int client, int param) {
    switch(action) {
        case MenuAction_End: {
            menu_end(menu);
        }
        case MenuAction_Cancel: {
            player_data[client].menu_state.show_menu = ShowMenu_None;
        }
        case MenuAction_Select: {
            char item[32];
            bool is_alive = !(!IsClientInGame(client) || !IsPlayerAlive(client));
            player_data[client].menu_state.show_menu = ShowMenu_None;
            player_data[client].menu_state.next_menu = ShowMenu_None;
            player_data[client].menu_state.prev_menu = ShowMenu_Main;
            GetMenuItem(menu, param, item, sizeof(item));
            
            player_data[client].msgs.show_main(client);
            
            if(StrEqual(item, "primary")) {
                player_data[client].menu_state.show_menu = ShowMenu_Primary;
                player_data[client].menu_state.next_menu = ShowMenu_Secondary;
                display_primary_menu(client, 0);
            } else if(StrEqual(item, "secondary")) {
                player_data[client].menu_state.show_menu = ShowMenu_Secondary;
                display_secondary_menu(client, 0);
            } else if(StrEqual(item, "nightvision")) {
                player_data[client].msgs.show_nv(client);
                if(nv_loaded) {
                    ProNightvision_NightvisionMenu(client);
                }
            } else if(StrEqual(item, "silencers")) {
                player_data[client].equip.silencers = !player_data[client].equip.silencers;
                if(is_alive) {
                    give_silencers(client, 0, player_data[client].equip.silencers);
                    give_silencers(client, 1, player_data[client].equip.silencers);
                }
                display_main_menu(client, 0);
            } else {
                for (int i=0; i < menu_extras.Length; i++) {
                    MenuExtra extra;
                    menu_extras.GetArray(i, extra);
                    if(StrEqual(item, extra.value) && extra.cmd[0] != '\0') {
                        FakeClientCommandEx(client, extra.cmd);
                        player_data[client].msgs.show_extra_msg(client, extra.id);
                        break;
                    }
                }
            }
            display_menu(client);
        }
    }
}



public Action display_primary_menu(int client, int args) {
    if(!IsClientInGame(client)) {
        return Plugin_Continue;
    }
    if(!IsPlayerAlive(client)) {
        player_data[client].menu_state.prev_menu = ShowMenu_None;
        // return Plugin_Handled;
    }
    if(player_weapons.num_primaries == 0 || !weapon_menu.weapon_menu_enabled || !weapon_menu.primary_menu_enabled || !player_data[client].menu.weapon_menu_enabled || !player_data[client].menu.primary_menu_enabled || !give_weapons) {
        PrintToChat(client, "Primary weapons menu is not available");
        player_data[client].menu_state.prev_menu = ShowMenu_None;
        return Plugin_Handled;
    }
    Menu primary_menu = new Menu(menu_handler_primary, MENU_ACTIONS_DEFAULT);
    primary_menu.SetTitle("Primary Weapons");
    
    menu_order(primary_menu, 0);
    
    if(player_weapons.num_primaries > 1) {
        primary_menu.AddItem(RANDOM_WEAPON_AVAILABLE_STRING, "Random Rifle");
    }
    primary_menu.Display(client, MENU_DISPLAY_TIME);
    return Plugin_Continue;
}

public int menu_handler_primary(Menu menu, MenuAction action, int client, int param) {
    switch(action) {
        case MenuAction_Cancel: {
            player_data[client].menu_state.show_menu = ShowMenu_None;
        }
        case MenuAction_Select: {
            char item[32];
            GetMenuItem(menu, param, item, sizeof(item));
            int index = StringToInt(item);
            player_data[client].menu_state.show_menu = player_data[client].menu_state.next_menu;
            player_data[client].menu_state.next_menu = ShowMenu_None;
            player_data[client].menu_state.prev_menu = ShowMenu_Primary;
            if((player_data[client].menu_state.show_menu == ShowMenu_Secondary && player_weapons.num_secondaries == 0) || !player_data[client].menu.secondary_menu_enabled) {
                player_data[client].menu_state.show_menu = ShowMenu_None;
            }
            player_data[client].set_rifle(index);
            if(IsPlayerAlive(client)) {
                if(!give_weapon_index(client, primaries, sizeof(primaries), player_weapons.primary_choices, player_weapons.num_primaries, 0, index)) {
                    LogToFileEx(EquipLogFile, "Failed to give primary item %i to %N", index, client);
                } else {
                    ClientCommand(client, "slot1");
                }
            }
            CreateTimer(0.1, TimerDelayedMenu, client);
        }
        case MenuAction_End: {
            menu_end(menu);
        }
    }
    return 0;
}



public Action display_secondary_menu(int client, int args) {
    if(!IsClientInGame(client)) {
        return Plugin_Continue;
    }
    if(!IsPlayerAlive(client)) {
        player_data[client].menu_state.prev_menu = ShowMenu_None;
        // return Plugin_Handled;
    }
    if(player_weapons.num_secondaries == 0 || !weapon_menu.weapon_menu_enabled || !weapon_menu.secondary_menu_enabled || !player_data[client].menu.weapon_menu_enabled || !player_data[client].menu.secondary_menu_enabled || !give_weapons) {
        PrintToChat(client, "Secondary weapons menu is not available");
        player_data[client].menu_state.prev_menu = ShowMenu_None;
        return Plugin_Handled;
    }
    Menu secondary_menu = new Menu(menu_handler_secondary, MENU_ACTIONS_DEFAULT);
    secondary_menu.SetTitle("Secondary Weapons");

    menu_order(secondary_menu, 1);

    if(player_weapons.num_secondaries > 1) {
        secondary_menu.AddItem(RANDOM_WEAPON_AVAILABLE_STRING, "Random Pistol");
    }
    secondary_menu.Display(client, MENU_DISPLAY_TIME);
    return Plugin_Continue;
}

public int menu_handler_secondary(Menu menu, MenuAction action, int client, int param) {
    switch(action) {
        case MenuAction_Cancel: {
            player_data[client].menu_state.show_menu = ShowMenu_None;
        }
        case MenuAction_Select: {
            char item[32];
            GetMenuItem(menu, param, item, sizeof(item));
            int index = StringToInt(item);
            player_data[client].set_pistol(index);
            if(IsPlayerAlive(client)) {
                if(!give_weapon_index(client, secondaries, sizeof(secondaries), player_weapons.secondary_choices, player_weapons.num_secondaries, 1, index)) {
                    LogToFileEx(EquipLogFile, "Failed to give secondary item %i to %N", index, client);
                } else if(player_data[client].menu_state.prev_menu != ShowMenu_Primary) {
                    ClientCommand(client, "slot2");
                }
            }
            player_data[client].menu_state.show_menu = ShowMenu_None;
            player_data[client].menu_state.prev_menu = ShowMenu_Secondary;
        }
        case MenuAction_End: {
            menu_end(menu);
        }
    }
    return 0;
}


public Action MainMenuCommand(int client, int args) {
    player_data[client].menu_state.prev_menu = ShowMenu_Main;
    return display_main_menu(client, args);
}
public Action PrimaryMenuCommand(int client, int args) {
    player_data[client].menu_state.prev_menu = ShowMenu_Primary;
    return display_primary_menu(client, args);
}
public Action SecondaryMenuCommand(int client, int args) {
    player_data[client].menu_state.prev_menu = ShowMenu_Secondary;
    return display_secondary_menu(client, args);
}


void menu_end(Menu menu) {
    delete menu;
}

public Action TimerDelayedMenu(Handle timer, int client) {
    display_menu(client);
}


bool use_compact_menu(int slot) {
    switch(slot) {
        case 0: {
            // return (sizeof(primaries) - COMPACT_MENU_THRESHOLD) > player_weapons.num_primaries;
            return player_weapons.num_primaries <= COMPACT_MENU_THRESHOLD;
        }
        case 1: {
            // return (sizeof(secondaries) - COMPACT_MENU_THRESHOLD) > player_weapons.num_secondaries;
            return player_weapons.num_secondaries <= COMPACT_MENU_THRESHOLD;
        }
        default: {
            return false;
        }
    }
}

void menu_order(Menu menu, int slot) {
    if(use_compact_menu(slot)) {
        compact_menu_order(menu, slot);
    } else {
        sparse_menu_order(menu, slot);
    }
}

public void compact_menu_order(Menu menu, int slot) {
    switch(slot) {
        case 0: {
            char buffer[8];
            for(int i=0; i < player_weapons.num_primaries; i++) {
                IntToString(player_weapons.primary_choices[i], buffer, sizeof(buffer));
                menu.AddItem(buffer, primaries[player_weapons.primary_choices[i]][7]);
            }
        }
        case 1: {
            char buffer[8];
            for(int i=0; i < player_weapons.num_secondaries; i++) {
                IntToString(player_weapons.secondary_choices[i], buffer, sizeof(buffer));
                menu.AddItem(buffer, secondaries[player_weapons.secondary_choices[i]][7]);
            }
        }
    }
}

public void sparse_menu_order(Menu menu, int slot) {
    switch(slot) {
        case 0: {
            char buffer[8];
            for(int i=0; i < sizeof(primaries); i++) {
                IntToString(i, buffer, sizeof(buffer));
                int idx = i;
                int style = (player_weapons.is_weapon_idx_available(slot, idx))? ITEMDRAW_DEFAULT: ITEMDRAW_DISABLED;
                menu.AddItem(buffer, primaries[idx][7], style);
            }
        }
        case 1: {
            char buffer[8];
            for(int i=0; i < sizeof(secondaries); i++) {
                IntToString(i, buffer, sizeof(buffer));
                int idx = i;
                int style = (player_weapons.is_weapon_idx_available(slot, idx))? ITEMDRAW_DEFAULT: ITEMDRAW_DISABLED;
                menu.AddItem(buffer, secondaries[idx][7], style);
            }
        }
    }
}

