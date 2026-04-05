
/*
 todo: - Add level up textdraw for congratulations (e.g. textdraw, sound, etc.)
 */

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define COLOR_XP_RED -8388353
#define COLOR_XP_TEXT -1
#define COLOR_XP_GAIN -8388353

#define XP_BAR_X 207.0
#define XP_BAR_Y 20.0
#define XP_BAR_WIDTH 235.0
#define XP_BAR_HEIGHT 6.0
#define XP_BAR_COLOR -1811885057

#define XP_TEXT_LEVEL_CURR_X 194.0
#define XP_TEXT_LEVEL_CURR_Y 13.0

#define XP_TEXT_LEVEL_NEXT_X 451.0
#define XP_TEXT_LEVEL_NEXT_Y 13.0

#define XP_TEXT_PROGRESS_X 323.0
#define XP_TEXT_PROGRESS_Y 29.0

#define XP_TEXT_GAIN_X 293.0
#define XP_TEXT_GAIN_Y 220.0

#define XP_TEXT_AMOUNT_X 305.0
#define XP_TEXT_AMOUNT_Y 240.0

// =============================================================================
// TYPES
// =============================================================================

enum E_XP_UI_DATA
{
	PlayerBar:xpBar,
	PlayerText:levelCurrentText,
	PlayerText:levelNextText,
	PlayerText:progressText,
	PlayerText:gainText,
	PlayerText:amountText
};

// =============================================================================
// STATIC VARIABLES
// =============================================================================

static XpUIData[MAX_PLAYERS][E_XP_UI_DATA];
static bool:XpUICreated[MAX_PLAYERS];

// =============================================================================
// HOOKS
// =============================================================================

hook OnPlayerConnect(playerid) {
	XpUICreated[playerid] = false;
	return 1;
}

hook OnPlayerSpawn(playerid) {
	if(!XpUICreated[playerid]) {
		XpUI_Create(playerid);
	}
	return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
	XpUI_Destroy(playerid);
	return 1;
}

hook OnPlayerSecondUpdate(playerid) {
	if(Player_IsSpawned(playerid) && XpUICreated[playerid]) {
		XpUI_Update(playerid);
	}
	return 1;
}

// =============================================================================
// INTERNAL FUNCTIONS
// =============================================================================

