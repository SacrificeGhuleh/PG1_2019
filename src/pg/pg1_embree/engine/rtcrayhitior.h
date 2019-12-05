//
// Created by zvone on 17-Oct-19.
//

#ifndef PG1_2019_RtcRayHitIorIOR_H
#define PG1_2019_RtcRayHitIorIOR_H

#include <embree3/rtcore.h>
#include <geometry/material.h>

struct RtcRayHitIor : RTCRayHit {
  float ior = IOR_AIR;
  
  inline bool collided() const {return hit.geomID != RTC_INVALID_GEOMETRY_ID;}
};

struct RtcRayHitIor4 : RTCRayHit4 {
  float ior[4] = {IOR_AIR, IOR_AIR, IOR_AIR, IOR_AIR};
};

struct RtcRayHitIor8 : RTCRayHit8 {
  float ior[8] = {IOR_AIR, IOR_AIR, IOR_AIR, IOR_AIR,
                  IOR_AIR, IOR_AIR, IOR_AIR, IOR_AIR};
};

struct RtcRayHitIor16 : RTCRayHit16 {
  float ior[16] = {IOR_AIR, IOR_AIR, IOR_AIR, IOR_AIR,
                   IOR_AIR, IOR_AIR, IOR_AIR, IOR_AIR,
                   IOR_AIR, IOR_AIR, IOR_AIR, IOR_AIR,
                   IOR_AIR, IOR_AIR, IOR_AIR, IOR_AIR};
};

#endif //PG1_2019_RtcRayHitIorIOR_H
