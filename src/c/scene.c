// License:MIT License
// copyright-holders:aburi6800 (Hitoshi Iwai)

#include "../include/resource.h"

// Number of scenes
#define SCENE_NUM 23

// Flag definitions
#define FLAG_FOUND_KEY      0b0000000000000001    // Found key
#define FLAG_HAVE_KEY       0b0000000000000010    // Obtained key
#define FLAG_OPEN_DOOR      0b0000000000000100    // Opened door
#define FLAG_FAIL_APPLE     0b0000000000001000    // Apple fell
#define FLAG_HAVE_APPLE     0b0000000000010000    // Obtained apple
#define FLAG_LOOK_SNAKE     0b0000000000100000    // Saw snake
#define FLAG_KILL_SNAKE     0b0000000001000000    // Defeated snake
#define FLAG_HAVE_SWORD     0b0000000010000000    // Obtained sword
#define FLAG_PULL_SWORD     0b0000000100000000    // Attempted to draw sword
#define FLAG_BROKEN_SWORD   0b0000001000000000    // Broke sword
#define FLAG_LOOK_SKELETON  0b0000010000000000    // Saw skeleton
#define FLAG_KILL_SKELETON  0b0000100000000000    // Defeated skeleton
#define FLAG_HAVE_SKARF     0b0001000000000000    // Obtained scarf
#define FLAG_EQUIP_SWORD    0b0010000000000000    // Equipped sword
#define FLAG_POLISH_PILLER  0b0100000000000000    // Polished pillar


// Scene ID enumeration
typedef enum {
    NOSCENE,
    TITLE,
    PROLOGUE,
    DESCRIPTION,
    SCENE01,
    SCENE02,
    SCENE03,
    SCENE03B,
    SCENE03_1,
    SCENE03_2,
    SCENE04,
    SCENE04_1,
    SCENE04_2,
    SCENE05,
    SCENE05B,
    SCENE05_1,
    SCENE05_2,
    SCENE05_3,
    SCENE06,
    SCENE06_1,
    SCENE06_2,
    SCENE07,
    OVER,
    ENDING
} SceneId;


// Choice definition
typedef struct {
    uint16_t required_flag;         // Condition for this choice to be displayed/executed (0 = no condition)
    const char *commands[5];        // Player input command (up to 5 commands per choice)
    uint16_t flag_to_check;         // Flag used for message branching (0 = no branching)
    uint8_t *message_if_unset;      // Message to display when flag is not set
    uint16_t set_flag_if_unset;     // Flag to set after execution (when flag is unset)
    SceneId next_sceneId_if_unset;  // Next scene (empty = no transition)
    uint8_t *message_if_set;        // Message to display when flag is set
    uint16_t set_flag_if_set;       // Flag to set after execution (when flag is set)
    SceneId next_sceneId_if_set;    // Next scene (empty = no transition)
} Choice;


// Scene definition
typedef struct {
    SceneId sceneId;                // Scene ID
    uint16_t flag_to_check;         // Flag used for scene branching (0 = no branching)
    SceneId next_sceneId_if_unset;  // Scene ID when flag is not set
    SceneId next_sceneId_if_set;    // Scene ID when flag is set
    uint8_t graphic_bank;           // Graphics data bank
    uint8_t *graphic_ptn0;          // Graphics data (pattern generator table PAGE0)
    uint8_t *graphic_ptn1;          // Graphics data (pattern generator table PAGE1)
    uint8_t *graphic_col0;          // Graphics data (color table PAGE0)
    uint8_t *graphic_col1;          // Graphics data (color table PAGE1)
    uint8_t *message;               // Message to display at the start of the scene
    uint8_t *choices;
} Scene;


// Scene data
Choice choicesNull[1] = {
    {   
        .required_flag          = 0,
        .commands               = {NULL}
    }
};


