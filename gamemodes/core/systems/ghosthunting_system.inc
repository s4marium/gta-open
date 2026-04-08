// Ghost hunting puzzle
//  Written by s4marium

#include <YSI_Coding\y_hooks>

// Ghost Hunting System Configuration
#define MAX_GHOSTS_PER_PLAYER       (5)             // Maximum ghosts to hunt in one session
#define MAX_GHOST_LOCATIONS         (5)             // Available ghost spawn locations
#define GHOST_SESSION_START_HOUR    (20)            // Night starts at 8 PM (20:00)
#define GHOST_SESSION_END_HOUR      (6)             // Night ends at 6 AM (06:00)
#define GHOST_DETECTION_RANGE       (50.0)          // Detection range for weapon camera
#define GHOST_CAPTURE_REWARD        (150)          // Money reward per capture
#define GHOST_CAPTURE_XP            (20)           // XP reward per capture
#define GHOST_ESCAPE_DISTANCE       (8.0)           // Distance at which ghost escapes
#define GHOST_OBJECT_ID             (3092)          // Ghost object model ID
#define GHOST_LABEL_DISTANCE        (30.0)          // 3D text label distance
#define GHOST_LABEL_HEIGHT          (2.0)           // 3D text label height offset
#define GHOST_DETECTION_INCREMENT   (5)             // Detection percentage per scan

// Message Strings
#define MSG_NIGHT_START             "[SERVER]"C_WHITE" The sun has set! Ghost hunting season has begun! Type /starthunt to begin hunting!"
#define MSG_HUNT_START              "[GHOST HUNT]"C_WHITE" A ghost hunting session has started! Catch ghosts until 6 AM!"
#define MSG_GHOST_LOCATION          "[GHOST HUNT]"C_WHITE" Ghost Location: %.2f, %.2f, %.2f"
#define MSG_ALL_GHOSTS_CAUGHT       "[GHOST HUNT]"C_WHITE" You have caught all 5 ghosts! Wait until 6 AM for session to end."
#define MSG_NO_MORE_GHOSTS          "[GHOST HUNT]"C_WHITE" No more ghosts available! Session will end soon."
#define MSG_GHOST_ESCAPED           "[GHOST HUNT]"C_WHITE" The ghost sensed your presence and vanished!"
#define MSG_DETECTION_PROGRESS      "[GHOST HUNT]"C_WHITE" Continue scanning! Detection: %i%%"
#define MSG_NIGHT_ENDED             "[GHOST HUNT]"C_WHITE" The sun has risen! Ghost hunting season has ended!"

// Ghost States
enum ghostStateType {
    GHOST_STATE_INACTIVE = 0,
    GHOST_STATE_SPAWNED = 1,
    GHOST_STATE_DETECTED = 2,
    GHOST_STATE_CAUGHT = 3
}

// Ghost Data Structure
enum ghostDataType {
    ghostID,
    ghostStateType:ghostState,
    Float:ghostX,
    Float:ghostY,
    Float:ghostZ,
    ghostObject,
    Text3D:ghostLabel,
    Timer:ghostTimer
}

// Player Ghost Hunt Data
enum playerGhostDataType {
    ghostsHunted,
    ghostsCaught,
    Timer:ghostSessionTimer,
    currentGhostID,
    huntDetectionLevel,
    Float:sessionRewardMoney,
    Float:sessionRewardXP
}

// Static Arrays
static ghostData[MAX_GHOST_LOCATIONS][ghostDataType];
static playerGhostData[MAX_PLAYERS][playerGhostDataType];
static ghostCounter = 0;
static BitArray:playerHunting<MAX_PLAYERS>;
static lastNightHour = -1;
static Float:ghostLocations[MAX_GHOST_LOCATIONS][3] = {
    {1099.113525, -1414.430541, 13.624711},
    {1264.079345, -1338.927124, 13.271038},
    {1075.156738, -1281.489624, 13.382812},
    {819.469787, -1412.805786, 13.492121},
    {656.12895, -1405.314331, 13.398592}
};

// ========== INITIALIZATION ==========

