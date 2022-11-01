#if defined _pro_equip_prefs_included
    #endinput
#endif
#define _pro_equip_prefs_included

#include <pro_equip/constants.inc>
#include <pro_equip/weapon_utils.inc>


enum struct WeaponMenu {
    bool weapon_menu_enabled;
    bool primary_menu_enabled;
    bool secondary_menu_enabled;
    void reset() {
        this.weapon_menu_enabled = true;
        this.primary_menu_enabled = true;
        this.secondary_menu_enabled = true;
    }
}

WeaponMenu weapon_menu;

enum struct MenuState {
    showMenu show_menu; // current menu to display
    showMenu next_menu; // what menu to show next
    showMenu prev_menu;
    
    void reset() {
        this.show_menu = ShowMenu_Main;
        this.next_menu = ShowMenu_None;
        this.prev_menu = ShowMenu_None;
    }
}

enum struct GunPref {
    int pistol; // use -1 for random weapon of available weapon choices, -2 for random weapon from all weapons (disregards map configs for weapon availability)
    int rifle; // use -1 for random weapon of available weapon choices, -2 for random weapon from all weapons (disregards map configs for weapon availability)
    int primary_ammo;
    int secondary_ammo;
    void set_rifle(int gun) {
        this.rifle = gun;
    }
    void set_pistol(int gun) {
        this.pistol = gun;
    }
    void reset_ammo() {
        this.primary_ammo = NO_OVERRIDE;
        this.secondary_ammo = NO_OVERRIDE;
    }
    void give_rifle_ammo(int client) {
        if(this.primary_ammo < 0) {
            return;
        }
        give_reserve_ammo(client, 0, this.primary_ammo);
    }
    bool has_any_infinite_guns() {
        return this.rifle == INFINITE_AMMO || this.pistol == INFINITE_AMMO;
    }
    bool set_slot_ammo(int client, int slot, int amount) {
        switch(slot) {
            case 0: {
                this.primary_ammo = amount;
                return true;
            }
            case 1: {
                this.secondary_ammo = amount;
                return true;
            }
            default: {
                return false;
            }
        }
    }
    void give_pistol_ammo(int client) {
        if(this.secondary_ammo < 0) {
            return;
        }
        give_reserve_ammo(client, 1, this.secondary_ammo);
    }
    void weapon_fire(int client, int slot) {
        switch(slot) {
            case 0: {
                if(this.primary_ammo == INFINITE_AMMO) {
                    inc_ammo(client, 0);
                }
            }
            case 1: {
                if(this.secondary_ammo == INFINITE_AMMO) {
                    inc_ammo(client, 1);
                }
            }
        }
    }
}

enum msgBits {
    MSG_MAIN    = 0,
    MSG_STORE   = 1,
    MSG_PERKS   = 2,
    MSG_SOUNDS  = 3,
    MSG_NV      = 4,
    MSG_MAPS    = 5,
    MSG_FRIENDS = 6,
}

enum struct MsgFlags {
    int msgs;
    
    void reset() {
        this.msgs = 0xFFFFFFF;
    }
    
    bool get(int msg) {
        return (this.msgs & (1 << msg)) != 0;
    }
    
    void set(int msg) {
        this.msgs = this.msgs | (1 << msg);
    }
    
    void clear(int msg) {
        this.msgs = this.msgs & ~(1 << msg);
    }
    
    void toggle(int msg) {
        this.msgs = this.msgs ^ (1 << msg);
    }
    
    void change(int msg, bool on) {
        if(on)
            this.set(msg);
        else
            this.clear(msg);
    }
    
    bool show_extra_msg(int client, int index) {
        if(!this.get(index + ReservedMenuSlots)) {
            return false;
        }
        
        MenuExtra extra;
        menu_extras.GetArray(index, extra);
        if(extra.msg[0] != '\0') {
            PrintToChat(client, extra.msg);
        }
        this.clear(index + ReservedMenuSlots);
        return true;
    }
    
    bool show_main(int client) {
        if(!this.get(MSG_MAIN)) {
            return false;
        }
        PrintToChat(client, "Type `!guns` or `!menu` to reopen the main menu.  You can also type `!rifles`, `!pistols` to choose a specific weapon.");
        this.clear(MSG_MAIN);
        return true;
    }
    bool show_nv(int client) {
        if(!this.get(MSG_NV)) {
            return false;
        }
        PrintToChat(client, "Type '!nv' or '!nightvision' in chat to open the nightvision menu");
        this.clear(MSG_NV);
        return true;
    }
}

enum struct bitflags {
    int flags;
    
    void reset(int reset=0x0) {
        this.flags = reset;
    }
    
    bool get(int flag) {
        return (this.flags & (1 << flag)) != 0; // retrieve the specified bit
    }
    
    void set(int flag) {
        this.flags = this.flags | (1 << flag);
    }
    
    void clear(int flag) {
        this.flags = this.flags & ~(1 << flag);
    }
    
    void toggle(int flag) {
        this.flags = this.flags ^ (1 << flag);
    }
    
    void change(int flag, bool on) {
        if(on)
            this.set(flag);
        else
            this.clear(flag);
    }
}
