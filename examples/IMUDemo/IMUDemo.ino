#include <Poom.h>
#include <PoomIMU.h>

bool imuReady = false;
PoomIMU imu;

void setup()
{
    Poom.begin();
    Poom.setFrameRate(30);
    imuReady = imu.beginAuto();
}

void loop()
{
    if (!Poom.nextFrame()) {
        return;
    }

    Poom.clear();
    Poom.setCursor(0, 0);
    Poom.println("POOM IMU");

    if (!imuReady) {
        Poom.println("NOT FOUND");
        Poom.show();
        return;
    }

    (void)imu.poll();
    char line[22];
    snprintf(line, sizeof(line), "AX %d", imu.sample().accelerationRaw[PoomIMUAxisX]);
    Poom.println(line);
    snprintf(line, sizeof(line), "AY %d", imu.sample().accelerationRaw[PoomIMUAxisY]);
    Poom.println(line);
    snprintf(line, sizeof(line), "AZ %d", imu.sample().accelerationRaw[PoomIMUAxisZ]);
    Poom.println(line);
    snprintf(line, sizeof(line), "TEMP %.1f C", imu.temperatureC());
    Poom.println(line);
    Poom.show();
}
