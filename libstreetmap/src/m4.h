#pragma once
#include <vector>
#include <m3.h>

struct DeliveryInfo {
    //Specifies a delivery order.
    //
    //To satisfy the order the item-to-be-delivered must have been picked-up 
    //from the pickUp intersection before visiting the dropOff intersection.

    DeliveryInfo(unsigned pick_up, unsigned drop_off)
        : pickUp(pick_up), dropOff(drop_off) {}


    //The intersection id where the item-to-be-delivered is picked-up.
    unsigned pickUp;

    //The intersection id where the item-to-be-delivered is dropped-off.
    unsigned dropOff;
};


// This routine takes in a vector of N deliveries (pickUp, dropOff
// intersection pairs) and another vector of M intersections that
// are legal start and end points for the path. 
//
// The first vector gives the delivery information: a set of 
// pickUp/dropOff pairs of intersection ids which specify the 
// deliveries to be made. A delivery can only be dropped-off after 
// the associated item has been picked-up. 
// 
// The second vector gives the intersection
// ids of courier company depots containing trucks; you start at any
// one of these depots and end at any one of the depots.
//
// This routine returns a vector of street segment ids that form a
// path, where the first street segment id is connected to a depot
// intersection, and the last street segment id also connects to a
// depot intersection.  The path must traverse all the delivery
// intersections in an order that allows all deliveries to be made --
// i.e. a package won't be dropped off if you haven't picked it up
// yet.
//
// You can assume that N is always at least 1, and M is always at
// least one (i.e. both input vectors are non-empty).
//
// It is legal for the same intersection to appear multiple times in
// the pickUp or dropOff list (e.g. you might have two deliveries with
// a pickUp intersection id of #50). The same intersection can also
// appear as both a pickUp location and a dropOff location.
//        
// If you have two pickUps to make at an intersection, 
// traversing the intersection once is sufficient
// to pick up both packages, and similarly one traversal of an 
// intersection is sufficient to drop off all the packages that have
// already been picked up and need to be dropped off at that intersection.
//
// Depots will never appear as pickUp or dropOff locations for deliveries.
//  
// If no path connecting all the delivery locations
// and a start and end depot exists, this routine should return an
// empty (size == 0) vector.

std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots);
vector<unsigned> ints_to_path(vector<unsigned> ints);
unsigned find_nearest_index(set<unsigned> goal, unsigned begin);

unsigned find_nearest_int_vector(vector<unsigned> goal, unsigned begin) ;

 vector<unsigned> twooptSwap(vector<unsigned> pathint, unsigned a, unsigned b);

double find_distance_between_two_points_t_points(t_point t1,t_point t2);
double find_path_distance(vector<unsigned> path);
void thread_func(vector<unsigned> ints);
void call_from_thread(vector<unsigned> ints, int num);

bool courier_path_is_legal(const std::vector<DeliveryInfo>& deliveries,
                           const std::vector<unsigned>& depots,
                           const std::vector<unsigned>& path);

bool valid_courier_problem(const std::vector<DeliveryInfo>& deliveries_vec,
                           const std::vector<unsigned>& depots_vec);

bool delivered_all_packages(const std::vector<DeliveryInfo>& deliveries,
                            const std::vector<bool>& deliveries_picked_up,
                            const std::vector<bool>& deliveries_dropped_off);

void drop_off_at_intersection(const std::vector<DeliveryInfo>& deliveries,
                              const std::multimap<unsigned,size_t>& intersections_to_drop_off,
                              const std::vector<bool>& deliveries_picked_up,
                              const unsigned curr_intersection,
                              std::vector<bool>& deliveries_dropped_off);

void pick_up_at_intersection(const std::vector<DeliveryInfo>& deliveries,
                             const std::multimap<unsigned,size_t>& intersections_to_pick_up,
                             const unsigned curr_intersection,
                             std::vector<bool>& deliveries_picked_up);

bool traverse_segment(const std::vector<unsigned>& path,
                      const unsigned path_idx,
                      const unsigned curr_intersection,
                      unsigned& next_intersection);
bool is_depot(const std::vector<unsigned>& depots,
              const unsigned intersection_id);

bool determine_start_intersection(const std::vector<unsigned>& path,
                                  const std::vector<unsigned>& depots,
                                  unsigned& start_intersection) ;

double find_distance_between_two_ints(unsigned int1,unsigned int2);

bool if_in_vec(vector<unsigned>& vec, unsigned ele);

bool path_legal(const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots,
        const std::vector<unsigned>& pathint);