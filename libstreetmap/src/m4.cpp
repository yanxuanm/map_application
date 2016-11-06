#include "m4.h"
#include <set>
#include <vector>
#include<float.h>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <algorithm> 
#include <stdlib.h> 
#include <mutex>
#define TIME_LIMIT 30
#define NUM_THREADS 8
extern std::vector<t_point> allints;
extern vector<unsigned> dropoffints;
extern vector<bool> pickupints;


unordered_map<unsigned, unordered_map<unsigned, double>>pathtime_ints;
unordered_multimap<unsigned, unsigned> dropoff_to_pickup;
unordered_multimap<unsigned, unsigned> pickup_to_dropoff;
//unordered_map<unsigned, unordered_map<unsigned, vector<unsigned>>>pathsegs;
double current_dis;
map<double, vector<unsigned>> time_to_path;
vector<DeliveryInfo> deliveriesglobal;
vector<unsigned> depotsglobal;

using namespace std;

mutex mtx;

std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots) {
    auto startTime = chrono::high_resolution_clock::now();
    bool timeOut = false;
    deliveriesglobal = deliveries;
    depotsglobal = depots;
    vector<unsigned> pathint;
    vector<unsigned> shortestpathint;
    vector<unsigned> path;
    vector<unsigned> shortestpath;
    vector<unsigned> temppath;
    vector<unsigned> dep_1 = depots;
    vector<unsigned> total;
    unordered_multimap<unsigned, unsigned> dropofffirst;
    unordered_multimap<unsigned, unsigned> dropoff;
    set<unsigned> dest;
    set<unsigned> destfirst;
    double timecompare = DBL_MAX;
    unsigned intersect;
    unsigned intersection1;
    unsigned intersection2;
    //    unsigned unreachable_depot;



    for (unsigned i = 0; i < deliveries.size(); i++) {
        dropofffirst.insert(pair<unsigned, unsigned>(deliveries[i].pickUp, deliveries[i].dropOff));
        dropoff_to_pickup.insert(pair<unsigned, unsigned>(deliveries[i].dropOff, deliveries[i].pickUp));
        destfirst.insert(deliveries[i].pickUp);
        total.push_back(deliveries[i].pickUp);
        total.push_back(deliveries[i].dropOff);
    }
    pickup_to_dropoff = dropofffirst;
    for (unsigned i = 0; i < depots.size(); i++) {
        total.push_back(depots[i]);

    }
    thread_func(total);

    
    for (auto iter = dep_1.begin(); iter != dep_1.end();) {
        pathint.clear();
        //        path.clear();
        dropoff = dropofffirst;
        dest = destfirst;
        intersect = find_nearest_index(dest, *iter);
        intersection1 = *iter;
        intersection2 = intersect;
        pathint.push_back(intersection1);
        pathint.push_back(intersection2);

        //        path = pathsegs[intersection1][intersection2];
        path = find_path_between_intersections(intersection1, intersection2);
        while (path.size() == 0) {
            iter = dep_1.erase(iter);
            intersect = find_nearest_index(dest, *iter);
            intersection1 = *iter;
            intersection2 = intersect;
            pathint[0] = intersection1;
            pathint[1] = intersection2;
            path = find_path_between_intersections(intersection1, intersection2);
            //            path = pathsegs[intersection1][intersection2];
        }

        dest.erase(intersection2);
        auto range = dropoff.equal_range(intersection2);
        for (auto it = range.first; it != range.second;) {
            dest.insert(it->second);
            auto iter = it;
            it++;
            dropoff.erase(iter);
        }



        while (!dest.empty()) {
            intersect = find_nearest_index(dest, intersection2);
            intersection1 = intersect;
            pathint.push_back(intersection1);
            if (dropoff.find(intersection1) != dropoff.end()) {
                dest.erase(intersection1);
                auto range = dropoff.equal_range(intersection1);
                for (auto it = range.first; it != range.second;) {
                    dest.insert(it->second);
                    auto iter = it;
                    it++;
                    dropoff.erase(iter);
                }
                //                temppath = pathsegs[intersection2][intersection1];
                //                path.insert(path.end(), temppath.begin(), temppath.end());
            } else {
                dest.erase(intersection1);
                //                temppath = pathsegs[intersection2][intersection1];
                //                path.insert(path.end(), temppath.begin(), temppath.end());
            }
            intersection2 = intersection1;
        }

        intersect = find_nearest_int_vector(dep_1, intersection2);
        pathint.push_back(intersect);
        //        temppath = pathsegs[intersection2][intersect];
        //        while (temppath.size() == 0) {
        //            unreachable_depot = intersect;
        //            t_point start = allints[intersection2];
        //            double distance = DBL_MAX;
        //            for (unsigned i = 0; i < dep_1.size(); i++) {
        //                if (dep_1[i] != unreachable_depot) {
        //                    double temp = find_distance_between_two_points_t_points(allints[dep_1[i]], start);
        //                    if (temp < distance) {
        //                        distance = temp;
        //                        intersect = dep_1[i];
        //                    }
        //                }
        //            }
        //
        //            temppath = find_path_between_intersections(intersection2, intersect);
        //            pathint[pathint.size() - 1] = intersect;
        //
        //        }
        //        path.insert(path.end(), temppath.begin(), temppath.end());

        double time = find_path_distance(pathint);
        if (time < timecompare) {
            shortestpathint = pathint;
            timecompare = time;
        }

        iter++;


    }
    double currenttime = timecompare;
    //    //    while (timecompare > currenttime) {
    cout << "start2opt" << endl;
startagain:
    if (!timeOut) {
        for (unsigned i = 1; i < shortestpathint.size() - 2; i++) {
            for (unsigned j = i + 1; j < shortestpathint.size() - 1; j++) {

                vector<unsigned> newroute = twooptSwap(shortestpathint, i, j);
                double newtime = find_path_distance(newroute);
                if (newtime < currenttime && path_legal(deliveries, depots, newroute)) {
                    shortestpathint = newroute;
                    currenttime = newtime;
                    auto currentTime = chrono::high_resolution_clock::now();
                    auto wallClock = chrono::duration_cast<chrono::duration<double>> (currentTime - startTime);
                    // Keep optimizing until within 10% of time limit
                    if (wallClock.count() > 0.9 * TIME_LIMIT)
                        timeOut = true;
                    goto startagain;
                }

            }
        }


    }



    //    int time_limit;
    //    if (shortestpathint.size() > 20) {
    //        time_limit = 30;
    //    } else {
    //        time_limit = 10;
    //    }
    //    while (!timeOut) {
    //        //        thread_func(shortestpathint);
    //
    //        auto currentTime = chrono::high_resolution_clock::now();
    //        auto wallClock = chrono::duration_cast<chrono::duration<double>> (currentTime - startTime);
    //        // Keep optimizing until within 10% of time limit
    //        if (wallClock.count() > 0.9 * TIME_LIMIT)
    //            timeOut = true;
    //    }


    cout << "finish" << endl;
    return ints_to_path(shortestpathint);

}

