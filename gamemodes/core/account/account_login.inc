#include <YSI_Coding\y_hooks>

static
    bool:gPlayerLoginStatus[MAX_PLAYERS],
    gPlayerHash[MAX_PLAYERS][62],
    PlayerAttempts[MAX_PLAYERS];

#define MAX_PASSWORD_ATTEMPT (5)

// Initialise
hook OnPlayerConnect(playerid) {
    gPlayerLoginStatus[playerid] = false;
    gPlayerHash[playerid][0] = EOS;
    return 1;
}

// Global
void:Account_PromptLogin(playerid, const hash[], len = sizeof(hash)) {
    // copy to local stack
    // fix with y_inline scopes.
    strcpy(gPlayerHash[playerid], hash, len);

    inline const PromptLoginResponse(response, listitem, string:inputtext[]) {
         #pragma unused listitem

        // User clicked the 'leave' buton or pressed 'ESC' .
        if(!response) {
            Kick(playerid);
            return;
        }

        bcrypt_verify(playerid, "OnPasswordVerify", inputtext, gPlayerHash[playerid]);
    }

    new
        string[256];

    static const LoginMessage[] = "\
        "C_SAMPBLUE"ACCOUNT LOGIN\
        \n\n"C_WHITE"Welcome back, "C_GOLD"%p"C_WHITE"!\
        \n\n"C_GREY"Please enter your password to access your account.\
        \n"C_GREY"Your account security is important to us.\
        \n\n"C_BLUE"Enter your password below to continue.\
    ";

    format(
        string,
        sizeof(string),
        LoginMessage,
        playerid,
        SERVER_NAME
    );

    Dialog_ShowCallback(
        playerid,
        using inline PromptLoginResponse,   // Handler
        DIALOG_STYLE_PASSWORD,              // Style
        SERVER_NAME,                        // Title
        string,                             // Content
        "Login",                            // Button Left
        "Leave"                             // Button Right
    );
}

// Local
static void:Account_NotifySuccess(playerid) {
    static const
        msg_title[] = "~g~Welcome back!",
        msg_content[] = "Hello ~p~%p, ~w~Welcome back to ~p~%s. ~n~~n~~y~Enjoy your stay!";
    MessageBox_ShowF(playerid, TYPE_MSGBOX, msg_title, msg_content, 8000, playerid, SERVER_NAME);
}


hook OnPlayerLogin(playerid) {
    new
        Statement: stmt_updatePlayerLastLogin;

    stmt_updatePlayerLastLogin = MySQL_PrepareStatement(MySQL_GetHandle(), "UPDATE players SET last_login = CURRENT_TIMESTAMP() WHERE u_id = ?");
    MySQL_BindInt(stmt_updatePlayerLastLogin, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteThreaded(stmt_updatePlayerLastLogin);
}

forward OnPasswordVerify(playerid, bool:success);
public OnPasswordVerify(playerid, bool:success) {
    if(!success) {
        PlayerAttempts[playerid] ++;
        if(PlayerAttempts[playerid] >= MAX_PASSWORD_ATTEMPT) {
            SendErrorMsg(playerid, "You have been kicked from the server for attempting to login numerous times.");
            Kick(playerid);

            Logger_Dbg("player", "player has been kicked for too many login attempts",
                _p("username", playerid),
                _i("uid", Player_GetAccountID(playerid))
            );
            return;
        }

        Account_PromptLogin(playerid, gPlayerHash[playerid]);
        SendErrorMsgF(playerid, "Wrong password, try again! - Attempts: [%i/%i]", PlayerAttempts[playerid], MAX_PASSWORD_ATTEMPT);

        Logger_Dbg("player", "player failed to login",
            _i("attemps", PlayerAttempts[playerid]),
            _p("username", playerid),
            _i("uid", Player_GetAccountID(playerid))
        );
        return;
    }

    Logger_Dbg("player", "player successfully logged in",
        _p("username", playerid),
        _i("uid", Player_GetAccountID(playerid))
    );

    PlayerAttempts[playerid] = 0;
    Account_NotifySuccess(playerid);
    CallLocalFunction("OnPlayerLogin", "i", playerid); // Used in other modules to load other data.
}


hook OnPlayeLogin(playerid) {
    gPlayerLoginStatus[playerid] = true;
    return 1;
}


 // API
Player_IsLoggedIn(playerid) {
    return gPlayerLoginStatus[playerid];
}

Player_SetLoginStatus(playerid, bool:status) {
    return gPlayerLoginStatus[playerid] = status;
}