XpUI_Create(playerid) {
	// Progress Bar
	XpUIData[playerid][xpBar] = CreatePlayerProgressBar(
		playerid,
		XP_BAR_X,
		XP_BAR_Y,
		XP_BAR_WIDTH,
		XP_BAR_HEIGHT,
		XP_BAR_COLOR,
		100.0,
		0
	);

	// Current Level Text
	XpUIData[playerid][levelCurrentText] = CreatePlayerTextDraw(
		playerid,
		XP_TEXT_LEVEL_CURR_X,
		XP_TEXT_LEVEL_CURR_Y,
		"1"
	);
	PlayerTextDrawFont(playerid, XpUIData[playerid][levelCurrentText], 3);
	PlayerTextDrawLetterSize(playerid, XpUIData[playerid][levelCurrentText], 0.295833, 1.900000);
	PlayerTextDrawTextSize(playerid, XpUIData[playerid][levelCurrentText], 400.0, 17.0);
	PlayerTextDrawSetOutline(playerid, XpUIData[playerid][levelCurrentText], 1);
	PlayerTextDrawSetShadow(playerid, XpUIData[playerid][levelCurrentText], 0);
	PlayerTextDrawAlignment(playerid, XpUIData[playerid][levelCurrentText], 2);
	PlayerTextDrawColor(playerid, XpUIData[playerid][levelCurrentText], COLOR_XP_RED);
	PlayerTextDrawBackgroundColor(playerid, XpUIData[playerid][levelCurrentText], 255);
	PlayerTextDrawBoxColor(playerid, XpUIData[playerid][levelCurrentText], 50);
	PlayerTextDrawUseBox(playerid, XpUIData[playerid][levelCurrentText], false);
	PlayerTextDrawSetProportional(playerid, XpUIData[playerid][levelCurrentText], true);
	PlayerTextDrawSetSelectable(playerid, XpUIData[playerid][levelCurrentText], false);

	// Next Level Text
	XpUIData[playerid][levelNextText] = CreatePlayerTextDraw(
		playerid,
		XP_TEXT_LEVEL_NEXT_X,
		XP_TEXT_LEVEL_NEXT_Y,
		"2"
	);
	PlayerTextDrawFont(playerid, XpUIData[playerid][levelNextText], 3);
	PlayerTextDrawLetterSize(playerid, XpUIData[playerid][levelNextText], 0.295833, 1.900000);
	PlayerTextDrawTextSize(playerid, XpUIData[playerid][levelNextText], 400.0, 17.0);
	PlayerTextDrawSetOutline(playerid, XpUIData[playerid][levelNextText], 1);
	PlayerTextDrawSetShadow(playerid, XpUIData[playerid][levelNextText], 0);
	PlayerTextDrawAlignment(playerid, XpUIData[playerid][levelNextText], 2);
	PlayerTextDrawColor(playerid, XpUIData[playerid][levelNextText], COLOR_XP_RED);
	PlayerTextDrawBackgroundColor(playerid, XpUIData[playerid][levelNextText], 255);
	PlayerTextDrawBoxColor(playerid, XpUIData[playerid][levelNextText], 50);
	PlayerTextDrawUseBox(playerid, XpUIData[playerid][levelNextText], false);
	PlayerTextDrawSetProportional(playerid, XpUIData[playerid][levelNextText], true);
	PlayerTextDrawSetSelectable(playerid, XpUIData[playerid][levelNextText], false);

	// Progress Text (XP/Required)
	XpUIData[playerid][progressText] = CreatePlayerTextDraw(
		playerid,
		XP_TEXT_PROGRESS_X,
		XP_TEXT_PROGRESS_Y,
		"0/0"
	);
	PlayerTextDrawFont(playerid, XpUIData[playerid][progressText], 3);
	PlayerTextDrawLetterSize(playerid, XpUIData[playerid][progressText], 0.295833, 1.900000);
	PlayerTextDrawTextSize(playerid, XpUIData[playerid][progressText], 400.0, 17.0);
	PlayerTextDrawSetOutline(playerid, XpUIData[playerid][progressText], 1);
	PlayerTextDrawSetShadow(playerid, XpUIData[playerid][progressText], 0);
	PlayerTextDrawAlignment(playerid, XpUIData[playerid][progressText], 2);
	PlayerTextDrawColor(playerid, XpUIData[playerid][progressText], COLOR_XP_TEXT);
	PlayerTextDrawBackgroundColor(playerid, XpUIData[playerid][progressText], 255);
	PlayerTextDrawBoxColor(playerid, XpUIData[playerid][progressText], 50);
	PlayerTextDrawUseBox(playerid, XpUIData[playerid][progressText], false);
	PlayerTextDrawSetProportional(playerid, XpUIData[playerid][progressText], true);
	PlayerTextDrawSetSelectable(playerid, XpUIData[playerid][progressText], false);

	// Gain Label
	XpUIData[playerid][gainText] = CreatePlayerTextDraw(
		playerid,
		XP_TEXT_GAIN_X,
		XP_TEXT_GAIN_Y,
		"+XP"
	);
	PlayerTextDrawFont(playerid, XpUIData[playerid][gainText], 3);
	PlayerTextDrawLetterSize(playerid, XpUIData[playerid][gainText], 0.712499, 2.499999);
	PlayerTextDrawTextSize(playerid, XpUIData[playerid][gainText], 333.0, 17.0);
	PlayerTextDrawSetOutline(playerid, XpUIData[playerid][gainText], 2);
	PlayerTextDrawSetShadow(playerid, XpUIData[playerid][gainText], 0);
	PlayerTextDrawAlignment(playerid, XpUIData[playerid][gainText], 1);
	PlayerTextDrawColor(playerid, XpUIData[playerid][gainText], COLOR_XP_GAIN);
	PlayerTextDrawBackgroundColor(playerid, XpUIData[playerid][gainText], 255);
	PlayerTextDrawBoxColor(playerid, XpUIData[playerid][gainText], 50);
	PlayerTextDrawUseBox(playerid, XpUIData[playerid][gainText], false);
	PlayerTextDrawSetProportional(playerid, XpUIData[playerid][gainText], true);
	PlayerTextDrawSetSelectable(playerid, XpUIData[playerid][gainText], false);

	// Amount Text
	XpUIData[playerid][amountText] = CreatePlayerTextDraw(
		playerid,
		XP_TEXT_AMOUNT_X,
		XP_TEXT_AMOUNT_Y,
		"0"
	);
	PlayerTextDrawFont(playerid, XpUIData[playerid][amountText], 3);
	PlayerTextDrawLetterSize(playerid, XpUIData[playerid][amountText], 0.712499, 2.499999);
	PlayerTextDrawTextSize(playerid, XpUIData[playerid][amountText], 333.0, 17.0);
	PlayerTextDrawSetOutline(playerid, XpUIData[playerid][amountText], 2);
	PlayerTextDrawSetShadow(playerid, XpUIData[playerid][amountText], 0);
	PlayerTextDrawAlignment(playerid, XpUIData[playerid][amountText], 1);
	PlayerTextDrawColor(playerid, XpUIData[playerid][amountText], COLOR_XP_TEXT);
	PlayerTextDrawBackgroundColor(playerid, XpUIData[playerid][amountText], 255);
	PlayerTextDrawBoxColor(playerid, XpUIData[playerid][amountText], 50);
	PlayerTextDrawUseBox(playerid, XpUIData[playerid][amountText], false);
	PlayerTextDrawSetProportional(playerid, XpUIData[playerid][amountText], true);
	PlayerTextDrawSetSelectable(playerid, XpUIData[playerid][amountText], false);

	XpUICreated[playerid] = true;
}