vector<unsigned> ints_to_path(vector<unsigned> ints) {
    vector<unsigned>path;
    vector<unsigned>temppath;
    for (unsigned i = 0; i < ints.size() - 1; i++) {
        temppath = find_path_between_intersections(ints[i], ints[i + 1]);
        path.insert(path.end(), temppath.begin(), temppath.end());
    }

    return path;

}

unsigned find_nearest_index(set<unsigned> goal, unsigned begin) {
    unsigned ret;
    double distance = DBL_MAX;
    for (auto it = goal.begin(); it != goal.end(); it++) {
        double temp = pathtime_ints[begin][*it];
        if (temp < distance) {
            ret = *it;
            distance = temp;
            current_dis = temp;
        }
    }

    return ret;


}

unsigned find_nearest_int_vector(vector<unsigned> goal, unsigned begin) {
    unsigned ret;
    double distance = DBL_MAX;
    for (unsigned i = 0; i < goal.size(); i++) {
        double temp = pathtime_ints[begin][goal[i]];
        if (temp < distance && temp != 0) {
            distance = temp;
            ret = goal[i];
        }
    }

    return ret;




}

double find_distance_between_two_points_t_points(t_point t1, t_point t2) {

    return (pow(t1.x - t2.x, 2) + pow(t1.y - t2.y, 2));


}

