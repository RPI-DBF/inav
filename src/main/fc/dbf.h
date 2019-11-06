
//////////////////////////////////////////////////////
//////////      False Positive Constants
//////////////////////////////////////////////////////

// Number of cycles to confirm an action
#define NUM_CYCLES 5

// Threshold for an RC signal to count (for binary actions)
#define RC_THRESHOLD 1800

//
int contiguous_cycles = 0;

//////////////////////////////////////////////////////
//////////      RC Channels
//////////////////////////////////////////////////////

// Number of actions that we want to support
#define NUM_ACTIONS 5

// RC CHANNELS

// these constants align with the index of their RC channel in RC_CHANNELS
const enum ACTIONS {
    UNROLL_BANNER,
    DROP_BANNER,
    FLAPS_LOW,
    FLAPS_MID,
    FLAPS_HIGH  // high is default
};

// note that RC_CHANNELS should remain the same length as the definition of NUM_ACTIONS
const int RC_CHANNELS[] = {
    <actual RC channel for action_1>,   // unroll_banner
    <actual RC channel for action_2>,   // drop_banner
    <actual RC channel for action_3>,   // flaps_low
    <actual RC channel for action_4>,   // flaps_mid
    <actual RC channel for action_5>    // flaps_high
};

// Expected usage (in code): RC_CHANNELS[action_1];
//               ==> this should return the actual RC channel

//////////////////////////////////////////////////////
//////////      Servo Settings
//////////////////////////////////////////////////////

// Number of servos
#define NUM_SERVOS 4

// Default positions for each of the servos
// This needs to be filled in manually (or hopefully via configurator!) for each servo
const int SERVO_DEFAULT[] = {
    1000,
    1000,
    1000,
    1000
};


//////////////////////////////////////////////////////
//////////      Function Prototypes
//////////////////////////////////////////////////////

void dbf_main();
void set_servo_position(int servo_index, int position);
int get_rc_data(int rc_channel);
int get_raw_rc_data(int rc_channel);
int int_clamp(int value, int min, int max);