Choice choicesTitle[2] = {
    {   
        .required_flag          = 0,
        .commands               = {"START", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = PROLOGUE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {   
        .required_flag          = 0,
        .commands               = {NULL}
    }
};

Scene sceneTitle = {
    .sceneId                = TITLE,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NOSCENE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 1,
    .graphic_ptn0           = TITLE_PTN_BLK0,
    .graphic_ptn1           = TITLE_PTN_BLK1,
    .graphic_col0           = TITLE_COL_BLK0,
    .graphic_col1           = TITLE_COL_BLK1,
    .message                = MESSAGE00000,
    .choices                = choicesTitle
};


Choice choicesPrologue[3] = {
    {   
        .required_flag          = 0,
        .commands               = {"Y", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = DESCRIPTION,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {   
        .required_flag          = 0,
        .commands               = {"N", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE01,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {   
        .required_flag          = 0,
        .commands               = {NULL}
    }
};

Scene scenePrologue = {
    .sceneId                = PROLOGUE,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NOSCENE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 1,
    .graphic_ptn0           = PLLG_PTN_BLK0,
    .graphic_ptn1           = PLLG_PTN_BLK1,
    .graphic_col0           = PLLG_COL_BLK0,
    .graphic_col1           = PLLG_COL_BLK1,
    .message                = MESSAGE97000,
    .choices                = choicesPrologue
};


Scene sceneDescription = {
    .sceneId                = DESCRIPTION,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = SCENE01,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 1,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = HELPMESSAGE,
    .choices                = choicesNull
};


Scene sceneOver = {
    .sceneId                = OVER,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = TITLE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 1,
    .graphic_ptn0           = OVER_PTN_BLK0,
    .graphic_ptn1           = OVER_PTN_BLK1,
    .graphic_col0           = OVER_COL_BLK0,
    .graphic_col1           = OVER_COL_BLK1,
    .message                = MESSAGE99000,
    .choices                = choicesNull
};


Scene sceneEnding = {
    .sceneId                = ENDING,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = TITLE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 1,
    .graphic_ptn0           = ENDING_PTN_BLK0,
    .graphic_ptn1           = ENDING_PTN_BLK1,
    .graphic_col0           = ENDING_COL_BLK0,
    .graphic_col1           = ENDING_COL_BLK1,
    .message                = MESSAGE98000,
    .choices                = choicesNull
};


Choice choices010[8] = {
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROOM", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE01001,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK TABLE", NULL},
        .flag_to_check          = FLAG_HAVE_KEY,
        .message_if_unset       = MESSAGE01002,
        .set_flag_if_unset      = FLAG_FOUND_KEY,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE01003,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = FLAG_FOUND_KEY,
        .commands               = {"LOOK KEY", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE01011,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = FLAG_FOUND_KEY,
        .commands               = {"GET KEY", "TAKE KEY", NULL},
        .flag_to_check          = FLAG_HAVE_KEY,
        .message_if_unset       = MESSAGE01004,
        .set_flag_if_unset      = FLAG_HAVE_KEY,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE01005,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK DOOR", NULL},
        .flag_to_check          = FLAG_OPEN_DOOR,
        .message_if_unset       = MESSAGE01006,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE01007,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = 0,
        .commands               = {"OPEN DOOR", "USE KEY", NULL},
        .flag_to_check          = FLAG_HAVE_KEY,
        .message_if_unset       = MESSAGE01008,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE01009,
        .set_flag_if_set        = FLAG_OPEN_DOOR,
        .next_sceneId_if_set    = NOSCENE
    },
    {
        .required_flag          = FLAG_OPEN_DOOR,
        .commands               = {"ENTER DOOR", "GO FORWARD", "FORWARD", "F", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE01010,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands               = {NULL}
    }
};

Scene scene010 = {
    .sceneId                = SCENE01,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NOSCENE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 2,
    .graphic_ptn0           = SC010_PTN_BLK0,
    .graphic_ptn1           = SC010_PTN_BLK1,
    .graphic_col0           = SC010_COL_BLK0,
    .graphic_col1           = SC010_COL_BLK1,
    .message                = MESSAGE01000,
    .choices                = choices010
};


Choice choices020[6] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE01,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO LEFT", "LEFT", "L", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE06,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO RIGHT", "RIGHT", "R", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE03,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO FORWARD", "FORWARD", "F", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE04,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROOM", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE02001,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands               = {NULL}
    }
};

Scene scene020 = {
    .sceneId                = SCENE02,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NOSCENE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 2,
    .graphic_ptn0           = SC020_PTN_BLK0,
    .graphic_ptn1           = SC020_PTN_BLK1,
    .graphic_col0           = SC020_COL_BLK0,
    .graphic_col1           = SC020_COL_BLK1,
    .message                = MESSAGE02000,
    .choices                = choices020
};


Scene scene030 = {
    .sceneId                = SCENE03,
    .flag_to_check          = FLAG_FAIL_APPLE,
    .next_sceneId_if_unset  = SCENE03_1,
    .next_sceneId_if_set    = SCENE03B,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Scene scene03B = {
    .sceneId                = SCENE03B,
    .flag_to_check          = FLAG_HAVE_APPLE,
    .next_sceneId_if_unset  = SCENE03_2,
    .next_sceneId_if_set    = SCENE03_1,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Choice choices031[7] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK TREE", "LOOK ROOM", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE03101,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"CLIMB TREE", "GO UP", "UP", "U", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE03102,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"KICK TREE", "SWING TREE", "PUSH TREE", NULL},
        .flag_to_check          = FLAG_HAVE_APPLE,
        .message_if_unset       = MESSAGE03103,
        .set_flag_if_unset      = FLAG_FAIL_APPLE,
        .next_sceneId_if_unset  = SCENE03_2,
        .message_if_set         = MESSAGE03104,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = FLAG_HAVE_APPLE,
        .commands               = {"EAT APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE03105,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = FLAG_HAVE_APPLE,
        .commands               = {"LOOK APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE03202,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene031 = {
    .sceneId                = SCENE03_1,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 2,
    .graphic_ptn0           = SC031_PTN_BLK0,
    .graphic_ptn1           = SC031_PTN_BLK1,
    .graphic_col0           = SC031_COL_BLK0,
    .graphic_col1           = SC031_COL_BLK1,
    .message                = MESSAGE03100,
    .choices                = choices031
};


Choice choices032[8] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NULL,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK TREE", "LOOK ROOM", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE03201,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"CLIMB TREE", "GO UP", "UP", "U", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE03202,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"KICK TREE", "SWING TREE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE03104,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE03202,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GET APPLE", "TAKE APPLE", NULL},
        .flag_to_check          = FLAG_HAVE_APPLE,
        .message_if_unset       = MESSAGE03203,
        .set_flag_if_unset      = FLAG_HAVE_APPLE,
        .next_sceneId_if_unset  = SCENE03_1,
        .message_if_set         = MESSAGE03204,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"EAT APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE03205,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands               = {NULL}
    }
};

Scene scene032 = {
    .sceneId                = SCENE03_2,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NOSCENE,
    .next_sceneId_if_set    = NOSCENE,
    .graphic_bank           = 2,
    .graphic_ptn0           = SC032_PTN_BLK0,
    .graphic_ptn1           = SC032_PTN_BLK1,
    .graphic_col0           = SC032_COL_BLK0,
    .graphic_col1           = SC032_COL_BLK1,
    .message                = MESSAGE03200,
    .choices                = choices032
};


Scene scene040 = {
    .sceneId                = SCENE04,
    .flag_to_check          = FLAG_KILL_SNAKE,
    .next_sceneId_if_unset  = SCENE04_1,
    .next_sceneId_if_set    = SCENE04_2,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Choice choices041[6] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO FORWARD", "FORWARD", "F", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE04101,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SNAKE", NULL},
        .flag_to_check          = FLAG_LOOK_SNAKE,
        .message_if_unset       = MESSAGE04102,
        .set_flag_if_unset      = FLAG_LOOK_SNAKE,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE04101,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = OVER,
    },
    {
        .required_flag          = FLAG_HAVE_APPLE,
        .commands               = {"THROW APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE04103,
        .set_flag_if_unset      = FLAG_KILL_SNAKE,
        .next_sceneId_if_unset  = SCENE04,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = SCENE04,
    },
    {
        .required_flag          = 0,
        .commands               = {"EAT APPLE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE04104,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene041 = {
    .sceneId                = SCENE04_1,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC041_PTN_BLK0,
    .graphic_ptn1           = SC041_PTN_BLK1,
    .graphic_col0           = SC041_COL_BLK0,
    .graphic_col1           = SC041_COL_BLK1,
    .message                = MESSAGE04100,
    .choices                = choices041
};


Choice choices042[5] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO FORWARD", "FORWARD", "F", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE05,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SNAKE", "LOOK BONE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE04201,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GET BONE", "TAKE BONE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE04202,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = SCENE04,
    },
    {
        .commands = {NULL}
    }
};

Scene scene042 = {
    .sceneId                = SCENE04_2,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC042_PTN_BLK0,
    .graphic_ptn1           = SC042_PTN_BLK1,
    .graphic_col0           = SC042_COL_BLK0,
    .graphic_col1           = SC042_COL_BLK1,
    .message                = MESSAGE04200,
    .choices                = choices042
};


Scene scene050 = {
    .sceneId                = SCENE05,
    .flag_to_check          = FLAG_HAVE_SWORD,
    .next_sceneId_if_unset  = SCENE05B,
    .next_sceneId_if_set    = SCENE05_2,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Scene scene05B = {
    .sceneId                = SCENE05B,
    .flag_to_check          = FLAG_BROKEN_SWORD,
    .next_sceneId_if_unset  = SCENE05_1,
    .next_sceneId_if_set    = SCENE05_3,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Choice choices051[7] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE04,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE05101,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SWORD", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE05102,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"PULL SWORD", NULL},
        .flag_to_check          = FLAG_PULL_SWORD,
        .message_if_unset       = MESSAGE05103,
        .set_flag_if_unset      = FLAG_PULL_SWORD,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE05104,
        .set_flag_if_set        = FLAG_BROKEN_SWORD,
        .next_sceneId_if_set    = SCENE05_3,
    },
    {
        .required_flag          = 0,
        .commands               = {"PUSH SWORD", NULL},
        .flag_to_check          = FLAG_PULL_SWORD,
        .message_if_unset       = MESSAGE05105,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE05106,
        .set_flag_if_set        = FLAG_HAVE_SWORD,
        .next_sceneId_if_set    = SCENE05_2,
    },
    {
        .required_flag          = 0,
        .commands               = {"MOVE ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE05107,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene051 = {
    .sceneId                = SCENE05_1,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC051_PTN_BLK0,
    .graphic_ptn1           = SC051_PTN_BLK1,
    .graphic_col0           = SC051_COL_BLK0,
    .graphic_col1           = SC051_COL_BLK1,
    .message                = MESSAGE05100,
    .choices                = choices051
};


Choice choices052[4] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE04,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE05201,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"MOVE ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE05107,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene052 = {
    .sceneId                = SCENE05_2,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC052_PTN_BLK0,
    .graphic_ptn1           = SC052_PTN_BLK1,
    .graphic_col0           = SC052_COL_BLK0,
    .graphic_col1           = SC052_COL_BLK1,
    .message                = MESSAGE05200,
    .choices                = choices052
};


Choice choices053[4] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE04,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE05301,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"MOVE ROCK", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE05107,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene053 = {
    .sceneId                = SCENE05_3,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 3,
    .graphic_ptn0           = SC053_PTN_BLK0,
    .graphic_ptn1           = SC053_PTN_BLK1,
    .graphic_col0           = SC053_COL_BLK0,
    .graphic_col1           = SC053_COL_BLK1,
    .message                = MESSAGE05300,
    .choices                = choices053
};


Scene scene060 = {
    .sceneId                = SCENE06,
    .flag_to_check          = FLAG_KILL_SKELETON,
    .next_sceneId_if_unset  = SCENE06_1,
    .next_sceneId_if_set    = SCENE06_2,
    .graphic_bank           = 0,
    .graphic_ptn0           = NULL,
    .graphic_ptn1           = NULL,
    .graphic_col0           = NULL,
    .graphic_col1           = NULL,
    .message                = NULL,
    .choices                = choicesNull
};


Choice choices061[8] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SKELETON", NULL},
        .flag_to_check          = FLAG_LOOK_SKELETON,
        .message_if_unset       = MESSAGE06101,
        .set_flag_if_unset      = FLAG_LOOK_SKELETON,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE06102,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = OVER,
    },
    {
        .required_flag          = FLAG_HAVE_SWORD,
        .commands               = {"USE SWORD", "EQUIP SWORD", NULL},
        .flag_to_check          = FLAG_EQUIP_SWORD,
        .message_if_unset       = MESSAGE06103,
        .set_flag_if_unset      = FLAG_EQUIP_SWORD,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE06104,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = FLAG_EQUIP_SWORD,
        .commands               = {"ATTACK SKELETON", "BATTLE SKELETON", "KILL SKELETON", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE06105,
        .set_flag_if_unset      = FLAG_KILL_SKELETON,
        .next_sceneId_if_unset  = SCENE06_2,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"ATTACK SKELETON", "BATTLE SKELETON", "KILL SKELETON", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE06106,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = OVER,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SCARF", NULL},
        .flag_to_check          = FLAG_LOOK_SKELETON,
        .message_if_unset       = MESSAGE06107,
        .set_flag_if_unset      = FLAG_LOOK_SKELETON,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE06102,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = OVER,
    },
    {
        .required_flag          = 0,
        .commands               = {"GET SCARF", "TAKE SCARF", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE06108,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene061 = {
    .sceneId                = SCENE06_1,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 4,
    .graphic_ptn0           = SC061_PTN_BLK0,
    .graphic_ptn1           = SC061_PTN_BLK1,
    .graphic_col0           = SC061_COL_BLK0,
    .graphic_col1           = SC061_COL_BLK1,
    .message                = MESSAGE06100,
    .choices                = choices061
};


Choice choices062[8] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE02,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GO FORWARD", "FORWARD", "F", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE07,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SKELETON", "LOOK BONE", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE06201,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SWORD", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE06202,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GET SWORD", "TAKE SWORD", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE06203,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK SCARF", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE06204,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"GET SCARF", "TAKE SCARF", NULL},
        .flag_to_check          = FLAG_HAVE_SKARF,
        .message_if_unset       = MESSAGE06205,
        .set_flag_if_unset      = FLAG_HAVE_SKARF,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE06206,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene062 = {
    .sceneId                = SCENE06_2,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 4,
    .graphic_ptn0           = SC062_PTN_BLK0,
    .graphic_ptn1           = SC062_PTN_BLK1,
    .graphic_col0           = SC062_COL_BLK0,
    .graphic_col1           = SC062_COL_BLK1,
    .message                = MESSAGE06200,
    .choices                = choices062
};


Choice choices070[6] = {
    {
        .required_flag          = 0,
        .commands               = {"GO BACK", "BACK", "B", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = NULL,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = SCENE06,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK PILLAR", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE07001,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = FLAG_HAVE_SKARF,
        .commands               = {"POLISH PILLAR", "WIPE PILLAR", NULL},
        .flag_to_check          = FLAG_POLISH_PILLER,
        .message_if_unset       = MESSAGE07002,
        .set_flag_if_unset      = FLAG_POLISH_PILLER,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = MESSAGE07003,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = ENDING,
    },
    {
        .required_flag          = 0,
        .commands               = {"POLISH PILLAR", "WIPE PILLAR", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE07004,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .required_flag          = 0,
        .commands               = {"LOOK ROOM", NULL},
        .flag_to_check          = 0,
        .message_if_unset       = MESSAGE07005,
        .set_flag_if_unset      = 0,
        .next_sceneId_if_unset  = NOSCENE,
        .message_if_set         = NULL,
        .set_flag_if_set        = 0,
        .next_sceneId_if_set    = NOSCENE,
    },
    {
        .commands = {NULL}
    }
};

Scene scene070 = {
    .sceneId                = SCENE07,
    .flag_to_check          = 0,
    .next_sceneId_if_unset  = NULL,
    .next_sceneId_if_set    = NULL,
    .graphic_bank           = 4,
    .graphic_ptn0           = SC070_PTN_BLK0,
    .graphic_ptn1           = SC070_PTN_BLK1,
    .graphic_col0           = SC070_COL_BLK0,
    .graphic_col1           = SC070_COL_BLK1,
    .message                = MESSAGE07000,
    .choices                = choices070
};


// Array of references to scene data
Scene *scenes[SCENE_NUM] = {
    sceneTitle,
    sceneOver,
    sceneEnding,
    scenePrologue,
    sceneDescription,
    scene010,
    scene020,
    scene030,
    scene03B,
    scene031,
    scene032,
    scene040,
    scene041,
    scene042,
    scene050,
    scene05B,
    scene051,
    scene052,
    scene053,
    scene060,
    scene061,
    scene062,
    scene070
};
