
# Pro Equip

Intended to be a feature-rich drop-in replacement for CSSDM's dm_equipment.
This should work without CSSDM, however I have not tested this.

Works with existing config files for CSSDM's dm_equipment.  Allows config files to specifiy a number of smoke and he grenades instead of just yes/no.

Several natives are provided that allow other plugins modify a players' weapon preferences and equipment.

If Pro Nightvision is running it will also allow setting custom nightvision filters (with the !setnv command) as well as reactivating nightvision after a respawn.

## Usage

In chat type `guns`, `menu`, or `weapons` to display the main menu.  `rifles` will bring up just the rifle menu and `pistols` will show the pistols menu.  These can also be used as commands with ! or / like `!rifles` or `/menu`.

## Admin Commands

### `!equip`

The `!equip` command modifies grenade quantities, armor, and hp as well as what weapons are available for bots and/or players (all bots/all players, not individual players).  It can also be used to reset equipment to defaults.

```
  !equip [bots|players|all|override <player>] [+|-all|rifles|pistols|knife] [hp|armor|he|flash|smoke=<num>]
  !equip override <target> reset
  !equip reset
  !equip examples
  
  # Examples
  !equip he=4 smoke=2 flash=0 armor=100 hp=250
  !equip -rifles +scout -pistols +glock +usp -grenades
  !equip all -all +scout armor=0 hp=100
  !equip players -flashbang +hegrenade
  !equip override <target> reset
```

### `!give`

`!give` command gives/removes weapons and can modify weapon preferences.


#### Modifying preferences

The `--save` flag will change the target's weapon preferences, `--force` will change preferences while also disabling the menu.  This can be undone using `!give reset <target>`


#### `!give only`

Using `!give <target> only <weapon>` will remove all other weapons, including the knife.

Giving a user only a specific grenade, like with `!give <target> only hegrenade` will give them an unlimited supply of the grenade, but one at a time.
This is done this way to avoid issues when the player only has grenades but has multiple of them; if they throw one the game tries to switch to another weapon but they have no other weapons and then are unable to throw anymore of that grenade.  If using CSSDM set `cssdm_refill_ammo "0"` in the `cssdm.cfg` for this to work properly.


#### `!give strip`

Using `!give <target> strip` gives more flexibility than `only` by allowing the admin to specify which weapon types will be removed (or added).  Unlike `only` the `strip` subcommand does not require a weapon name to be specified (can be used as just `!give <target> strip`).  If no other weapon types are specified it will leave the user with only a knife and the specified weapon (if given).  Weapon types can be removed 

### Console Commands

- `dbg_equip dump`: dumps debug info to the log file.  The location of this file can be found in `include/pro_equip/constants.inc`.

#### Usage

Command usage:

```
  !give <target> <weapon> [--save|--force]
  !give <target> random [rifle|pistol|gun|grenade] [--save|--force]
  !give <target> remove [rifle|pistol|knife|<weapon>] [--save|--force]
  !give <target> only <weapon> [--save|--force]
  !give <target> strip [+|-grenades|rifles|pistols|knife] [<weapon>] [--save|--force]
  !give reset <target>
  !give help|list|examples
```

Examples:

```
  !give help
  !give examples
  !give <target> scout
  !give <target> random grenade
  !give <target> random pistol --save
  !give <target> only hegrenade --force
  !give @all scout --force
  !give @bots only random any
  !give @all strip -grenades random rifle
  !give @all strip random pistol
  !give reset @all
```

### `sethp`

Set a player's current hp.  If set equal to or less than 0 the target will die.

Examples:

- `!sethp <target> 250` will set the target's current hp to 250
- `!sethp <target> +50` will add 50 to the target's current hp
- `!sethp <target> -50` will subtract 50 from the target's current hp
- `!sethp <target> 80 --save` will set the target's hp to 80 immediately and everytime they spawn as well




### `!setnv`

set a player's nightivision (if ProNightvision is loaded).

Usage:

```
  !setnv <target> on|normal|off|<filter_name>
  !setnv list
```

On defaults to normal.

Examples:

```
  !setnv help
  !setnv <target> normal
  !setnv <target> off
  !setnv <target> strong
  !setnv list
```

## Installation

Installation is pretty simple.  If using CSSDM, disable the `dm_equipment` plugin first.  Copy the .smx file to the plugins folder (e.g. `cstrike/addons/sourcemod/plugins`) and load the file using `sm plugins load`.

### Optional Installation

Custom menu entries can be added with a database.  You can skip this step if no custom menu entries are desired.  You can add up to 30 custom menu entries (that number seems gratuitous, but maybe there's a use case for that many).

To add custom menu entries add the following to`cstrike/addons/sourcemod/configs/databases.cfg`:

```
  "pro_menu"
	{
		"driver"			"default"
		"host"				"<hostname>"
		"database"		"<database>"
		"user"				"<username>"
		"pass"				"<password>"
	}
```

On first run the table will be automatically created (only tested with MySQL but should work with Postgres and SQLite too).

If you have problems you can manually create the table.  MySQL code:

```
CREATE TABLE `pro_menu` (
  `id` int(11) NOT NULL,
  `ordering` int(11) NOT NULL,
  `value` varchar(32) NOT NULL,
  `title` varchar(32) NOT NULL,
  `cmd` varchar(32) NOT NULL,
  `msg` varchar(256) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

ALTER TABLE `pro_menu`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `pro_menu_value_unique` (`value`);

ALTER TABLE `pro_menu`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
```

Each entry consists of: 
- an auto-incremented `id`
- an `ordering` field to specify the arbitrary order to sort the menu items by (lowest are displayed first)
- a `title` that will be displayed in the menu
- a `value` which is a unique value tied to that entry
- a `cmd` which will be the command that gets executed (do not include a ! or /)
- an optional message that gets printed to chat the first time the user selects the menu item (useful for informing the user they can also use the associated command directly).  Leave blank or set to null to disable the message.


## TODO

- Allow `!equip` to set infinite ammo and reserve ammo amount for rifles/pistols and grenades
- Allow `!setnv` to specify an intensity
- Maybe: refilling reserve ammo when empty
- Maybe: per-player weapon availabilities instead of just players or bots

