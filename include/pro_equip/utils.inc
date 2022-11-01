#if defined _pro_utils_included
    #endinput
#endif
#define _pro_utils_included

stock void StringToLower(char[] string) {
    int len = strlen(string);
    for(int i=0; i < len; i++) {
        string[i] = CharToLower(string[i]);
    }
}

stock int clamp(int val, int min, int max) {
    return (val < min)? min:(val > max)? max: val;
}

// Source: https://forums.alliedmods.net/showthread.php?t=191743
stock void AddComma( String: sString[ ], iSize, iPosition ) {
    new String: sTemp[ 32 ];
    FormatEx( sTemp, iPosition + 1, "%s", sString ), Format( sTemp, 31, "%s,", sTemp ), Format( sTemp, 31, "%s%s", sTemp, sString[ iPosition ] ), FormatEx( sString, iSize, "%s", sTemp );
}
stock void NumberFormat( iNumber, String: sNumber[ ], iSize ) {
    new bool: bNegative = iNumber < 0 ? true : false;
    if( bNegative ) iNumber *= -1;
    FormatEx( sNumber, iSize, "%d", iNumber );
    for( new i = strlen( sNumber ) - 3; i > 0; i -= 3 ) AddComma( sNumber, iSize, i );
    if( bNegative ) Format( sNumber, iSize, "-%s", sNumber );
}
