#include <YSI_Coding\y_hooks>

forward PlayerItems_SaveInventory(playerid);

static
    PlayerC4[MAX_PLAYERS],
    PlayerCrack[MAX_PLAYERS],
    PlayerWeed[MAX_PLAYERS],
    PlayerPickLock[MAX_PLAYERS],
    PlayerWallet[MAX_PLAYERS],
    PlayerCondom[MAX_PLAYERS],
    PlayerRope[MAX_PLAYERS],
    PlayerScissors[MAX_PLAYERS],
    PlayerOranges[MAX_PLAYERS],
    PlayerBottles[MAX_PLAYERS],
    PlayerOrangeJuice[MAX_PLAYERS],
    PlayerFruitPicker[MAX_PLAYERS];

hook OnPlayerConnect(playerid) {
    PlayerC4[playerid]          =
    PlayerCrack[playerid]       =
    PlayerWeed[playerid]        =
    PlayerPickLock[playerid]    =
    PlayerWallet[playerid]      =
    PlayerCondom[playerid]      =
    PlayerRope[playerid]        =
    PlayerScissors[playerid]    =
    PlayerOranges[playerid]     =
    PlayerBottles[playerid]     =
    PlayerOrangeJuice[playerid] =
    PlayerFruitPicker[playerid] = 0;
    return 1;
}

