#include <set>

#include "m3.h"

LatLon goal;

std::vector<unsigned> find_path_between_intersections(unsigned intersect_id_start, unsigned intersect_id_end) {

    unsigned intnum = getNumberOfIntersections();
    vector<unsigned> path;
    vector<unsigned> cameFrom(intnum, UINT_MAX);
    vector<unsigned> cameFrom_seg(intnum, UINT_MAX);
    map<double, unsigned> openset;
    priority_queue<double> q;
    vector<double> g_score(intnum, UINT_MAX);
    vector<unsigned> closedset(intnum, 0);

    openset.insert(pair<double, unsigned>(0, intersect_id_start));
    q.push(0);
    g_score[intersect_id_start] = 0;
    cameFrom[intersect_id_start] = intersect_id_start;

    unsigned current = intersect_id_start;
    double g_score_;
    double f_score_;
    unsigned nextid;

    while (openset.size() != 0) {
        current = openset.begin()->second;
        openset.erase(openset.begin());
        q.pop();
        closedset[current] = 1;

        if (current == intersect_id_end)
            break;

        for (unsigned i = 0; i < intersection_to_segments1[current].size(); i++) {
            unsigned segmentid = intersection_to_segments1[current][i];
            StreetSegmentInfo segment = getStreetSegmentInfo(segmentid);

            if (segment.from == current) {
                nextid = segment.to;
            } else if (!segment.oneWay) {
                nextid = segment.from;
            } else {
                continue;
            }

            if (closedset[nextid] == 1)
                continue;

            if (current == intersect_id_start) {
                g_score_ = find_street_segment_travel_time(segmentid);
            } else {
                unsigned lastsegid = cameFrom_seg[current];
                StreetSegmentInfo lastseg = getStreetSegmentInfo(lastsegid);
                if (lastseg.streetID != segment.streetID) {
                    g_score_ = find_street_segment_travel_time(segmentid) + 0.25;
                } else {
                    g_score_ = find_street_segment_travel_time(segmentid);
                }

            }
            f_score_ = g_score[current] + g_score_;

            if (g_score[nextid] == UINT_MAX) {
                g_score[nextid] = f_score_;
                cameFrom_seg[nextid] = segmentid;
                cameFrom[nextid] = current;

            } else if (g_score[nextid] > f_score_) {
                g_score[nextid] = f_score_;
                cameFrom_seg[nextid] = segmentid;
                cameFrom[nextid] = current;

            }
            //cout<<f_score_<<"               "<<heuristic_estimate_of_time(nextid, intersect_id_end)<<endl;
            double time = f_score_ + heuristic_estimate_of_time(nextid, intersect_id_end);
            
            
            openset.insert(pair<double, unsigned>(time, nextid));
            q.push(-time);

        }
    }

    if (current != intersect_id_end) {
        return path;
    }
    unsigned current_ = current;
    while (current_ != intersect_id_start) {

        path.insert(path.begin(), cameFrom_seg[current_]);
        current_ = cameFrom[current_];
    }

    return path;



}

double compute_path_travel_time(const std::vector < unsigned >& path) {
    double time = 0;
    for (unsigned i = 0; i < path.size(); i++) {
        if (i != 0 && (getStreetSegmentInfo(path[i])).streetID != (getStreetSegmentInfo(path[i - 1])).streetID) {
            time += 0.25;
        }
        time += find_street_segment_travel_time(path[i]);
    }



    return time;
}