double find_distance_between_two_ints(unsigned int1, unsigned int2) {
    t_point t1 = allints[int1];
    t_point t2 = allints[int2];
    return (pow(t1.x - t2.x, 2) + pow(t1.y - t2.y, 2));

}

double find_path_distance(vector<unsigned> path) {
    double distance = 0;
    for (unsigned i = 0; i < path.size() - 1; i++) {
        distance += pathtime_ints[path[i]][path[i + 1]];
    }
    return distance;
}

void thread_func(vector<unsigned> ints) {
    thread myThread[NUM_THREADS];
    int i;
    for (i = 0; i < NUM_THREADS - 1; i++) // Launch threads
        myThread[i] = thread(call_from_thread, ints, i);
    // Main thread waits for all threads to complete
    call_from_thread(ints, i);
    for (int i = 0; i < NUM_THREADS - 1; i++)
        myThread[i].join();

    return;
}

void call_from_thread(vector<unsigned> ints, int num) {
    unsigned times = ints.size() / NUM_THREADS;
    unsigned start = times*num;
    unsigned end;
    if (num != NUM_THREADS - 1) {
        end = start + times;
    } else {
        end = ints.size();
    }

    for (unsigned i = start; i < end; i++) {
        for (unsigned j = 0; j < ints.size(); j++) {

            double time = find_distance_between_two_ints(ints[i], ints[j]);
            mtx.lock();
            pathtime_ints[ints[i]][ints[j]] = time;
            //            pathsegs[ints[i]][ints[j]] = storepath;
            mtx.unlock();

        }



    }


    //    if ((plus + id2) < ints.size()) {
    //        vector<unsigned> storepath;
    //        storepath = find_path_between_intersections(ints[id1], ints[id2+plus]);
    //        double time = compute_path_travel_time(storepath);
    //        mtx.lock();
    //        pathtime_ints[ints[id1]][ints[id2+plus]] = time;
    //        pathsegs[ints[id1]][ints[id2+plus]] = storepath;
    //        mtx.unlock();



    //        vector<unsigned> path_ = pathint;
    //        unsigned p1, p2;
    //        p1 = rand() % (path_.size() - 2) + 1;
    //        p2 = rand() % (path_.size() - 2) + 1;
    //        unsigned seg1 = path_[p1];
    //        unsigned seg2 = path_[p2];
    //        path_[p1] = seg2;
    //        path_[p2] = seg1;
    //        vector<unsigned>pathseg = ints_to_path(path_);
    //        if (courier_path_is_legal(deliveriesglobal, depotsglobal, pathseg)) {
    //            mtx.lock();
    //            time_to_path.insert(pair<double, vector<unsigned>>(find_path_distance(pathseg), path_));
    //            mtx.unlock();
    //            return;
    //        } else {
    //            return;
    //        }
    //    } else {
    //        return;
    //    }
}

vector<unsigned> twooptSwap(vector<unsigned> pathint, unsigned a, unsigned b) {
    vector<unsigned> temppath;
    vector<unsigned> retpath;
    //    for (auto it = pathint.begin(); it != pathint.end(); it++) {
    //        cout << *it << "   ";
    //
    //    }
    //    cout << endl;
    for (unsigned i = 0; i < a; i++) {
        retpath.push_back(pathint[i]);
    }
    for (unsigned i = a; i <= b; i++) {
        temppath.push_back(pathint[i]);
    }
    //    for (auto it = temppath.begin(); it != temppath.end(); it++) {
    //        cout << *it << "   ";
    //
    //    }
    //    cout << endl;
    reverse(temppath.begin(), temppath.end());
    retpath.insert(retpath.end(), temppath.begin(), temppath.end());
    for (unsigned i = b + 1; i < pathint.size(); i++) {
        retpath.push_back(pathint[i]);
    }

    //    for (auto it = retpath.begin(); it != retpath.end(); it++) {
    //        cout << *it << "   ";
    //
    //    }
    //    cout << endl;

    return retpath;
}

