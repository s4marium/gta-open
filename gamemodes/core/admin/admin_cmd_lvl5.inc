ACMD:[5]setadmin(playerid, params[])
{
    new
        targetid,
        level;

    if(sscanf(params, "ui", targetid, level)) {
        return SendSyntaxMsg(playerid, "/setadmin [player] [level]");
    }

    if(IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "INVALID PLAYER!");
    }

    if(Admin_GetPlayerLevel(targetid) == 5) {
        return SendErrorMsg(playerid, "You can't demote a developer!");
    }

    if(Admin_GetPlayerLevel(targetid) == level) {
        return SendErrorMsg(playerid, "The player has currently at that admin position");
    }

    if(!IS_IN_RANGE(level, 0, MAX_ADMIN_LEVELS)) {
        return SendErrorMsgF(playerid, "The admin levels are 0 to %i", MAX_ADMIN_LEVELS);
    }

    Admin_SetPlayerLevel(targetid, level);
    SendMsgAllF(COLOR_MODERATION, "%p(%d)'s admin level is set to %s", targetid, targetid, Admin_GetRankName(targetid));
    return 1;
}

ACMD:[5]skick(playerid, params[])
{
    new target;
    if(sscanf(params, "u", target)) return SendSyntaxMsg(playerid, "/skick (name/id)");
    if(!IsPlayerConnected(target)) {
        return SendErrorMsg(playerid, "Player is not connected");
    }
    Kick(target);
    return 1;
}

ACMD:[5]mypos(playerid, params[]) {

    new angleString[128], Float:a;
    GetPlayerFacingAngle(playerid, a);
    format(angleString, sizeof(angleString), "Your angle is: %0.2f", a);
    SendMsgF(playerid, COLOR_MODERATION, angleString);

    new posString[128], Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    format(posString, sizeof(posString), "Your position is: %f, %f, %f", x, y, z);
    SendMsgF(playerid, COLOR_MODERATION, posString);
    return 1;
}

ACMD:[5]gotoco(playerid, params[])
{
	new Float: pos[3], int, vw;
	if(sscanf(params, "p<,>fffD(0)D(0)", pos[0], pos[1], pos[2], int, vw)) return SendSyntaxMsg(playerid, "USAGE: /gotoco [x coordinate] [y coordinate] [z coordinate] [interior] [vw]");

	SendMsgF(playerid, COLOR_MODERATION, "You have been teleported to the coordinates specified.");

	SetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	SetPlayerInterior(playerid, int);
	SetPlayerVirtualWorld(playerid, vw);
	return 1;
}
