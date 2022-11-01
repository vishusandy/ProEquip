# `!give`

## Overview

The `!give` command can be used to give/remove weapons.  It can also:

- save weapon preferences with `--save`

- force weapon preferences with `--force`, which also disables the menu for the specified weapon slot

- give random weapons

- give and remove weapons simultaneously using `strip` or `only`

- reset admin overrides back to defaults

## Syntax

```
  !give <target> <weapon> [--save|--force]
  !give <target> random [rifle|pistol|gun|grenade] [--save|--force]
  !give <target> remove [rifle|pistol|knife|<weapon>] [--save|--force]
  !give <target> only <weapon> [--save|--force]
  !give <target> strip [+|-grenades|rifles|pistols|knife] [<weapon>] [--save|--force]
  !give reset <target>
  !give help|list|examples
```

## Examples

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

## Detailed Explanation

- The `--save` flag will change the target's weapon preferences, `--force` will change preferences while also disabling the associated menu. This can be undone using `!give reset <target>`

- `only`: using `!give <target> only <weapon>` will remove all other weapons, including the knife and give the target the specified weapon. A weapon must be specified when using `only`.

- `strip`: use `!give <target> strip` strip all weapons except the knife. It can optionally give the target a specified weapon, like: `!give <target> strip scout`, which will strip everything but the knife and give the target a scout.

- Both `only` and `strip` can be used with options to change which weapons are removed.
  
  - `!give <target> strip -knifes +pistols -rifles -grenades` will remove everything but pistols.

- Giving a user only a specific grenade, like with `!give <target> only hegrenade` will give them an unlimited supply of the grenade, but one at a time.
  
  - This is done this way to avoid issues when the player only has one type of grenades but has multiple of them. If they throw one the game tries to switch to another weapon but they have no other weapons and then are unable to throw anymore of that grenade. If using CSSDM set `cssdm_refill_ammo "0"` in the `cssdm.cfg` for this to work properly.

- Using `!give <target> strip` gives more flexibility than `only` by allowing the admin to specify which weapon types will be removed (or added). Unlike `only`, the `strip` subcommand does not require a weapon name to be specified (can be used as just `!give <target> strip`). If no other weapon types are specified it will leave the user with only a knife and the specified weapon (if given). Weapon types can be removed