bool courier_path_is_legal(const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots,
        const std::vector<unsigned>& path) {

    //
    //Ensuree we have a valid problem specification
    //
    if (!valid_courier_problem(deliveries, depots)) {
        return false;
    }

    if (path.empty()) {
        //If it is a valid path it must contain at-least one segment
        //(since we gaurentee there is at-least one depot and delivery)
        //        std::cerr << "Invalid courier path: path was empty\n";

        return false;

    } else {
        assert(!path.empty());

        //
        //Determine what our start intersection is (it must be a depot)
        //
        unsigned curr_intersection;
        if (!determine_start_intersection(path, depots, curr_intersection)) {
            return false;
        }

        //curr_intersection must be a depot
        assert(is_depot(depots, curr_intersection));


        //We store whether each delivery has been picked-up or dropped-off
        std::vector<bool> deliveries_picked_up(deliveries.size(), false);
        std::vector<bool> deliveries_dropped_off(deliveries.size(), false);

        //We also build fast-lookups from: intersection id to DeliveryInfo index for both
        //pickUp and dropOff intersections
        std::multimap<unsigned, size_t> intersections_to_pick_up; //Intersection_ID -> deliveries index
        std::multimap<unsigned, size_t> intersections_to_drop_off; //Intersection_ID -> deliveries index

        //Load the look-ups
        for (size_t delivery_idx = 0; delivery_idx < deliveries.size(); ++delivery_idx) {
            unsigned pick_up_intersection = deliveries[delivery_idx].pickUp;
            unsigned drop_off_intersection = deliveries[delivery_idx].dropOff;

            intersections_to_pick_up.insert(std::make_pair(pick_up_intersection, delivery_idx));
            intersections_to_drop_off.insert(std::make_pair(drop_off_intersection, delivery_idx));
        }

        //We verify the path by walking along each segment and:
        //  * Checking that the next step along the path is valid (see traverse_segment())
        //  * Recording any package pick-ups (see pick_up_at_intersection())
        //  * Recording any package drop-offs (see drop_off_at_intersection())
        for (size_t path_idx = 0; path_idx < path.size(); ++path_idx) {

            unsigned next_intersection; //Set by traverse_segment

            if (!traverse_segment(path, path_idx, curr_intersection, next_intersection)) {
                return false;
            }

            //Process packages
            pick_up_at_intersection(deliveries,
                    intersections_to_pick_up,
                    curr_intersection,
                    deliveries_picked_up);

            drop_off_at_intersection(deliveries,
                    intersections_to_drop_off,
                    deliveries_picked_up,
                    curr_intersection,
                    deliveries_dropped_off);

            //Advance
            curr_intersection = next_intersection;
        }

        //
        //We should be at a depot
        //
        if (!is_depot(depots, curr_intersection)) {
            //Not at a valid end intersection
            //            std::cerr << "Invalid courier path: Last path segment did not end at a depot (ended at intersection " << curr_intersection << ")\n";
            return false;
        }

        //
        //Check everything was delivered
        //
        if (!delivered_all_packages(deliveries, deliveries_picked_up, deliveries_dropped_off)) {
            return false;
        }

    }

    //Everything validated
    return true;

}