XpUI_Update(playerid) {
	if(!XpUICreated[playerid]) {
		return;
	}

	new
		currentLevel = GetPlayerLevel(playerid),
		nextLevel = currentLevel + 1,
		currentXp = GetPlayerXp(playerid),
		xpRequired = GetPlayerXpRequired(playerid),
		str[58];

	// Update level text
	format(str, sizeof(str), "%d", currentLevel);
	PlayerTextDrawSetString(playerid, XpUIData[playerid][levelCurrentText], str);

	// Update next level text
	format(str, sizeof(str), "%d", nextLevel);
	PlayerTextDrawSetString(playerid, XpUIData[playerid][levelNextText], str);

	// Update progress text
	format(str, sizeof(str), "%d/%d", currentXp, xpRequired);
	PlayerTextDrawSetString(playerid, XpUIData[playerid][progressText], str);

	// Update progress bar
	SetPlayerProgressBarMaxValue(playerid, XpUIData[playerid][xpBar], float(xpRequired));
	SetPlayerProgressBarValue(playerid, XpUIData[playerid][xpBar], float(currentXp));
}

XpUI_ShowGain(playerid, amount) {
	if(!XpUICreated[playerid]) {
		return;
	}

	new str[32];

	// Update and show progress bar info
	new
		currentLevel = GetPlayerLevel(playerid),
		nextLevel = currentLevel + 1,
		currentXp = GetPlayerXp(playerid),
		xpRequired = GetPlayerXpRequired(playerid);

	format(str, sizeof(str), "%d", currentLevel);
	PlayerTextDrawSetString(playerid, XpUIData[playerid][levelCurrentText], str);
	PlayerTextDrawShow(playerid, XpUIData[playerid][levelCurrentText]);

	format(str, sizeof(str), "%d", nextLevel);
	PlayerTextDrawSetString(playerid, XpUIData[playerid][levelNextText], str);
	PlayerTextDrawShow(playerid, XpUIData[playerid][levelNextText]);

	format(str, sizeof(str), "%d/%d", currentXp, xpRequired);
	PlayerTextDrawSetString(playerid, XpUIData[playerid][progressText], str);
	PlayerTextDrawShow(playerid, XpUIData[playerid][progressText]);

	SetPlayerProgressBarMaxValue(playerid, XpUIData[playerid][xpBar], float(xpRequired));
	SetPlayerProgressBarValue(playerid, XpUIData[playerid][xpBar], float(currentXp));
	ShowPlayerProgressBar(playerid, XpUIData[playerid][xpBar]);

	// Show gain label and amount
	PlayerTextDrawShow(playerid, XpUIData[playerid][gainText]);

	format(str, sizeof(str), "%d", amount);
	PlayerTextDrawSetString(playerid, XpUIData[playerid][amountText], str);
	PlayerTextDrawShow(playerid, XpUIData[playerid][amountText]);

	// Hide after 6 seconds using YSI defer
	defer XpUI_HideGain(playerid);
}

XpUI_Destroy(playerid) {
	if(!XpUICreated[playerid]) {
		return;
	}

	HidePlayerProgressBar(playerid, XpUIData[playerid][xpBar]);
	PlayerTextDrawHide(playerid, XpUIData[playerid][levelCurrentText]);
	PlayerTextDrawHide(playerid, XpUIData[playerid][levelNextText]);
	PlayerTextDrawHide(playerid, XpUIData[playerid][progressText]);
	PlayerTextDrawHide(playerid, XpUIData[playerid][gainText]);
	PlayerTextDrawHide(playerid, XpUIData[playerid][amountText]);

	XpUICreated[playerid] = false;
}

// =============================================================================
// TIMER CALLBACKS
// =============================================================================

timer XpUI_HideGain[6000](playerid) {
	if(!XpUICreated[playerid]) {
		return;
	}

	HidePlayerProgressBar(playerid, XpUIData[playerid][xpBar]);
	PlayerTextDrawHide(playerid, XpUIData[playerid][levelCurrentText]);
	PlayerTextDrawHide(playerid, XpUIData[playerid][levelNextText]);
	PlayerTextDrawHide(playerid, XpUIData[playerid][progressText]);
	PlayerTextDrawHide(playerid, XpUIData[playerid][gainText]);
	PlayerTextDrawHide(playerid, XpUIData[playerid][amountText]);
}
