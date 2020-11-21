/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "SDL_config.h"

#if defined(SDL_SENSOR_VITA)

#include "SDL_error.h"
#include "SDL_sensor.h"
#include "SDL_vitasensor.h"
#include "../SDL_syssensor.h"
#include <psp2/motion.h>
#include <psp2/error.h>

typedef struct
{
    SDL_SensorType type;
    SDL_SensorID instance_id;
} SDL_VitaSensor;

static SDL_VitaSensor *SDL_sensors;
static int SDL_sensors_count;

static int
SDL_VITA_SensorInit(void)
{
    sceMotionReset();
    sceMotionStartSampling();
//    sceMotionMagnetometerOn();
    sceMotionSetAngleThreshold(40);
    sceMotionSetGyroBiasCorrection(SCE_TRUE);

    SDL_sensors_count = 2;

    SDL_sensors = (SDL_VitaSensor *)SDL_calloc(SDL_sensors_count, sizeof(*SDL_sensors));
    if (!SDL_sensors) {
        return SDL_OutOfMemory();
    }

    SDL_sensors[0].type = SDL_SENSOR_ACCEL;
    SDL_sensors[0].instance_id = SDL_GetNextSensorInstanceID();
    SDL_sensors[1].type = SDL_SENSOR_GYRO;
    SDL_sensors[1].instance_id = SDL_GetNextSensorInstanceID();

    return 0;
}

static int
SDL_VITA_SensorGetCount(void)
{
    return SDL_sensors_count;
}

static void
SDL_VITA_SensorDetect(void)
{
}

static const char *
SDL_VITA_SensorGetDeviceName(int device_index)
{
    if (device_index < SDL_sensors_count) {
        switch (SDL_sensors[device_index].type) {
            case SDL_SENSOR_ACCEL:
                return "Accelerometer";
            case SDL_SENSOR_GYRO:
                return "Gyro";
            default:
                return "Unknown";
        }
    }

    return NULL;
}

static SDL_SensorType
SDL_VITA_SensorGetDeviceType(int device_index)
{
    if (device_index < SDL_sensors_count) {
        return SDL_sensors[device_index].type;
    }

    return SDL_SENSOR_INVALID;
}

static int
SDL_VITA_SensorGetDeviceNonPortableType(int device_index)
{
    if (device_index < SDL_sensors_count) {
        return SDL_sensors[device_index].type;
    }
    return -1;
}

static SDL_SensorID
SDL_VITA_SensorGetDeviceInstanceID(int device_index)
{
    if (device_index < SDL_sensors_count) {
        return SDL_sensors[device_index].instance_id;
    }
    return -1;
}

static int
SDL_VITA_SensorOpen(SDL_Sensor *sensor, int device_index)
{
    return 0;
}

static void
SDL_VITA_SensorUpdate(SDL_Sensor *sensor)
{
    SceMotionState motionState;
    int err = SCE_OK;
    err = sceMotionGetState(&motionState);
    if (err != SCE_OK) return;

    switch (sensor->type)
    {
        case SDL_SENSOR_ACCEL:
        {
            float data[3];
            data[0] = motionState.acceleration.x * SDL_STANDARD_GRAVITY;
            data[1] = motionState.acceleration.y * SDL_STANDARD_GRAVITY;
            data[2] = motionState.acceleration.z * SDL_STANDARD_GRAVITY;
            SDL_PrivateSensorUpdate(sensor, data, SDL_arraysize(data));
        }
        break;
        case SDL_SENSOR_GYRO:
        {
            float data[3];
            data[0] = motionState.angularVelocity.x;
            data[1] = motionState.angularVelocity.y;
            data[2] = motionState.angularVelocity.z;
            SDL_PrivateSensorUpdate(sensor, data, SDL_arraysize(data));
        }
        break;
        default:
        break;
    }
}

static void
SDL_VITA_SensorClose(SDL_Sensor *sensor)
{
}

static void
SDL_VITA_SensorQuit(void)
{
    sceMotionStopSampling();
}

SDL_SensorDriver SDL_VITA_SensorDriver =
{
    SDL_VITA_SensorInit,
    SDL_VITA_SensorGetCount,
    SDL_VITA_SensorDetect,
    SDL_VITA_SensorGetDeviceName,
    SDL_VITA_SensorGetDeviceType,
    SDL_VITA_SensorGetDeviceNonPortableType,
    SDL_VITA_SensorGetDeviceInstanceID,
    SDL_VITA_SensorOpen,
    SDL_VITA_SensorUpdate,
    SDL_VITA_SensorClose,
    SDL_VITA_SensorQuit,
};

#endif /* SDL_SENSOR_VITA */

/* vi: set ts=4 sw=4 expandtab: */