bool valid_courier_problem(const std::vector<DeliveryInfo>& deliveries_vec,
        const std::vector<unsigned>& depots_vec) {
    if (deliveries_vec.empty()) {
        //        std::cerr << "Invalid courier problem: Must have at-least one delivery" << "\n";
        return false;
    }

    if (depots_vec.empty()) {
        //        std::cerr << "Invalid courier problem: Must have at-least one depot" << "\n";
        return false;
    }

    // Check for duplicates amoung:
    //   * Pick-Ups and depots
    //   * Drop-Offs and depots

    std::vector<unsigned> depots = depots_vec; //Copy since we need to sort the depots

    std::vector<unsigned> delivery_pick_ups;
    std::vector<unsigned> delivery_drop_offs;

    for (const DeliveryInfo& info : deliveries_vec) {
        delivery_pick_ups.push_back(info.pickUp);
        delivery_drop_offs.push_back(info.dropOff);
    }

    //Sort all the ids so we can quickly find the set intersections
    std::sort(delivery_pick_ups.begin(), delivery_pick_ups.end());
    std::sort(delivery_drop_offs.begin(), delivery_drop_offs.end());
    std::sort(depots.begin(), depots.end());

    //Verify that there are is not commonality between:
    //  * pick_ups and depots
    //  * drop_offs and depots
    //
    // Note: we allow duplicates between pick_ups and drop_offs
    std::vector<unsigned> common_pick_up_depots;
    std::vector<unsigned> common_drop_off_depots;

    //Common between pick-ups and depots
    std::set_intersection(delivery_pick_ups.begin(), delivery_pick_ups.end(),
            depots.begin(), depots.end(),
            std::back_inserter(common_pick_up_depots));

    //Common between drop-offs and depots
    std::set_intersection(delivery_drop_offs.begin(), delivery_drop_offs.end(),
            depots.begin(), depots.end(),
            std::back_inserter(common_drop_off_depots));

    if (!common_pick_up_depots.empty()) {
        return false;
    }

    if (!common_drop_off_depots.empty()) {
        return false;
    }

    //
    //Sanity check on id bounds
    //
    //Since vectors are sorted, can just query first and last elements
    assert(*(depots.begin()) >= 0);
    assert(*(--depots.end()) <= getNumberOfIntersections());
    assert(*(delivery_pick_ups.begin()) >= 0);
    assert(*(--delivery_pick_ups.end()) <= getNumberOfIntersections());
    assert(*(delivery_drop_offs.begin()) >= 0);
    assert(*(--delivery_drop_offs.end()) <= getNumberOfIntersections());

    return true;
}

bool determine_start_intersection(const std::vector<unsigned>& path,
        const std::vector<unsigned>& depots,
        unsigned& start_intersection) {
    assert(path.size() > 0);

    StreetSegmentInfo first_seg = getStreetSegmentInfo(path[0]);

    //Look for the from and to in the depot list
    auto to_iter = std::find(depots.begin(), depots.end(), first_seg.to);
    auto from_iter = std::find(depots.begin(), depots.end(), first_seg.from);

    //Initilize the current intersection
    if (to_iter != depots.end() && from_iter == depots.end()) {

        //To is exclusively a depot
        start_intersection = *to_iter;

    } else if (to_iter == depots.end() && from_iter != depots.end()) {

        //From is exclusivley a depot
        start_intersection = *from_iter;

    } else if (to_iter != depots.end() && from_iter != depots.end()) {
        //Both to and from are depots.
        //
        //This is the unlikely case where two depots are adjacent, and the student
        //returns the single segment joining them as the first element of the path.
        if (path.size() == 1) {
            //If we have a path consisting of only a single segment
            //we only need to take care when the segment is a one-way (i.e.
            //we should pick the start intersection to not violate the one-way)
            //
            //If it is not a one-way, we can pick an arbitrary start intersection.
            //
            //As a result we always pick the 'from' intersection as the start, as this
            //is gaurnteed to be correct for a one-way segment
            start_intersection = *from_iter;

        } else {
            //Multiple segments
            //
            //We can determine the start intersection by identify the common 
            //intersection between the two segments.
            assert(path.size() > 1);

            StreetSegmentInfo second_seg = getStreetSegmentInfo(path[1]);

            if (first_seg.to == second_seg.to || first_seg.to == second_seg.from) {
                //first_seg to connects to the second seg via it's 'to' intersection
                start_intersection = first_seg.from;
            } else {
                assert(first_seg.from == second_seg.to || first_seg.from == second_seg.from);

                //first_seg to connects to the second seg via it's 'from' intersection
                start_intersection = first_seg.to;
            }
        }

    } else {
        assert(to_iter == depots.end() && from_iter == depots.end());

        //Neither 'to' nor 'from' are depots
        //        std::cerr << "Invalid courier path: first path segment not connected to a depot\n";
        return false;
    }
    return true;
}

bool is_depot(const std::vector<unsigned>& depots,
        const unsigned intersection_id) {

    auto iter = std::find(depots.begin(), depots.end(), intersection_id);

    return (iter != depots.end());
}

