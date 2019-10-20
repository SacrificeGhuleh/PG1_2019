//
// Created by zvone on 17-Oct-19.
//

#ifndef PG1_2019_RtcRayHitIorIOR_H
#define PG1_2019_RtcRayHitIorIOR_H

#include <embree3/rtcore.h>
#include <geometry/material.h>

struct RtcRayHitIor : RTCRayHit {
  float ior = IOR_AIR;
};

#endif //PG1_2019_RtcRayHitIorIOR_H