std::vector < unsigned > find_path_to_point_of_interest(unsigned intersect_id_start, std::string point_of_interest_name) {
    vector<unsigned> path;
    unsigned intnum = getNumberOfIntersections();
    vector<unsigned> cameFrom(intnum, UINT_MAX);
    vector<unsigned> cameFrom_seg(intnum, UINT_MAX);
    map<double, unsigned> openset;
    priority_queue<double> q;
    vector<double> g_score(intnum, UINT_MAX);
    vector<unsigned> closedset(intnum, 0);
    vector<double> poid;
    vector<LatLon> poi_position;
    unordered_map<unsigned, LatLon> closedint;
    LatLon start = getIntersectionPosition(intersect_id_start);
    double d;
    double d_;
    auto range = poiname_to_id.equal_range(point_of_interest_name);
    for (auto it = range.first; it != range.second; it++) {
        poi_position.push_back(poiid_to_position[it->second]);
        d = find_distance_between_two_points(start, poiid_to_position[it->second]);
        if (it == range.first)
            d_ = d;
        else if (d < d_)
            d_ = d;
        poid.push_back(d);
    }

    for (unsigned i = 0; i < poid.size(); i++) {
        if (poid.size() < 5) {
            closedint.insert(pair<unsigned, LatLon>(find_closest_intersection(poi_position[i]), poi_position[i]));
        } else if (poid[i]<(d_ + 4500)) {
            closedint.insert(pair<unsigned, LatLon>(find_closest_intersection(poi_position[i]), poi_position[i]));
        }
    }

    if (poi_position.size() == 0) {
        return path;
    } else {
        openset.insert(pair<double, unsigned>(0, intersect_id_start));
        g_score[intersect_id_start] = 0;
        cameFrom[intersect_id_start] = intersect_id_start;

        unsigned current = intersect_id_start;
        double g_score_;
        double f_score_;
        unsigned nextid;
        
        while (openset.size() != 0) {
        current = openset.begin()->second;
        openset.erase(openset.begin());
        closedset[current] = 1;
        
        if(closedint.find(current)!=closedint.end()){
            goal = closedint.at(current);
            while(current != intersect_id_start){
                path.insert(path.begin(),cameFrom_seg[current]);
                current = cameFrom[current];
            }
            return path;
        }else{

        for (unsigned i = 0; i < intersection_to_segments1[current].size(); i++) {
            unsigned segmentid = intersection_to_segments1[current][i];
            StreetSegmentInfo segment = getStreetSegmentInfo(segmentid);

            if (segment.from == current) {
                nextid = segment.to;
            } else if (!segment.oneWay) {
                nextid = segment.from;
            } else {
                continue;
            }

            if (closedset[nextid] == 1)
                continue;

            if (current == intersect_id_start) {
                g_score_ = find_street_segment_travel_time(segmentid);
            } else {
                unsigned lastsegid = cameFrom_seg[current];
                StreetSegmentInfo lastseg = getStreetSegmentInfo(lastsegid);
                if (lastseg.streetID != segment.streetID) {
                    g_score_ = find_street_segment_travel_time(segmentid) + 0.25;
                } else {
                    g_score_ = find_street_segment_travel_time(segmentid);
                }

            }
            f_score_ = g_score[current] + g_score_;

            if (g_score[nextid] == UINT_MAX) {
                g_score[nextid] = f_score_;
                cameFrom_seg[nextid] = segmentid;
                cameFrom[nextid] = current;

            } else if (g_score[nextid] > f_score_) {
                g_score[nextid] = f_score_;
                cameFrom_seg[nextid] = segmentid;
                cameFrom[nextid] = current;

            }


            openset.insert(pair<double, unsigned>(f_score_, nextid));

        }
        }
    }

    }

    return path;



}

double heuristic_estimate_of_time(unsigned int1, unsigned int2) {
    double distance = find_distance_between_two_points(getIntersectionPosition(int1), getIntersectionPosition(int2));
   //   cout<<distance/1000<<endl;
    return distance/1800;


}

vector<unsigned> reconstruct_path(unordered_map<unsigned, unsigned> cameFrom, unordered_map<unsigned, unsigned>cameFrom_seg, unsigned current, unsigned from) {
    vector<unsigned> total;
    unsigned current_ = current;
    while (current_ != from) {

        total.push_back(cameFrom_seg[current_]);
        current_ = cameFrom[current_];
    }
    reverse(total.begin(), total.end());

    return total;


}
