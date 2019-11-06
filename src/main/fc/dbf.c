
#include "drivers/pwm_output.h"
#include "flight/servos.h"
#include "dbf.h"

void dbf_main()
{
    // if the servos haven't been moved, set them to their defaults

    // check for RC movement



}

/**
    Sets the servo at index servo_index to the given position

    @param servo_index - the index of the servo that we're modifying
    @param position - the percentage of the range of motion we are setting
                        the servo to (from 0 to 100)

    @requires 0 <= servo_index < NUM_SERVOS && 0 <= position <= 100

    @effects servo[servo_index] = position
**/
void set_servo_position(int servo_index, int position)
{
    // 0-100 -> 0-1000 -> 1000-2000
    int real_position = 10 * position + 1000;

    servo[servo_index] = real_position; // do we need this line?

    pwmWriteServo(servo_index, servo[servo_index]);
}


/**
    Obtains the 0-100 RC reading for the given action.

    @param action - an action from the enum ACTIONS

    @requires 0 <= action < NUM_ACTIONS

    @returns the percentage (0-100) reading on that rc channel for that action
**/
int get_rc_data(int action) {
    return (get_raw_rc_data(action) - 1000) / 10;
}

/**
    Obtains the 1000-2000 RC reading for the given action.

    @param action - an action from the enum ACTIONS

    @requires 0 <= action < NUM_ACTIONS

    @returns the 1000-2000 data from the rc channel for that action
**/
int get_raw_rc_data(int action) {
    return int_clamp(rcData[RC_CHANNELS[action]], 1000, 2000);
}

/**
    Clamps an integer to a given range

    @param value    the value to clamp
    @param min      the minimum value to clamp up to
    @param max      the maximum value to clamp down to

    @requires       min < max

    @returns
            if value < min
                return min
            if value > max:
                return max
            return value
**/
int int_clamp(int value, int min, int max){
	return (value < min) ? min : (value > max) ? max : value;
}



void bombDropping()
{
	//for(int i = 0; i < 4; i++){
	//	//if(!beenDropped[i] || beenDropped[i] >= numLoopsToDrop){
	//	if(!beenDropped[i]){
	//		servo[i] = 1000;
	//	} /*else {
	//		beenDropped[i]++;
	//	}*/
	//}

    if (rcData[RC_reset] > RC_THRESHOLD) {
        contig_dropNext = 0;
        bombIndex = 0;
        for(int i = 0; i < 4; i++){
			beenDropped[i] = 0;
		}
        servo[0] = 1000;
        servo[1] = 2000;
        servo[2] = 2000;
        servo[3] = 1000;
        return;
    }

    if (beenDropped[0])
        servo[0] = 2000;
    if (beenDropped[1])
        servo[1] = 1000;
    if (beenDropped[2])
        servo[2] = 1000;
    if (beenDropped[3])
        servo[3] = 2000;

    // filter out false positives
    if(rcData[RC_dropNext] > RC_THRESHOLD){
        contig_dropNext++;
    } else {
        contig_dropNext = 0;
    }


	if(rcData[RC_dropSpecific] > RC_THRESHOLD){
		contig_dropSpecific++;
	} else {
		contig_dropSpecific = 0;
	}

	int dropDial_val;
	dropDial_val = int_clamp(rcData[RC_dropDial], 1000, 2000);

	/*
	*	0 - don't drop
	*	1 - drop next in order (out_left, out_right, in_left, in_right)
	*	2 - drop specific bomb as designated by dial
	*/

	if(contig_dropNext >= numLoopsToDrop){
		droppingPayload = 1;
	} else if (contig_dropSpecific >= numLoopsToDrop){
		droppingPayload = 2;
	} else {
		droppingPayload = 0;
	}

    // if we switch was JUST pressed, payload_val = 1000
    if(!dropping_prev_state && droppingPayload){
        servo_payload_value = 1000;
    } else if(dropping_prev_state && !droppingPayload){
		// if we just let go, then move
        servo_payload_value = 2000;
    } else {
        servo_payload_value = 0;
    }

	// determine the servo once you press the switch
	if (droppingPayload == 2){
		// process rc data from dial based channel
		/*
		*	1000 to 1250	-	0
		*	1251 to 1500	-	1
		*	1501 to 1750	-	2
		*	1751 to	2000	-	3
		*/
		targetServo = (dropDial_val - 1001) / 250;

		// note: this code allows you to "drop" the same bomb twice using dropSpecific

	} else if (!dropping_prev_state && droppingPayload == 1){
		// just drop the next one
		targetServo = 0;
        bool broke_out = false;
		while(beenDropped[targetServo]){
			if(bombIndex > 3) {
                broke_out = true;
                break;
            }
			targetServo = drop_order[bombIndex];
            bombIndex++;
            if (broke_out)
                targetServo = -1;
		}
		// DEBUG_TRACE("Target Servo #%d\r\n", targetServo);
		// DEBUG_TRACE("Hitting Servo #%d\r\n", bombs[targetServo]);
	}

    if (targetServo != -1 && servo_payload_value != 0) {
        // we are dropping this bomb, so it's unavailable to drop again

		if(!beenDropped[targetServo]){
			beenDropped[targetServo]++;
		}

        if (targetServo == 0 || targetServo == 3) {
            if (servo_payload_value == 2000)
                servo_payload_value = 1000;
            else
                servo_payload_value = 2000;
        }

        servo[bombs[targetServo]] = servo_payload_value;
        DEBUG_TRACE("servo %d with value %d\r\n", bombs[targetServo], servo_payload_value);
    }

    dropping_prev_state = droppingPayload;
    //DEBUG_TRACE("%d, %d, %d, %d\r\n", servo[0], servo[1], servo[2], servo[3]);
    //DEBUG_TRACE("%d, \r\n", servo[0]);
}

/*
void radome() {
    servo[5] = RADOME_STOPPED;
    if (rcData[RC_radome] > RC_THRESHOLD) {
        servo[5] = RADOME_MOVING;
    }
}
*/

void folding() {
    servo[4] = CH4_CLOSED;
    servo[5] = CH5_CLOSED;
    if (rcData[RC_folding] > RC_THRESHOLD) {
        servo[4] = CH4_OPEN;
        servo[5] = CH5_OPEN;
    }
}
