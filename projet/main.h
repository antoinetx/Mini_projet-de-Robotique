#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"


//constants for the differents parts of the project
#define IMAGE_BUFFER_SIZE		640
// les prochains en dessous sont juste mis au pif, a compléter avec des vrai valeurs
#define GOAL_DISTANCE			10
#define ROTATION_THRESHOLD		10
#define ROTATION_COEFF			10
#define KP						800
#define KI						3.5f
#define MIN_LINE_WIDTH			40
#define MAX_DISTANCE			180
#define WIDTH_SLOPE				5
#define PXTOCM					1


/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif
