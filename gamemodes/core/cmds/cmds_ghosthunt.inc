CMD:starthunt(playerid, params[])
{
    new hour, minute, second;
    gettime(hour, minute, second);

    // Check if it's night time (8 PM - 6 AM)
    if(!(hour >= 20 || hour < 6)) {
        return SendErrorMsg(playerid, "Ghost hunting is only available at night (8 PM - 6 AM)!");
    }

    if(GhostHunting_IsHunting(playerid)) {
        return SendErrorMsg(playerid, "You are already on a ghost hunt!");
    }

    if(!GhostHunting_StartHunt(playerid)) {
        return SendErrorMsg(playerid, "Failed to start ghost hunt!");
    }

    return 1;
}

CMD:leavehunt(playerid, params[])
{
    if(!GhostHunting_IsHunting(playerid)) {
        return SendErrorMsg(playerid, "You are not currently on a ghost hunt!");
    }

    SendClientMessage(playerid, COLOR_ORANGE, "[GHOST HUNT]"C_WHITE" You have left the ghost hunting session.");
    GhostHunting_EndSession(playerid);

    return 1;
}