hook OnPlayerRegister(playerid) {
    new
        Statement: stmt_insert;

    stmt_insert = MySQL_PrepareStatement(MySQL_GetHandle(), "INSERT INTO player_items (u_id) VALUES (?)");

    MySQL_BindInt(stmt_insert, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteThreaded(stmt_insert);
    MySQL_StatementClose(stmt_insert);
    return 1;
}

hook OnPlayerLogin(playerid) {
    new
        Statement: stmt_load;

    inline const OnItemLoad() {
        MySQL_BindResultInt(stmt_load, 0, PlayerC4[playerid]);
        MySQL_BindResultInt(stmt_load, 1, PlayerCrack[playerid]);
        MySQL_BindResultInt(stmt_load, 2, PlayerWeed[playerid]);
        MySQL_BindResultInt(stmt_load, 3, PlayerPickLock[playerid]);
        MySQL_BindResultInt(stmt_load, 4, PlayerWallet[playerid]);
        MySQL_BindResultInt(stmt_load, 5, PlayerCondom[playerid]);
        MySQL_BindResultInt(stmt_load, 6, PlayerRope[playerid]);
        MySQL_BindResultInt(stmt_load, 7, PlayerScissors[playerid]);
        // farming player inventory
        MySQL_BindResultInt(stmt_load, 8, PlayerOranges[playerid]);
        MySQL_BindResultInt(stmt_load, 9, PlayerBottles[playerid]);
        MySQL_BindResultInt(stmt_load, 10, PlayerOrangeJuice[playerid]);
        MySQL_BindResultInt(stmt_load, 11, PlayerFruitPicker[playerid]);


        if(!MySQL_Statement_FetchRow(stmt_load)) {
            Logger_Dbg(
                "player_items",
                "[OnPlayerLogin / player_items.inc] Oops player_items did not load properly.",
                Logger_P(playerid),
                Logger_I("uid", Player_GetAccountID(playerid))
            );
        }
    }

    static const query[] = "\
    SELECT \
        c4, \
        crack, \
        weed, \
        picklock, \
        wallet, \
        rope, \
        condom, \
        scissors, \
        oranges, \
        bottles, \
        orange_juice, \
        fruit_picker \
    FROM \
        player_items \
    WHERE \
        u_id = ? \
    ";

    stmt_load = MySQL_PrepareStatement(MySQL_GetHandle(), query);
    MySQL_BindInt(stmt_load, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteThreaded_Inline(stmt_load, using inline OnItemLoad);
    MySQL_StatementClose(stmt_load);
    return 1;
}

// C4
stock void:Player_GiveC4(playerid, amount) {
    PlayerC4[playerid] += amount;

    _SaveItem(playerid, "c4", amount);
}

stock Player_GetC4(playerid) {
    return PlayerC4[playerid];
}

// Crack
stock void:Player_GiveCrack(playerid, amount) {
    PlayerCrack[playerid] += amount;

    _SaveItem(playerid, "crack", amount);
}

stock Player_GetCrack(playerid) {
    return PlayerCrack[playerid];
}

// Picklock
stock void:Player_GivePicklock(playerid, amount) {
    PlayerPickLock[playerid] += amount;

    _SaveItem(playerid, "picklock", amount);
}

stock Player_GetPicklock(playerid) {
    return PlayerPickLock[playerid];
}

// Wallet
stock void:Player_GiveWallet(playerid, amount) {
    PlayerWallet[playerid] += amount;

    _SaveItem(playerid, "wallet", amount);
}

stock Player_GetWallet(playerid) {
    return PlayerWallet[playerid];
}

// Weed
stock void:Player_GiveWeed(playerid, amount) {
    PlayerWeed[playerid] += amount;

    _SaveItem(playerid, "weed", amount);
}

stock Player_GetWeed(playerid) {
    return PlayerWeed[playerid];
}

// Condom
stock void:Player_GiveCondom(playerid, amount) {
    PlayerCondom[playerid] += amount;

    _SaveItem(playerid, "condom", amount);
}

stock Player_GetCondom(playerid) {
    return PlayerCondom[playerid];
}

// Rope
stock void:Player_GiveRope(playerid, amount) {
    PlayerRope[playerid] += amount;

    _SaveItem(playerid, "rope", amount);
}

stock Player_GetRope(playerid) {
    return PlayerRope[playerid];
}

// Scissors
stock void:Player_GiveScissors(playerid, amount) {
    PlayerScissors[playerid] += amount;

    _SaveItem(playerid, "scissors", amount);
}

stock Player_GetScissors(playerid) {
    return PlayerScissors[playerid];
}

// farming inv
// Oranges
stock void:Player_GiveOranges(playerid, amount) {
    PlayerOranges[playerid] += amount;
    PlayerItems_SaveInventory(playerid);
}

stock void:Player_RemoveOranges(playerid, amount) {
    PlayerOranges[playerid] -= amount;
    if(PlayerOranges[playerid] < 0) PlayerOranges[playerid] = 0;
    PlayerItems_SaveInventory(playerid);
}

stock Player_GetOranges(playerid) {
    return PlayerOranges[playerid];
}

// Bottles
stock void:Player_GiveBottles(playerid, amount) {
    PlayerBottles[playerid] += amount;
    PlayerItems_SaveInventory(playerid);
}

stock void:Player_RemoveBottles(playerid, amount) {
    PlayerBottles[playerid] -= amount;
    if(PlayerBottles[playerid] < 0) PlayerBottles[playerid] = 0;
    PlayerItems_SaveInventory(playerid);
}

stock Player_GetBottles(playerid) {
    return PlayerBottles[playerid];
}

// Orange Juice
stock void:Player_GiveOrangeJuice(playerid, amount) {
    PlayerOrangeJuice[playerid] += amount;
    PlayerItems_SaveInventory(playerid);
}

stock void:Player_RemoveOrangeJuice(playerid, amount) {
    PlayerOrangeJuice[playerid] -= amount;
    if(PlayerOrangeJuice[playerid] < 0) PlayerOrangeJuice[playerid] = 0;
    PlayerItems_SaveInventory(playerid);
}

stock Player_GetOrangeJuice(playerid) {
    return PlayerOrangeJuice[playerid];
}

// Fruit Picker
stock void:Player_GiveFruitPicker(playerid, amount) {
    PlayerFruitPicker[playerid] += amount;
    PlayerItems_SaveInventory(playerid);
}

stock void:Player_RemoveFruitPicker(playerid, amount) {
    PlayerFruitPicker[playerid] -= amount;
    if(PlayerFruitPicker[playerid] < 0) PlayerFruitPicker[playerid] = 0;
    PlayerItems_SaveInventory(playerid);
}

stock Player_GetFruitPicker(playerid) {
    return PlayerFruitPicker[playerid];
}



// Internal Function - Save single item
static void:_SaveItem(playerid, const itemName[], value) {
    new
        Statement: stmt_saveItem;

    stmt_saveItem = MySQL_PrepareStatement(
        MySQL_GetHandle(),
        "UPDATE player_items SET ? = ? + ? WHERE u_id = ?"
    );

    MySQL_Bind(stmt_saveItem, 0, itemName, true);
    MySQL_Bind(stmt_saveItem, 1, itemName, true);
    MySQL_BindInt(stmt_saveItem, 2, value);
    MySQL_BindInt(stmt_saveItem, 3, Player_GetAccountID(playerid));
    MySQL_ExecuteThreaded(stmt_saveItem);
}

// Save complete inventory to database
public PlayerItems_SaveInventory(playerid) {
    new
        Statement: stmt_save,
        accountID = Player_GetAccountID(playerid);

    if(accountID <= 0) return 0;

    stmt_save = MySQL_PrepareStatement(MySQL_GetHandle(),
        "UPDATE player_items SET c4=?, crack=?, weed=?, picklock=?, wallet=?, condom=?, rope=?, scissors=?, oranges=?, bottles=?, orange_juice=?, fruit_picker=? WHERE u_id=?");

    MySQL_BindInt(stmt_save, 0, PlayerC4[playerid]);
    MySQL_BindInt(stmt_save, 1, PlayerCrack[playerid]);
    MySQL_BindInt(stmt_save, 2, PlayerWeed[playerid]);
    MySQL_BindInt(stmt_save, 3, PlayerPickLock[playerid]);
    MySQL_BindInt(stmt_save, 4, PlayerWallet[playerid]);
    MySQL_BindInt(stmt_save, 5, PlayerCondom[playerid]);
    MySQL_BindInt(stmt_save, 6, PlayerRope[playerid]);
    MySQL_BindInt(stmt_save, 7, PlayerScissors[playerid]);
    MySQL_BindInt(stmt_save, 8, PlayerOranges[playerid]);
    MySQL_BindInt(stmt_save, 9, PlayerBottles[playerid]);
    MySQL_BindInt(stmt_save, 10, PlayerOrangeJuice[playerid]);
    MySQL_BindInt(stmt_save, 11, PlayerFruitPicker[playerid]);
    MySQL_BindInt(stmt_save, 12, accountID);

    MySQL_ExecuteThreaded(stmt_save);
    MySQL_StatementClose(stmt_save);

    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    PlayerItems_SaveInventory(playerid);
    return 1;
}

static _ItemPlayerHoldingValue(playerid, itemId) {
    switch(itemId) {
        case 0: return Player_GetC4(playerid);
        case 1: return Player_GetCrack(playerid);
        case 2: return Player_GetWeed(playerid);
        case 3: return Player_GetPicklock(playerid);
        case 4: return Player_GetWallet(playerid);
        case 5: return Player_GetCondom(playerid);
        case 6: return Player_GetRope(playerid);
        case 7: return Player_GetScissors(playerid);
        case 8: return Player_GetOranges(playerid);
        case 9: return Player_GetBottles(playerid);
        case 10: return Player_GetOrangeJuice(playerid);
        case 11: return Player_GetFruitPicker(playerid);
    }
    return -1;
}

static void:_GiveItem(playerid, itemId, amount) {
    switch(itemId) {
        case 0: Player_GiveC4(playerid, amount);
        case 1: Player_GiveCrack(playerid, amount);
        case 2: Player_GiveWeed(playerid, amount);
        case 3: Player_GivePicklock(playerid, amount);
        case 4: Player_GiveWallet(playerid, amount);
        case 5: Player_GiveCondom(playerid, amount);
        case 6: Player_GiveRope(playerid, amount);
        case 7: Player_GiveScissors(playerid, amount);
        case 8: Player_GiveOranges(playerid, amount);
        case 9: Player_GiveBottles(playerid, amount);
        case 10: Player_GiveOrangeJuice(playerid, amount);
        case 11: Player_GiveFruitPicker(playerid, amount);
    }
}

// Command
ACMD:[4]giveitem(playerid, params[]) {
    new
        targetid,
        type[13],
        amount;

    if(sscanf(params, "us[13]i", targetid, type, amount)) {
        return SendSyntaxMsg(playerid, "/giveitem [playerid] [type] [amount]");
    }

    static const validItems[][] = {
        "c4",
        "crack",
        "weed",
        "picklock",
        "wallet",
        "condom",
        "rope",
        "scissors",
        "oranges",
        "bottles",
        "orangejuice",
        "fruitpicker"
    };

    static const itemLimit[] = {
        MAX_ITEM,
        MAX_CRACK,
        MAX_WEED,
        MAX_ITEM,
        MAX_ITEM,
        MAX_ITEM,
        MAX_ITEM,
        MAX_ITEM,
        MAX_ORANGES,
        MAX_BOTTLES,
        MAX_ORANGEJUICE,
        MAX_FRUITPICKER
    };

    new
        bool:isValid = false,
        itemId = -1;

    for(new i = 0; i < sizeof(validItems); i ++) {
        if(!strcmp(type, validItems[i])) {
            isValid = true;
            itemId = i;
            break;
        }
    }

    if(!isValid) {
        return SendErrorMsg(
                playerid,
                "invalid type \
                ~n~valid types: \
                ~n~[~y~c4, crack, weed, picklock, wallet, condom, rope, scissors, orange, bottles, orangejuice, fruitpicker~w~]"
            );
    }

    if(amount > 10 || amount < 1) {
        return SendErrorMsg(playerid, "invalid range 1 - 10");
    }

    if(amount > (itemLimit[itemId] - _ItemPlayerHoldingValue(playerid, itemId))) {
        return SendErrorMsgF(
                playerid,
                "value too high\
                ~n~tip: player can hold ~y~%i ~p~%s",
                (itemLimit[itemId] - _ItemPlayerHoldingValue(playerid, itemId)),
                validItems[itemId]
            );
    }

    _GiveItem(targetid, itemId, amount);

    SendServerMsgF(playerid, "You have given %p[%i] x"C_YELLOW"%i "C_GREY"%s", targetid, targetid, amount, validItems[itemId]);
    SendServerMsgF(playerid, "%p[%i] have given you x"C_YELLOW"%i "C_GREY"%s", playerid, playerid, amount, validItems[itemId]);
    return 1;
}
