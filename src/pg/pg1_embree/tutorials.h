#ifndef TUTORIALS_H_
#define TUTORIALS_H_

void error_handler(void *user_ptr, RTCError code, const char *str = nullptr);

int tutorial_1(const char *config = "threads=0,verbose=3");

int tutorial_2();

int tutorial_3(std::string file_name, const char *config = "threads=0,verbose=0");

int staticSceneSphere(const char *config = "threads=0,verbose=0");

int staticSceneGeoSpheres(const char *config = "threads=0,verbose=0");

int staticSceneShip(const char *config = "threads=0,verbose=0");

int staticSceneCornellbox(const char *config = "threads=0,verbose=0");

#endif
