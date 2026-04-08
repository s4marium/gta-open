// keymash - Mash the key to fill the bar before it decays
// Written by s4marium

#include <YSI_Coding\y_hooks>

// ====== KEYMASH MINIGAME CONSTANTS ======
#define KEYMASH_PROGRESS_MAX 100
#define KEYMASH_PROGRESS_INCREMENT 5
#define KEYMASH_PROGRESS_DECAY 10
#define KEYMASH_KEY_COOLDOWN 150 // Milliseconds between key presses
#define KEYMASH_DECAY_INTERVAL 1000 // Milliseconds between decay ticks
#define KEYMASH_MAX_TIME 60000 // 60 seconds max
#define KEYMASH_ANIMATION_INTERVAL 1000 // 1 second between animation loops

// ====== KEYMASH MINIGAME TYPES ======
#define KEYMASH_TYPE_HARVEST 1
#define KEYMASH_TYPE_COLLECTION 2
#define KEYMASH_TYPE_MINING 3

// ====== KEYMASH MINIGAME ENUMS ======
enum E_KEYMASH_STATE {
     bool:IsActive,
     PlayerID,
     Type,
     CustomData,
     Progress,
     LastKeyTime,
     DecayTimer,
     TimeoutTimer,
     AnimationTimer
};

// ====== KEYMASH MINIGAME VARIABLES ======
new KeymashState[MAX_PLAYERS][E_KEYMASH_STATE];

// Textdraw UI
new PlayerBar:KeymashProgressBar[MAX_PLAYERS];
new PlayerText:KeymashTitle[MAX_PLAYERS];
new PlayerText:KeymashInstructions[MAX_PLAYERS];

// ====== FORWARD DECLARATIONS ======
forward public Keymash_InitPlayer(playerid);
forward public Keymash_OnKeyStateChange(playerid, newkeys, oldkeys);
forward public Keymash_Start(playerid, type, customData, const title[], const instructions[]);
forward public Keymash_End(playerid, bool:success);
forward public Keymash_OnDecay(playerid);
forward public Keymash_OnTimeout(playerid);
forward public Keymash_LoopAnimation(playerid);

// ====== INITIALIZATION ======
hook OnGameModeInit() {
    LoadKeymashMinigame();
    return 1;
}

LoadKeymashMinigame() {
    print("[KEYMASH] Mini-game system loaded successfully!");
    return 1;
}

hook OnPlayerConnect(playerid) {
    Keymash_InitPlayer(playerid);
    return 1;
}

