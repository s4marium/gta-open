#include <YSI_Coding\y_hooks>

static GPS_ShowCategory(playerid, category);

static PlayerGPSCheckpoint[MAX_PLAYERS] = { -1, ... };

hook OnPlayerConnect(playerid) {
    PlayerGPSCheckpoint[playerid] = -1;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    if(IsValidDynamicCP(PlayerGPSCheckpoint[playerid])) {
        DestroyDynamicCP(PlayerGPSCheckpoint[playerid]);
    }
    PlayerGPSCheckpoint[playerid] = -1;
    return 1;
}

static GPS_ClearLocation(playerid) {
    if(IsValidDynamicCP(PlayerGPSCheckpoint[playerid])) {
        DestroyDynamicCP(PlayerGPSCheckpoint[playerid]);
    }
    PlayerGPSCheckpoint[playerid] = -1;
}

static GPS_SetLocation(playerid, Float:x, Float:y, Float:z, const string:msg[]) {
    GPS_ClearLocation(playerid);
    PlayerGPSCheckpoint[playerid] = CreateDynamicCP(x, y, z, 5.0, .worldid = -1, .interiorid = -1, .playerid = playerid, .streamdistance = 99999.0);
    SendServerMsg(playerid, msg);
}

CMD:gps(playerid, params[]) {
    inline _response(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, inputtext
        if (!response) {
            return 0;
        }

        GPS_ShowCategory(pid, listitem);
    }
    
    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        ""C_ORANGE"GPS - Select Category",
        "Public Locations\nJobs\n24/7 Stores\nAmmu-Nations\nGas Stations\nClear GPS",
        "Select",
        "Close"
    );
    return 1;
}

static GPS_ShowCategory(playerid, category) {
    if (category == 0) { // Public Locations
        inline _response(pid, dialogid, response, listitem, string:inputtext[]) {
            #pragma unused dialogid, inputtext
            if (!response) {
                CommandExecute(pid, "gps");
                return 0;
            }
            switch(listitem) {
                case 0: { GPS_SetLocation(pid, 1481.0377, -1772.3126, 18.7957, "GPS set to City Hall."); }
                case 1: { GPS_SetLocation(pid, 595.6651, -1250.7883, 18.2999, "GPS set to Bank."); }
                case 2: { GPS_SetLocation(pid, 1555.5012, -1675.6390, 16.1953, "GPS set to LSPD."); }
                case 3: { GPS_SetLocation(pid, 2229.9028, -1721.2580, 13.5612, "GPS set to Gym."); }
            }
        }
        Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_LIST, ""C_ORANGE"GPS - Public Locations", "City Hall\nBank\nLSPD\nGym", "Set", "Back");
    }
    else if (category == 1) { // Jobs
        inline _response(pid, dialogid, response, listitem, string:inputtext[]) {
            #pragma unused dialogid, inputtext
            if (!response) {
                CommandExecute(pid, "gps");
                return 0;
            }
            switch(listitem) {
                case 0: { GPS_SetLocation(pid, 2432.5537, -2115.6113, 13.5468, "GPS set to Garbage Collector."); }
                case 1: { GPS_SetLocation(pid, 2115.5190, -1788.4954, 13.5547, "GPS set to Pizza Delivery."); }
                case 2: { GPS_SetLocation(pid, 2174.6182, -2266.6165, 13.3797, "GPS set to Trucker."); }
                case 3: { GPS_SetLocation(pid, 1659.9645, -1546.2712, 12.8828, "GPS set to Sweeper."); }
            }
        }
        Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_LIST, ""C_ORANGE"GPS - Jobs", "Garbage Collector\nPizza Delivery\nTrucker\nSweeper", "Set", "Back");
    }
    else if (category == 2) { // 24/7 Stores
        inline _response(pid, dialogid, response, listitem, string:inputtext[]) {
            #pragma unused dialogid, inputtext
            if (!response) { CommandExecute(pid, "gps"); return 0; }
            switch(listitem) {
                case 0: { GPS_SetLocation(pid, 1833.7773, -1842.6236, 13.5781, "GPS set to 24/7 (Idlewood)."); }
                case 1: { GPS_SetLocation(pid, 1000.5930, -919.9168, 42.3281, "GPS set to 24/7 (Temple)."); }
            }
        }
        Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_LIST, ""C_ORANGE"GPS - 24/7 Stores", "24/7 (Idlewood)\n24/7 (Temple)", "Set", "Back");
    }
    else if (category == 3) { // Ammu-Nation
        inline _response(pid, dialogid, response, listitem, string:inputtext[]) {
            #pragma unused dialogid, inputtext
            if (!response) { CommandExecute(pid, "gps"); return 0; }
            switch(listitem) {
                case 0: { GPS_SetLocation(pid, 2400.4934, -1981.9956, 13.5468, "GPS set to Ammu-Nation (Willowfield)."); }
                case 1: { GPS_SetLocation(pid, 1369.0001, -1279.7126, 13.5468, "GPS set to Ammu-Nation (Market)."); }
            }
        }
        Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_LIST, ""C_ORANGE"GPS - Ammu-Nations", "Ammu-Nation (Willowfield)\nAmmu-Nation (Market)", "Set", "Back");
    }
    else if (category == 4) { // Gas Stations
        inline _response(pid, dialogid, response, listitem, string:inputtext[]) {
            #pragma unused dialogid, inputtext
            if (!response) { CommandExecute(pid, "gps"); return 0; }
            switch(listitem) {
                case 0: { GPS_SetLocation(pid, 1928.5809, -1776.2648, 13.5468, "GPS set to Gas Station (Idlewood)."); }
                case 1: { GPS_SetLocation(pid, -78.3608, -1169.8706, 2.1355, "GPS set to Gas Station (Flint County)."); }
            }
        }
        Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_LIST, ""C_ORANGE"GPS - Gas Stations", "Gas Station (Idlewood)\nGas Station (Flint County)", "Set", "Back");
    }
    else if (category == 5) { // Clear GPS
        GPS_ClearLocation(playerid);
        SendServerMsg(playerid, "GPS checkpoint cleared.");
    }
    return 1;
}

CMD:cleargps(playerid, params[]) {
    GPS_ClearLocation(playerid);
    SendServerMsg(playerid, "GPS checkpoint cleared.");
    return 1;
}
