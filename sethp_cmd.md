# `sethp`

Set a player's hp. If set equal to or less than 0 the target will die.

```
!sethp <target> [+|-]<amount> [--save]
```

## Examples

- `!sethp <target> 250` will set the target's current hp to 250
- `!sethp <target> +50` will add 50 to the target's current hp
- `!sethp <target> -50` will subtract 50 from the target's current hp
- `!sethp <target> 80 --save` will set the target's hp to 80 immediately and every time they spawn as well
