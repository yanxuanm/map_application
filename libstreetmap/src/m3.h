#pragma once
#include <vector>
#include <string>
#include "m2.h"

extern std::vector<unsigned> *intersection_to_segments1;
extern vector<unsigned> closedset;
extern unordered_map<double, unsigned> openset1;
extern set<unsigned> openset;
extern unordered_map<unsigned, unsigned> cameFrom;
extern unordered_map<unsigned, unsigned> cameFrom_seg;
extern unordered_map<unsigned, double> g_score;
extern unordered_map<unsigned, double> h_score;
extern unordered_map<unsigned, double> f_score;
extern unordered_multimap<string, unsigned> poiname_to_id;
extern vector<LatLon> poiid_to_position;

// Returns a path (route) between the start intersection and the end 
// intersection, if one exists. If no path exists, this routine returns 
// an empty (size == 0) vector. If more than one path exists, the path 
// with the shortest travel time is returned. The path is returned as a vector 
// of street segment ids; traversing these street segments, in the given order,
// would take one from the start to the end intersection.
std::vector<unsigned> find_path_between_intersections(unsigned intersect_id_start, unsigned intersect_id_end);


// Returns the time required to travel along the path specified. The path
// is passed in as a vector of street segment ids, and this function can 
// assume the vector either forms a legal path or has size == 0.
// The travel time is the sum of the length/speed-limit of each street 
// segment, plus 15 seconds per turn implied by the path. A turn occurs
// when two consecutive street segments have different street names.
double compute_path_travel_time(const std::vector<unsigned>& path);


// Returns the shortest travel time path (vector of street segments) from 
// the start intersection to a point of interest with the specified name.
// If no such path exists, returns an empty (size == 0) vector.
std::vector<unsigned> find_path_to_point_of_interest(unsigned intersect_id_start, std::string point_of_interest_name);

double heuristic_estimate_of_time(unsigned int1, unsigned int2);

vector<unsigned> reconstruct_path(unordered_map<unsigned, unsigned> cameFrom, unordered_map<unsigned, unsigned>cameFrom_seg, unsigned current, unsigned from);

bool if_ele_exist(vector<unsigned> set, unsigned);


double find_time(unsigned int1, unsigned int2);

double find_time_(unsigned int1, unsigned int2, unsigned intfrom);

vector<unsigned> convert_path(vector<unsigned> path_in_int);

struct comparator
{
    bool operator()(pair<unsigned, double > &pair1, pair<unsigned, double> &pair2)
    {
        return pair1.second > pair2.second;
    }
};