public Keymash_InitPlayer(playerid) {
    KeymashState[playerid][IsActive] = false;
    KeymashState[playerid][Progress] = 0;
    KeymashState[playerid][LastKeyTime] = 0;
    KeymashState[playerid][DecayTimer] = 0;
    KeymashState[playerid][TimeoutTimer] = 0;
    KeymashState[playerid][AnimationTimer] = 0;

    // Create progress bar
    KeymashProgressBar[playerid] = CreatePlayerProgressBar(playerid, 272.0, 363.0, 107.5, 9.2, -1347440641, 100.0, 0);

    // Create title textdraw
    KeymashTitle[playerid] = CreatePlayerTextDraw(playerid, 270.0, 350.0, "[KEYMASH]");
    PlayerTextDrawBackgroundColor(playerid, KeymashTitle[playerid], 255);
    PlayerTextDrawFont(playerid, KeymashTitle[playerid], TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, KeymashTitle[playerid], 0.249999, 1.100000);
    PlayerTextDrawColor(playerid, KeymashTitle[playerid], -1);
    PlayerTextDrawSetOutline(playerid, KeymashTitle[playerid], 0);
    PlayerTextDrawSetProportional(playerid, KeymashTitle[playerid], true);
    PlayerTextDrawSetShadow(playerid, KeymashTitle[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, KeymashTitle[playerid], false);

    // Create instructions textdraw below progress bar
    KeymashInstructions[playerid] = CreatePlayerTextDraw(playerid, 270.0, 375.0, "Press 'SPACE' to mash");
    PlayerTextDrawBackgroundColor(playerid, KeymashInstructions[playerid], 255);
    PlayerTextDrawFont(playerid, KeymashInstructions[playerid], TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, KeymashInstructions[playerid], 0.249999, 1.000000);
    PlayerTextDrawColor(playerid, KeymashInstructions[playerid], -1);
    PlayerTextDrawSetOutline(playerid, KeymashInstructions[playerid], 0);
    PlayerTextDrawSetProportional(playerid, KeymashInstructions[playerid], true);
    PlayerTextDrawSetShadow(playerid, KeymashInstructions[playerid], 1);
    PlayerTextDrawSetSelectable(playerid, KeymashInstructions[playerid], false);

    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    Keymash_OnKeyStateChange(playerid, newkeys, oldkeys);
    return 1;
}

public Keymash_OnKeyStateChange(playerid, newkeys, oldkeys) {
    if(!KeymashState[playerid][IsActive]) return 0;

    // Check for SPACE key press
    if(PRESSED(KEY_SPRINT)) {
        new currentTime = GetTickCount();

        // Check key cooldown
        if(currentTime - KeymashState[playerid][LastKeyTime] >= KEYMASH_KEY_COOLDOWN) {
            KeymashState[playerid][LastKeyTime] = currentTime;
            Keymash_UpdateProgress(playerid, KEYMASH_PROGRESS_INCREMENT);
        }
    }

    return 1;
}

// ====== PUBLIC FUNCTIONS ======
public Keymash_Start(playerid, type, customData, const title[], const instructions[]) {
    if(KeymashState[playerid][IsActive]) return 0;

    // Initialize state
    KeymashState[playerid][IsActive] = true;
    KeymashState[playerid][PlayerID] = playerid;
    KeymashState[playerid][Type] = type;
    KeymashState[playerid][CustomData] = customData;
    KeymashState[playerid][Progress] = 0;
    KeymashState[playerid][LastKeyTime] = GetTickCount();

    // Lock player
    TogglePlayerControllable(playerid, false);

    // Show UI (progress bar, title, and instructions)
    PlayerTextDrawSetString(playerid, KeymashTitle[playerid], title);
    PlayerTextDrawShow(playerid, KeymashTitle[playerid]);
    PlayerTextDrawSetString(playerid, KeymashInstructions[playerid], instructions);
    PlayerTextDrawShow(playerid, KeymashInstructions[playerid]);
    ShowPlayerProgressBar(playerid, KeymashProgressBar[playerid]);
    SetPlayerProgressBarValue(playerid, KeymashProgressBar[playerid], 0.0);

    // Start decay timer
    KeymashState[playerid][DecayTimer] = SetTimerEx("Keymash_OnDecay", KEYMASH_DECAY_INTERVAL, true, "i", playerid);

    // Start timeout timer
    KeymashState[playerid][TimeoutTimer] = SetTimerEx("Keymash_OnTimeout", KEYMASH_MAX_TIME, false, "i", playerid);

    // Start animation loop for harvest type
    if(type == KEYMASH_TYPE_HARVEST) {
        KeymashState[playerid][AnimationTimer] = SetTimerEx("Keymash_LoopAnimation", KEYMASH_ANIMATION_INTERVAL, true, "i", playerid);
    }

    // Update display
    Keymash_UpdateDisplay(playerid);

    return 1;
}

static Keymash_UpdateProgress(playerid, amount) {
    if(!KeymashState[playerid][IsActive]) return;

    KeymashState[playerid][Progress] += amount;

    if(KeymashState[playerid][Progress] >= KEYMASH_PROGRESS_MAX) {
        KeymashState[playerid][Progress] = KEYMASH_PROGRESS_MAX;
        Keymash_UpdateDisplay(playerid);
        Keymash_Complete(playerid);
        return;
    }

    Keymash_UpdateDisplay(playerid);
}

public Keymash_OnDecay(playerid) {
    if(!KeymashState[playerid][IsActive]) return 0;

    KeymashState[playerid][Progress] -= KEYMASH_PROGRESS_DECAY;

    // Check if progress reached 0 (failure)
    if(KeymashState[playerid][Progress] <= 0) {
        KeymashState[playerid][Progress] = 0;
        Keymash_UpdateDisplay(playerid);
        Keymash_Fail(playerid);
        return 0;
    }

    Keymash_UpdateDisplay(playerid);
    return 1;
}

static Keymash_UpdateDisplay(playerid) {
    // Update progress bar value (0.0 to 100.0)
    new Float:barValue = float(KeymashState[playerid][Progress]);
    SetPlayerProgressBarValue(playerid, KeymashProgressBar[playerid], barValue);
}

static Keymash_Complete(playerid) {
    Keymash_End(playerid, true);
}

static Keymash_Fail(playerid) {
    Alert_Send(playerid, "~r~ERROR:~w~ You were too slow! Collection failed.", 3000);
    Keymash_End(playerid, false);
}

public Keymash_End(playerid, bool:success) {
    if(!KeymashState[playerid][IsActive]) return 0;

    // Kill timers
    if(KeymashState[playerid][DecayTimer]) {
        KillTimer(KeymashState[playerid][DecayTimer]);
        KeymashState[playerid][DecayTimer] = 0;
    }
    if(KeymashState[playerid][TimeoutTimer]) {
        KillTimer(KeymashState[playerid][TimeoutTimer]);
        KeymashState[playerid][TimeoutTimer] = 0;
    }
    if(KeymashState[playerid][AnimationTimer]) {
        KillTimer(KeymashState[playerid][AnimationTimer]);
        KeymashState[playerid][AnimationTimer] = 0;
    }

    // Hide UI
    PlayerTextDrawHide(playerid, KeymashTitle[playerid]);
    PlayerTextDrawHide(playerid, KeymashInstructions[playerid]);
    HidePlayerProgressBar(playerid, KeymashProgressBar[playerid]);
    SetPlayerProgressBarValue(playerid, KeymashProgressBar[playerid], 0.0);

    // Store state before clearing
    new type = KeymashState[playerid][Type];
    new customData = KeymashState[playerid][CustomData];

    // Reset state
    KeymashState[playerid][IsActive] = false;
    KeymashState[playerid][Progress] = 0;

    // Unlock player & clear animations
    TogglePlayerControllable(playerid, true);
    ClearAnimations(playerid, true);

    // Call appropriate callback based on type
    if(success) {
        if(type == KEYMASH_TYPE_HARVEST) {
            Orange_MiniGame_OnComplete(playerid, customData);
        }
    } else {
        if(type == KEYMASH_TYPE_HARVEST) {
            Orange_MiniGame_OnFail(playerid, customData);
        }
    }

    return 1;
}

public Keymash_OnTimeout(playerid) {
    if(!KeymashState[playerid][IsActive]) return 0;

    Alert_Send(playerid, "~r~ERROR:~w~ Time's up!", 3000);
    Keymash_End(playerid, false);
    return 1;
}

public Keymash_LoopAnimation(playerid) {
    if(!KeymashState[playerid][IsActive]) return 0;

    // Reapply harvest animation to keep it looping
    ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.0, true, false, false, false, 0, 1);
    return 1;
}

// ====== HELPER FUNCTIONS ======
stock bool:Keymash_IsPlaying(playerid) {
    return KeymashState[playerid][IsActive];
}

stock Keymash_GetProgress(playerid) {
    return KeymashState[playerid][Progress];
}