hook OnGameModeInit() {
    GhostHunting_Initialize();
    repeat GhostHunting_CheckNightTime[1000]();
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerConnect(playerid) {
    GhostHunting_ResetPlayerData(playerid);
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerDisconnect(playerid, reason) {
    if(Bit_Get(playerHunting, playerid)) {
        stop playerGhostData[playerid][ghostSessionTimer];
        Bit_Set(playerHunting, playerid, false);
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

GhostHunting_Initialize() {
    for(new i = 0; i < MAX_GHOST_LOCATIONS; i++) {
        ghostData[i][ghostState] = GHOST_STATE_INACTIVE;
        ghostData[i][ghostID] = i;
        ghostData[i][ghostObject] = INVALID_OBJECT_ID;
        ghostData[i][ghostLabel] = Text3D:INVALID_3DTEXT_ID;
    }
    ghostCounter = 0;
    lastNightHour = -1;
    print("[GHOST HUNTING] System initialized successfully!");
    return 1;
}

// ========== HELPER FUNCTIONS ==========

stock bool:GhostHunting_IsValidSlot(slotID) {
    return (slotID >= 0 && slotID < MAX_GHOST_LOCATIONS);
}

stock bool:GhostHunting_IsNight(hour) {
    return (hour >= GHOST_SESSION_START_HOUR || hour < GHOST_SESSION_END_HOUR);
}

stock bool:GhostHunting_IsDaytime(hour) {
    return (hour >= GHOST_SESSION_END_HOUR && hour < GHOST_SESSION_START_HOUR);
}

stock GhostHunting_NotifyNightStart() {
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(IsPlayerConnected(i)) {
            SendClientMessage(i, COLOR_ORANGE, MSG_NIGHT_START);
        }
    }
}

stock GhostHunting_ResetPlayerData(playerid) {
    playerGhostData[playerid][ghostsHunted] = 0;
    playerGhostData[playerid][ghostsCaught] = 0;
    playerGhostData[playerid][huntDetectionLevel] = 0;
    playerGhostData[playerid][currentGhostID] = -1;
    playerGhostData[playerid][sessionRewardMoney] = 0.0;
    playerGhostData[playerid][sessionRewardXP] = 0.0;
    Bit_Set(playerHunting, playerid, false);
}

stock Float:GhostHunting_GetDistance(playerid, ghostIdx) {
    new Float:px, Float:py, Float:pz;
    new Float:gx, Float:gy, Float:gz;

    GetPlayerPos(playerid, px, py, pz);
    gx = ghostData[ghostIdx][ghostX];
    gy = ghostData[ghostIdx][ghostY];
    gz = ghostData[ghostIdx][ghostZ];

    return floatsqroot(
        ((px - gx) * (px - gx)) +
        ((py - gy) * (py - gy)) +
        ((pz - gz) * (pz - gz))
    );
}

// ========== NIGHTTIME DETECTION ==========

timer GhostHunting_CheckNightTime[1000]() {
    new hour, minute, second;
    gettime(hour, minute, second);

    new isNightTime = GhostHunting_IsNight(hour);

    if(isNightTime && lastNightHour != hour) {
        lastNightHour = hour;
        GhostHunting_NotifyNightStart();
        print("[GHOST HUNTING] Night has fallen! Players can now start ghost hunting sessions!");
    }

    if(!isNightTime && lastNightHour != -1) {
        lastNightHour = -1;
    }
}

// ========== GHOST SPAWN & MANAGEMENT ==========

stock GhostHunting_SpawnGhost(slotID) {
    if(!GhostHunting_IsValidSlot(slotID)) {
        return 0;
    }

    new Float:x = ghostLocations[slotID][0];
    new Float:y = ghostLocations[slotID][1];
    new Float:z = ghostLocations[slotID][2];

    ghostData[slotID][ghostX] = x;
    ghostData[slotID][ghostY] = y;
    ghostData[slotID][ghostZ] = z;
    ghostData[slotID][ghostState] = GHOST_STATE_SPAWNED;

    ghostData[slotID][ghostObject] = CreateDynamicObject(
        GHOST_OBJECT_ID,
        x, y, z,
        0, 0, 0
    );

    ghostData[slotID][ghostLabel] = CreateDynamic3DTextLabelEx(
        "[GHOST]\n"C_WHITE"Use weapon camera to detect",
        COLOR_YELLOW,
        x, y, z + GHOST_LABEL_HEIGHT,
        GHOST_LABEL_DISTANCE,
        .testlos = 0
    );

    ghostCounter++;
    return 1;
}

stock GhostHunting_DespawnGhost(slotID) {
    if(!GhostHunting_IsValidSlot(slotID)) {
        return 0;
    }

    if(ghostData[slotID][ghostObject] != INVALID_OBJECT_ID) {
        DestroyDynamicObject(ghostData[slotID][ghostObject]);
        ghostData[slotID][ghostObject] = INVALID_OBJECT_ID;
    }

    if(ghostData[slotID][ghostLabel] != Text3D:INVALID_3DTEXT_ID) {
        DestroyDynamic3DTextLabel(ghostData[slotID][ghostLabel]);
        ghostData[slotID][ghostLabel] = Text3D:INVALID_3DTEXT_ID;
    }

    ghostData[slotID][ghostState] = GHOST_STATE_INACTIVE;
    ghostCounter--;
    return 1;
}

stock GhostHunting_GetInactiveGhostSlot() {
    new inactiveGhosts[MAX_GHOST_LOCATIONS];
    new inactiveCount = 0;

    for(new i = 0; i < MAX_GHOST_LOCATIONS; i++) {
        if(ghostData[i][ghostState] == GHOST_STATE_INACTIVE) {
            inactiveGhosts[inactiveCount++] = i;
        }
    }

    if(inactiveCount == 0) return -1;
    return inactiveGhosts[random(inactiveCount)];
}

// ========== GHOST DETECTION & HUNTING ==========

stock GhostHunting_StartHunt(playerid) {
    if(Bit_Get(playerHunting, playerid)) {
        return SendErrorMsg(playerid, "You are already on a ghost hunt!");
    }

    new ghostSlot = GhostHunting_GetInactiveGhostSlot();

    if(ghostSlot == -1) {
        return SendErrorMsg(playerid, "No ghosts available to hunt right now!");
    }

    GhostHunting_SpawnGhost(ghostSlot);
    Bit_Set(playerHunting, playerid, true);
    playerGhostData[playerid][currentGhostID] = ghostSlot;
    playerGhostData[playerid][huntDetectionLevel] = 0;
    playerGhostData[playerid][ghostsHunted] = 0;
    playerGhostData[playerid][ghostsCaught] = 0;
    playerGhostData[playerid][sessionRewardMoney] = 0.0;
    playerGhostData[playerid][sessionRewardXP] = 0.0;
    playerGhostData[playerid][ghostSessionTimer] = repeat OnGhostSessionUpdate[500](playerid);

    SendClientMessage(playerid, COLOR_ORANGE, MSG_HUNT_START);

    new locMsg[128];
    format(locMsg, sizeof(locMsg), MSG_GHOST_LOCATION,
        ghostData[ghostSlot][ghostX],
        ghostData[ghostSlot][ghostY],
        ghostData[ghostSlot][ghostZ]);
    SendClientMessage(playerid, COLOR_ORANGE, locMsg);

    return 1;
}

stock GhostHunting_DetectGhost(playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        return 0;
    }

    new currentGhost = playerGhostData[playerid][currentGhostID];

    if(!GhostHunting_IsValidSlot(currentGhost)) {
        return 0;
    }

    new Float:detDist = GhostHunting_GetDistance(playerid, currentGhost);

    if(detDist <= GHOST_DETECTION_RANGE) {
        if(playerGhostData[playerid][huntDetectionLevel] < 100) {
            playerGhostData[playerid][huntDetectionLevel] += GHOST_DETECTION_INCREMENT;
        }

        if(playerGhostData[playerid][huntDetectionLevel] > 100) {
            playerGhostData[playerid][huntDetectionLevel] = 100;
        }

        return 1;
    }

    return 0;
}

stock GhostHunting_CaptureGhost(playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        return SendErrorMsg(playerid, "You are not on a ghost hunt!");
    }

    new targetGhost = playerGhostData[playerid][currentGhostID];

    if(!GhostHunting_IsValidSlot(targetGhost)) {
        return SendErrorMsg(playerid, "Ghost error!");
    }

    new detectionLvl = playerGhostData[playerid][huntDetectionLevel];

    if(detectionLvl < 100) {
        new detMsg[128];
        format(detMsg, sizeof(detMsg),
            "[GHOST HUNT]"C_WHITE" You need to detect the ghost fully first! Detection: %i%%",
            detectionLvl);
        SendClientMessage(playerid, COLOR_RED, detMsg);
        return 0;
    }

    playerGhostData[playerid][ghostsCaught]++;
    playerGhostData[playerid][ghostsHunted]++;
    playerGhostData[playerid][sessionRewardMoney] += GHOST_CAPTURE_REWARD;
    playerGhostData[playerid][sessionRewardXP] += GHOST_CAPTURE_XP;

    // Give rewards immediately to player
    GivePlayerMoney(playerid, GHOST_CAPTURE_REWARD);
    GivePlayerXp(playerid, GHOST_CAPTURE_XP);

    new rewardMsg[128];
    format(rewardMsg, sizeof(rewardMsg),
        "[GHOST HUNT] "C_GREEN"SUCCESS! "C_WHITE"You captured a ghost! +$%i | +%i XP",
        GHOST_CAPTURE_REWARD, GHOST_CAPTURE_XP);
    SendClientMessage(playerid, COLOR_GREEN, rewardMsg);

    ApplyAnimation(playerid, "CAMERA", "camcrane_end", 4.0, false, false, false, true, 0, SYNC_ALL);
    GhostHunting_DespawnGhost(targetGhost);
    GhostHunting_SpawnNextGhost(playerid);

    return 1;
}

stock GhostHunting_EndSession(playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        return 0;
    }

    new sessionGhost = playerGhostData[playerid][currentGhostID];

    if(GhostHunting_IsValidSlot(sessionGhost)) {
        GhostHunting_DespawnGhost(sessionGhost);
    }

    stop playerGhostData[playerid][ghostSessionTimer];

    new totalGhosts = playerGhostData[playerid][ghostsCaught];

    if(totalGhosts > 0) {
        new endMsg[256];
        format(endMsg, sizeof(endMsg),
            "[GHOST HUNT] "C_GREEN"SESSION ENDED! "C_WHITE"You caught %i ghost(s) and earned all rewards!",
            totalGhosts);
        SendClientMessage(playerid, COLOR_GREEN, endMsg);
    } else {
        SendClientMessage(playerid, COLOR_RED,
            "[GHOST HUNT]"C_WHITE" Session ended! You didn't catch any ghosts.");
    }

    Bit_Set(playerHunting, playerid, false);
    GhostHunting_ResetPlayerData(playerid);

    return 1;
}

