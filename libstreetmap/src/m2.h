#pragma once

#include "m1.h"
#include <iostream>
#include <cfloat> 
#include <chrono>
#include <thread>
#include <cstdlib>
#include <vector>
#include "graphics.h"
#include <array>
#include "math.h"
#include <sstream>
#include "m3.h"


void drawscreen(void);

void draw_map(void);

void draw_feature(unsigned featureID);
void draw_feature_name(unsigned featureID);
void draw_river(unsigned featureID);
void drawlineofset(t_point *pts, unsigned number);
t_point find_close_feature_mid_point(t_point* pts, unsigned number);
t_point find_open_feature_mid_point(t_point* pts, unsigned number);
void draw_subway(OSMWay* subways, const t_color& color);
void draw_motorway(unsigned segmentid, const t_color& color);
void draw_primary(unsigned segmentid, const t_color& color);
void draw_unclassified(unsigned segmentid, const t_color& color);
void draw_residential(unsigned segmentid, const t_color& color);
void draw_service(unsigned segmentid, const t_color& color);
double find_degree(unsigned segmentid);

void draw_text(unsigned segmentid);

double distance_between_two_t_points(t_point t1, t_point t2);
void act_on_button_press(float x, float y, t_event_buttonPressed event);

void draw_arrow(unsigned segmentid);

double find_degree_(t_point t1, t_point t2);

void act_on_new_button_gas(void (*drawscreen_ptr) (void));
void act_on_new_button_find(void (*drawscreen_ptr) (void));
void draw_gas();
void act_on_new_button_int(void (*drawscreen_ptr) (void));

void act_on_new_button_interest(void (*drawscreen_ptr) (void));
void print_big_name();
void act_on_new_button_name(void (*drawscreen_ptr) (void));
void act_on_new_button_arrow(void (*drawscreen_ptr) (void));

void act_on_new_button_parking(void (*drawscreen_ptr) (void));
void act_on_new_button_wash(void (*drawscreen_ptr) (void));
void act_on_new_button_sharing(void (*drawscreen_ptr) (void));
void act_on_new_button_rental(void (*drawscreen_ptr) (void));

void act_on_new_button_findPath(void (*drawscreen_ptr) (void));
void act_on_new_button_help(void (*drawscreen_ptr) (void));
void act_on_new_button_travelDirection(void (*drawscreen_ptr) (void));

void print_travel_direction(std::vector<unsigned> path);

void draw_parking();
void draw_washing();
void draw_sharing();
void draw_rental();

char** street_Name_command_completion(const char* stem_text, int start, int end);
char* street_name_generator(const char* stem_text, int state);

