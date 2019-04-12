
//
// For RAMPS v1.4
//
#define FARMBOT_BOARD_ID 0

//
// For FARMDUINO v1.0
//
//#define FARMBOT_BOARD_ID 1

//
// For FARMDUINO v1.4
//
//#define FARMBOT_BOARD_ID 2

#ifndef FARMBOT_BOARD_ID

  //#define RAMPS_V14
  //#define FARMDUINO_V10
  #define FARMDUINO_V14

#else

  #undef RAMPS_V14
  #undef FARMDUINO_V10
  #undef FARMDUINO_V14

  #if FARMBOT_BOARD_ID == 0
    #define RAMPS_V14
  #elif FARMBOT_BOARD_ID == 1
    #define FARMDUINO_V10
  #elif FARMBOT_BOARD_ID == 2
    #define FARMDUINO_V14
  #endif

#endif