stock GhostHunting_SpawnNextGhost(playerid) {
    if(playerGhostData[playerid][ghostsCaught] >= MAX_GHOSTS_PER_PLAYER) {
        SendClientMessage(playerid, COLOR_ORANGE, MSG_ALL_GHOSTS_CAUGHT);
        return 0;
    }

    new spawnSlot = GhostHunting_GetInactiveGhostSlot();

    if(spawnSlot == -1) {
        SendClientMessage(playerid, COLOR_RED, MSG_NO_MORE_GHOSTS);
        return 0;
    }

    GhostHunting_SpawnGhost(spawnSlot);
    playerGhostData[playerid][currentGhostID] = spawnSlot;
    playerGhostData[playerid][huntDetectionLevel] = 0;

    new ghostNum = playerGhostData[playerid][ghostsCaught] + 1;
    new numMsg[128];
    format(numMsg, sizeof(numMsg),
        "[GHOST HUNT]"C_WHITE" New ghost spawned! Ghost: %i/%i",
        ghostNum, MAX_GHOSTS_PER_PLAYER);
    SendClientMessage(playerid, COLOR_YELLOW, numMsg);

    new locMsg2[128];
    format(locMsg2, sizeof(locMsg2), MSG_GHOST_LOCATION,
        ghostData[spawnSlot][ghostX],
        ghostData[spawnSlot][ghostY],
        ghostData[spawnSlot][ghostZ]);
    SendClientMessage(playerid, COLOR_ORANGE, locMsg2);

    return 1;
}

