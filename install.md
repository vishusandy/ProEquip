
# Installation

Installation is pretty simple.  There are optional components, but the basic install is just the smx file.  It is highly recommended to add some [config files](#config-files), but it can run without those.

- [Installation](#basic-installation)
    - [config files](#config-files)
    - [optional - custom menu entries](install.md#optional---custom-menu-entries)

## Basic Installation

1. If using CSSDM, disable the `dm_equipment` plugin first.

2. Copy the .smx file to the plugins folder (e.g. `cstrike/addons/sourcemod/plugins`) and load the file using `sm plugins load`.

3. **Optionally** add CSSDM config files.  It can work without any config files, but using them is **recommended**.

4. **Optionally** install [ProNightvision](https://github.com/vishusandy/ProNightvision)

5. **Optionally** add custom menu entries (see below)

## Config files

Config files are not required for the plugin to work, however without any config files you can't specify default weapon/equipment settings.  An example of the config format can be found [here](cssdm.equip.txt).

Config files:

- Global settings
  
  - `cstrike/cfg/cssdm/cssdm.equip.txt`

- Map-specific settings
  
  - `cstrike/cfg/cssdm/maps/<map_name>.equip.txt`

Note: If using CSSDM it is recommended to set `cssdm_refill_ammo "0"` in `cstrike/cfg/cssdm/cssdm.cfg` to allow infinite grenades to work properly.

## Optional - Custom Menu Entries

Optionally, custom menu entries can added and will be displayed in the main menu. Each menu entry will have a command associated with it; after selecting the menu entry the command will be executed with the specified client ID.

Custom menu entries are stored in a database.  You can skip this step if no custom menu entries are desired.  You can add up to 30 custom menu entries (that number seems gratuitous, but maybe there's a use case it).

How to add custom menu entries:

1. To add custom menu entries add the following to`cstrike/addons/sourcemod/configs/databases.cfg`:

```
  "pro_menu"
    {
        "driver"    "default"
        "host"      "<hostname>"
        "database"  "<database>"
        "user"      "<username>"
        "pass"      "<password>"
    }
```

2. After starting the plugin the table will be automatically created (only tested with MySQL but should work with Postgres and SQLite too).  If you have issues see: [Database Setup](db_setup.md)

3. Add rows consisting of:
   
   - an auto-incremented `id`
   
   - an `ordering` field to specify the arbitrary order to sort the menu items by (lowest are displayed first)
   
   - a `title` that will be displayed in the menu
   
   - a `value` which is a unique value tied to that entry
   
   - a `cmd` which will be the command that gets executed (do not include a ! or /)
   
   - an optional message that gets printed to chat the first time the user selects the menu item (useful for learning purposes). Leave blank or set to null to not show a message.  It is recommended to type a short message 