bool traverse_segment(const std::vector<unsigned>& path,
        const unsigned path_idx,
        const unsigned curr_intersection,
        unsigned& next_intersection) {
    assert(path_idx < path.size());

    StreetSegmentInfo seg_info = getStreetSegmentInfo(path[path_idx]);

    //
    //Are we moving forward or back along the segment?
    //
    bool seg_traverse_forward;
    if (seg_info.from == curr_intersection) {
        //We take care to check 'from' first. This ensures
        //we get a reasonable traversal direction even in the 
        //case of a self-looping one-way segment

        //Moving forwards
        seg_traverse_forward = true;

    } else if (seg_info.to == curr_intersection) {
        //Moving backwards
        seg_traverse_forward = false;

    } else {
        assert(seg_info.from != curr_intersection && seg_info.to != curr_intersection);

        //This segment isn't connected to the current intersection
        //        std::cerr << "Invalid courier path: path disconnected"
        //                  << " (intersection " << curr_intersection 
        //                  << " does not connect to street segment " << path[path_idx] 
        //                  << " at path index " << path_idx << ")\n";
        return false;
    }

    //
    //Are we going the wrong way along the segment?
    //
    if (!seg_traverse_forward && seg_info.oneWay) {
        //        std::cerr << "Invalid courier path: Attempting to traverse against one-way"
        //                  << " (from intersection " << curr_intersection 
        //                  << " along street segment " << path[path_idx] 
        //                  << " at path index " << path_idx << ")\n";
        return false;
    }

    //
    //Advance to the next intersection
    //
    next_intersection = (seg_traverse_forward) ? seg_info.to : seg_info.from;

    return true;
}

void pick_up_at_intersection(const std::vector<DeliveryInfo>& deliveries,
        const std::multimap<unsigned, size_t>& intersections_to_pick_up,
        const unsigned curr_intersection,
        std::vector<bool>& deliveries_picked_up) {
    //
    //Check if we are picking up packages
    //

    //Find all the deliveries picking-up from this intersection
    auto range_pair = intersections_to_pick_up.equal_range(curr_intersection);

    //Mark each delivery as picked-up
    for (auto key_value_iter = range_pair.first; key_value_iter != range_pair.second; ++key_value_iter) {
        size_t delivery_idx = key_value_iter->second;

        deliveries_picked_up[delivery_idx] = true;

#ifdef DEBUG_PICK_UP_DROP_OFF
        //        std::cerr << "Info: Picked-up package for delivery " << delivery_idx
        //                  << " at intersection " << curr_intersection << "\n";
#endif

    }
}

void drop_off_at_intersection(const std::vector<DeliveryInfo>& deliveries,
        const std::multimap<unsigned, size_t>& intersections_to_drop_off,
        const std::vector<bool>& deliveries_picked_up,
        const unsigned curr_intersection,
        std::vector<bool>& deliveries_dropped_off) {
    //
    //Check if we are dropping-off packages
    //

    //Find all the deliveries dropping-off to this intersection
    auto range_pair = intersections_to_drop_off.equal_range(curr_intersection);

    //Mark each delivery dropped-off
    for (auto key_value_iter = range_pair.first; key_value_iter != range_pair.second; ++key_value_iter) {
        size_t delivery_idx = key_value_iter->second;

        if (deliveries_picked_up[delivery_idx]) {
            //Can only drop-off if the delivery was already picked-up
            deliveries_dropped_off[delivery_idx] = true;

#ifdef DEBUG_PICK_UP_DROP_OFF
            //            std::cerr << "Info: Dropped-off package for delivery " << delivery_idx
            //                      << " at intersection " << curr_intersection << "\n";
            //        } else {
            //            std::cerr << "Info: Did not drop-off package for delivery " << delivery_idx
            //                      << " at intersection " << curr_intersection 
            //                      << " since it has not been picked-up" << "\n";
#endif

        }
    }
}