// ========== SESSION UPDATE TIMER ==========

timer OnGhostSessionUpdate[500](playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        stop playerGhostData[playerid][ghostSessionTimer];
        return 0;
    }

    new hour, minute, second;
    gettime(hour, minute, second);

    if(GhostHunting_IsDaytime(hour)) {
        SendClientMessage(playerid, COLOR_ORANGE, MSG_NIGHT_ENDED);
        GhostHunting_EndSession(playerid);
        stop playerGhostData[playerid][ghostSessionTimer];
        return 0;
    }

    new timerGhost = playerGhostData[playerid][currentGhostID];

    if(GhostHunting_IsValidSlot(timerGhost)) {
        new Float:playerX, Float:playerY, Float:playerZ;
        new Float:ghostPosX, Float:ghostPosY, Float:ghostPosZ;

        GetPlayerPos(playerid, playerX, playerY, playerZ);
        ghostPosX = ghostData[timerGhost][ghostX];
        ghostPosY = ghostData[timerGhost][ghostY];
        ghostPosZ = ghostData[timerGhost][ghostZ];

        new Float:distToGhost = floatsqroot(
            ((playerX - ghostPosX) * (playerX - ghostPosX)) +
            ((playerY - ghostPosY) * (playerY - ghostPosY)) +
            ((playerZ - ghostPosZ) * (playerZ - ghostPosZ))
        );

        if(distToGhost < GHOST_ESCAPE_DISTANCE) {
            SendClientMessage(playerid, COLOR_RED, MSG_GHOST_ESCAPED);
            GhostHunting_DespawnGhost(timerGhost);

            if(!GhostHunting_SpawnNextGhost(playerid)) {
                GhostHunting_EndSession(playerid);
            }
            return 1;
        }
    }

    if(GetPlayerWeapon(playerid) == WEAPON_CAMERA && GhostHunting_IsValidSlot(timerGhost)) {
        GhostHunting_DetectGhost(playerid);
    }

    if(playerGhostData[playerid][ghostsCaught] < MAX_GHOSTS_PER_PLAYER) {
        new hudDetection = playerGhostData[playerid][huntDetectionLevel];
        new totalHunted = playerGhostData[playerid][ghostsHunted];
        new totalCaught = playerGhostData[playerid][ghostsCaught];
        new hudRewards = floatround(playerGhostData[playerid][sessionRewardMoney]);

        new hud[256];
        format(hud, sizeof(hud),
            "~w~Ghost %i/%i~n~Detection: ~r~%i%%~w~ | Time: ~g~%i:%02d~w~~n~Caught: ~b~%i~w~ | Earned: ~g~$%i",
            totalHunted,
            MAX_GHOSTS_PER_PLAYER,
            hudDetection,
            hour,
            minute,
            totalCaught,
            hudRewards
        );

        ShowPlayerPopup(playerid, "~y~[GHOST HUNT SESSION]", hud, 500);
    } else {
        // All ghosts caught - clear the HUD popup
        GameFeed_ResetAllTextDraws(playerid);
    }

    return 1;
}

