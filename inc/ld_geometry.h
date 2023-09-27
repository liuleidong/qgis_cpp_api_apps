#ifndef LDGEOMETRY_H
#define LDGEOMETRY_H

typedef struct _SGeometryInfo{
    int deviceId;
    int deviceType;
    int deviceBattery;
    //# Latitude [degrees]. Positive is north of equator; negative is south.
    double latitude;
    //# Longitude [degrees]. Positive is east of prime meridian; negative is west.
    double longitude;
    //# Altitude [m]. Positive is above the WGS 84 ellipsoid
    //# (quiet NaN if no altitude is available).
    double altitude;
}SGeometryInfo;

typedef struct _SGeometryPoint{
    double x;
    double y;
    double z;
    double m;
}SGeometryPoint;

#endif // LDGEOMETRY_H