bool delivered_all_packages(const std::vector<DeliveryInfo>& deliveries,
        const std::vector<bool>& deliveries_picked_up,
        const std::vector<bool>& deliveries_dropped_off) {

    //
    //Check how many undelivered packages there are
    //
    size_t undelivered_packages = 0;
    for (size_t delivery_idx = 0; delivery_idx < deliveries.size(); ++delivery_idx) {
        if (!deliveries_dropped_off[delivery_idx]) {
            ++undelivered_packages;
        } else {
            //If it was dropped-off it must have been picked-up
            assert(deliveries_picked_up[delivery_idx]);
        }
    }

    //
    //Report to the user the missing packages
    //
    if (undelivered_packages > 0) {
        //        std::cerr << "Invalid courier path: " << undelivered_packages << " packages were never delivered:\n";

        for (size_t delivery_idx = 0; delivery_idx < deliveries.size(); ++delivery_idx) {
            //Un-picked up
            if (!deliveries_picked_up[delivery_idx]) {
                //                std::cerr << "\tDelivery " << delivery_idx
                //                          << " was never picked-up at intersection " << deliveries[delivery_idx].pickUp
                //                          << "\n";
            }

            //Un-delivered up
            if (!deliveries_dropped_off[delivery_idx]) {
                //                std::cerr << "\tDelivery " << delivery_idx
                //                          << " was never dropped-off at intersection " << deliveries[delivery_idx].dropOff
                //                          << "\n";
            }
        }

        //Some un-delivered
        return false;
    }

    //All delivered
    return true;
}

bool path_legal(const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots,
        const std::vector<unsigned>& pathint) {

    //    unordered_multimap<unsigned,unsigned> dropoff;
    set<unsigned> dropoff_;
    unordered_multimap<unsigned, unsigned> pickup_to_dropoff_ = pickup_to_dropoff;
    //    bool is_legal = true;
    if (!valid_courier_problem(deliveries, depots)) {
        return false;
    }

    if (pathint.empty()) {
        //If it is a valid path it must contain at-least one segment
        //(since we gaurentee there is at-least one depot and delivery)
        //        std::cerr << "Invalid courier path: path was empty\n";

        return false;

    } else {
        for (unsigned i = 1; i < pathint.size() - 1; i++) {

            if (pickup_to_dropoff_.find(pathint[i]) != pickup_to_dropoff_.end() && dropoff_to_pickup.find(pathint[i]) == dropoff_to_pickup.end()) {
                auto range = pickup_to_dropoff_.equal_range(pathint[i]);
                for (auto it = range.first; it != range.second;) {

                    //                    dropoff.insert(pair<unsigned,unsigned>(it->second,it->first));
                    dropoff_.insert(it->second);
                    auto iter = it;
                    it++;
                    pickup_to_dropoff_.erase(iter);
                }
            } else if (pickup_to_dropoff_.find(pathint[i]) == pickup_to_dropoff_.end() && dropoff_to_pickup.find(pathint[i]) != dropoff_to_pickup.end()) {
                if (dropoff_.find(pathint[i]) == dropoff_.end()) {
                    return false;

                } else {
                    dropoff_.erase(pathint[i]);
                }


            } else if (pickup_to_dropoff_.find(pathint[i]) != pickup_to_dropoff_.end() && dropoff_to_pickup.find(pathint[i]) != dropoff_to_pickup.end()) {
                auto range = pickup_to_dropoff_.equal_range(pathint[i]);
                for (auto it = range.first; it != range.second;) {

                    //                    dropoff.insert(pair<unsigned,unsigned>(it->second,it->first));
                    dropoff_.insert(it->second);
                    auto iter = it;
                    it++;
                    pickup_to_dropoff_.erase(iter);
                }
                if (dropoff_.find(pathint[i]) == dropoff_.end()) {
                    return false;

                } else {
                    dropoff_.erase(pathint[i]);
                }


            }


        }










    }
    return true;
}

bool if_in_vec(vector<unsigned>& vec, unsigned ele) {
    for (auto it = vec.begin(); it != vec.end(); it++) {
        if (ele == *it) {
            return true;

        }


    }
    return false;

}

bool if_in_set(set<unsigned> set1, unsigned ele) {
    for (auto it = set1.begin(); it != set1.end(); it++) {
        if (ele == *it) {

            return true;
        }


    }
    return false;


}