// ========== WEAPON FIRE HANDLER ==========

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if((newkeys & KEY_FIRE) && !(oldkeys & KEY_FIRE)) {
        if(GetPlayerWeapon(playerid) == WEAPON_CAMERA && Bit_Get(playerHunting, playerid)) {
            new fireGhostID = playerGhostData[playerid][currentGhostID];

            if(!GhostHunting_IsValidSlot(fireGhostID)) {
                return 1;
            }

            new Float:playerDist = GhostHunting_GetDistance(playerid, fireGhostID);
            new fireDet = playerGhostData[playerid][huntDetectionLevel];

            if(playerDist <= GHOST_DETECTION_RANGE) {
                if(fireDet >= 100) {
                    SendClientMessage(playerid, COLOR_GREEN,
                        "[GHOST HUNT]"C_WHITE" Ghost DETECTED! Capturing now...");
                    ApplyAnimation(playerid, "CAMERA", "camcrane_end", 4.0, false, false, false, true, 0, SYNC_ALL);
                    GhostHunting_CaptureGhost(playerid);
                } else {
                    new fireMsg[128];
                    format(fireMsg, sizeof(fireMsg), MSG_DETECTION_PROGRESS, fireDet);
                    SendClientMessage(playerid, COLOR_YELLOW, fireMsg);
                }
            }
        }
    }

    return 1;
}

// ========== GETTERS / STATISTICS ==========

stock GhostHunting_GetGhostsCaught(playerid) {
    return playerGhostData[playerid][ghostsCaught];
}

stock GhostHunting_GetGhostsHunted(playerid) {
    return playerGhostData[playerid][ghostsHunted];
}

stock GhostHunting_GetDetectionLevel(playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        return 0;
    }
    return playerGhostData[playerid][huntDetectionLevel];
}

stock bool:GhostHunting_IsHunting(playerid) {
    return Bit_Get(playerHunting, playerid);
}

stock GhostHunting_GetGhostCount() {
    return ghostCounter;
}

// ========== COMMAND HANDLERS ==========
/*
stock Command_StartHunt(const playerid) {
    new hour, minute, second;
    gettime(hour, minute, second);

    if(!GhostHunting_IsNight(hour)) {
        SendErrorMsg(playerid, "Ghost hunting is only available at night (8 PM - 6 AM)!");
        return 0;
    }

    if(Bit_Get(playerHunting, playerid)) {
        SendErrorMsg(playerid, "You are already on a ghost hunt!");
        return 0;
    }

    GhostHunting_StartHunt(playerid);
    return 1;
}

stock Command_LeaveHunt(const playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        SendErrorMsg(playerid, "You are not currently on a ghost hunt!");
        return 0;
    }

    SendClientMessage(playerid, COLOR_ORANGE,
        "[GHOST HUNT]"C_WHITE" You have left the ghost hunting session.");
    GhostHunting_EndSession(playerid);
    return 1;
} */
