-- define module table
local EmpyInput = {}

-- check mouse input
function EmpyInput.IsMouse(button)
    return ApiMouseDown(button)  
end

-- check key input
function EmpyInput.IsKey(key)
    return ApiKeyDown(key)
end

-- mouse input codes  
EmpyInput.MOUSE_LEFT     = 0
EmpyInput.MOUSE_MIDDLE   = 1
EmpyInput.MOUSE_RIGHT    = 2
EmpyInput.MOUSE_BUTTON_4 = 3
EmpyInput.MOUSE_BUTTON_5 = 4
EmpyInput.MOUSE_BUTTON_6 = 5
EmpyInput.MOUSE_BUTTON_7 = 6
EmpyInput.MOUSE_BUTTON_8 = 7

-- key input codes
EmpyInput.KEY_SPACE             = 32
EmpyInput.KEY_APOSTROPHE        = 39 
EmpyInput.KEY_COMMA             = 44 
EmpyInput.KEY_MINUS             = 45  
EmpyInput.KEY_PERIOD            = 46  
EmpyInput.KEY_SLASH             = 47  
EmpyInput.KEY_0                 = 48
EmpyInput.KEY_1                 = 49
EmpyInput.KEY_2                 = 50
EmpyInput.KEY_3                 = 51
EmpyInput.KEY_4                 = 52
EmpyInput.KEY_5                 = 53
EmpyInput.KEY_6                 = 54
EmpyInput.KEY_7                 = 55
EmpyInput.KEY_8                 = 56
EmpyInput.KEY_9                 = 57
EmpyInput.KEY_SEMICOLON         = 59  
EmpyInput.KEY_EQUAL             = 61  
EmpyInput.KEY_A                 = 65
EmpyInput.KEY_B                 = 66
EmpyInput.KEY_C                 = 67
EmpyInput.KEY_D                 = 68
EmpyInput.KEY_E                 = 69
EmpyInput.KEY_F                 = 70
EmpyInput.KEY_G                 = 71
EmpyInput.KEY_H                 = 72
EmpyInput.KEY_I                 = 73
EmpyInput.KEY_J                 = 74
EmpyInput.KEY_K                 = 75
EmpyInput.KEY_L                 = 76
EmpyInput.KEY_M                 = 77
EmpyInput.KEY_N                 = 78
EmpyInput.KEY_O                 = 79
EmpyInput.KEY_P                 = 80
EmpyInput.KEY_Q                 = 81
EmpyInput.KEY_R                 = 82
EmpyInput.KEY_S                 = 83
EmpyInput.KEY_T                 = 84
EmpyInput.KEY_U                 = 85
EmpyInput.KEY_V                 = 86
EmpyInput.KEY_W                 = 87
EmpyInput.KEY_X                 = 88
EmpyInput.KEY_Y                 = 89
EmpyInput.KEY_Z                 = 90
EmpyInput.KEY_LEFT_BRACKET      = 91   
EmpyInput.KEY_BACKSLASH         = 92   
EmpyInput.KEY_RIGHT_BRACKET     = 93   
EmpyInput.KEY_GRAVE_ACCENT      = 96   
EmpyInput.KEY_WORLD_1           = 161  
EmpyInput.KEY_WORLD_2           = 162  

-- Fion keys *--
EmpyInput.KEY_ESCAPE            = 256
EmpyInput.KEY_ENTER             = 257
EmpyInput.KEY_TAB               = 258
EmpyInput.KEY_BACKSPACE         = 259
EmpyInput.KEY_INSERT            = 260
EmpyInput.KEY_DELETE            = 261
EmpyInput.KEY_RIGHT             = 262
EmpyInput.KEY_LEFT              = 263
EmpyInput.KEY_DOWN              = 264
EmpyInput.KEY_UP                = 265
EmpyInput.KEY_PAGE_UP           = 266
EmpyInput.KEY_PAGE_DOWN         = 267
EmpyInput.KEY_HOME              = 268
EmpyInput.KEY_END               = 269
EmpyInput.KEY_CAPS_LOCK         = 280
EmpyInput.KEY_SCROLL_LOCK       = 281
EmpyInput.KEY_NUM_LOCK          = 282
EmpyInput.KEY_PRINT_SCREEN      = 283
EmpyInput.KEY_PAUSE             = 284
EmpyInput.KEY_F1                = 290
EmpyInput.KEY_F2                = 291
EmpyInput.KEY_F3                = 292
EmpyInput.KEY_F4                = 293
EmpyInput.KEY_F5                = 294
EmpyInput.KEY_F6                = 295
EmpyInput.KEY_F7                = 296
EmpyInput.KEY_F8                = 297
EmpyInput.KEY_F9                = 298
EmpyInput.KEY_F10               = 299
EmpyInput.KEY_F11               = 300
EmpyInput.KEY_F12               = 301
EmpyInput.KEY_F13               = 302
EmpyInput.KEY_F14               = 303
EmpyInput.KEY_F15               = 304
EmpyInput.KEY_F16               = 305
EmpyInput.KEY_F17               = 306
EmpyInput.KEY_F18               = 307
EmpyInput.KEY_F19               = 308
EmpyInput.KEY_F20               = 309
EmpyInput.KEY_F21               = 310
EmpyInput.KEY_F22               = 311
EmpyInput.KEY_F23               = 312
EmpyInput.KEY_F24               = 313
EmpyInput.KEY_F25               = 314
EmpyInput.KEY_KP_0              = 320
EmpyInput.KEY_KP_1              = 321
EmpyInput.KEY_KP_2              = 322
EmpyInput.KEY_KP_3              = 323
EmpyInput.KEY_KP_4              = 324
EmpyInput.KEY_KP_5              = 325
EmpyInput.KEY_KP_6              = 326
EmpyInput.KEY_KP_7              = 327
EmpyInput.KEY_KP_8              = 328
EmpyInput.KEY_KP_9              = 329
EmpyInput.KEY_KP_DECIMAL        = 330
EmpyInput.KEY_KP_DIVIDE         = 331
EmpyInput.KEY_KP_MULTIPLY       = 332
EmpyInput.KEY_KP_SUBTRACT       = 333
EmpyInput.KEY_KP_ADD            = 334
EmpyInput.KEY_KP_ENTER          = 335
EmpyInput.KEY_KP_EQUAL          = 336
EmpyInput.KEY_LEFT_SHIFT        = 340
EmpyInput.KEY_LEFT_CONTROL      = 341
EmpyInput.KEY_LEFT_ALT          = 342
EmpyInput.KEY_LEFT_SUPER        = 343
EmpyInput.KEY_RIGHT_SHIFT       = 344
EmpyInput.KEY_RIGHT_CONTROL     = 345
EmpyInput.KEY_RIGHT_ALT         = 346
EmpyInput.KEY_RIGHT_SUPER       = 347
EmpyInput.KEY_MENU              = 348

-- export module
return EmpyInput
