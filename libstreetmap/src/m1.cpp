#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "m1.h"
//load the map
//the index of the map is streetid and the elements are the vectors of intersections on each street
std::unordered_map<unsigned, std::vector<unsigned>> streetid_to_intersections;
//the index of the map is street name and teh elemetns are the vector of street ids of each same name
std::unordered_map<std::string, std::vector<unsigned>> streetname_to_ids;
//this is a pointer points to a vector array which will initialize later in load map
//the array index is each intersection and the element of each index is the segment ids related to this intersection
std::vector<unsigned> *intersection_to_segments1;
//the index of the map is streetid and the elements are the vectors of all the segments on this street
std::unordered_map<unsigned, std::vector<unsigned>> street_id_to_segments;
//the index of the map is intersections and the elements are the vectors of all streetname related to this intersection
std::unordered_map<unsigned, std::vector<std::string>> intersection_to_streetname;

unordered_multimap<string, unsigned> poiname_to_id;

vector<LatLon> poiid_to_position;

vector<bool> dropoffints;

vector<bool> pickupints;
//vector<unsigned> closedset;
//unordered_map<double, unsigned> openset1;
//set<unsigned> openset;
//unordered_map<unsigned, unsigned> cameFrom;
//unordered_map<unsigned, unsigned> cameFrom_seg;
//unordered_map<unsigned, double> g_score;
//unordered_map<unsigned, double> h_score;
//unordered_map<unsigned, double> f_score;


std::unordered_map<unsigned long long, const OSMWay*> OSMid_to_ways;

std::vector<unsigned> motorWay;
std::vector<unsigned> trunk;
std::vector<unsigned> primary;
std::vector<unsigned> secondary;
std::vector<unsigned> tertiary;
std::vector<unsigned> unclassified;
std::vector<unsigned> residential;
std::vector<unsigned> service;
std::vector<unsigned> motorway_link;
std::vector<unsigned> trunk_link;
std::vector<unsigned> primary_link;
std::vector<t_point> allints;
std::vector<t_point> interests;
std::map<double, unsigned> area_to_closed_features;
double maxlat, minlat;
t_bound_box initial_coords;
string bigname;

bool load_map(std::string map_name) {
    //create data structure of load map

    cout << map_name << endl;
    bool load_success = loadStreetsDatabaseBIN(map_name);

    auto it = map_name.find("streets");
    map_name.erase(it);
    bigname = map_name.substr(26);
    if (bigname[0] == '/') {
        bigname = bigname.substr(1);
    }
    auto iter = bigname.find(".");
    bigname.erase(iter);
    boost::to_upper(bigname);
    string osm = map_name + "osm.bin";
    cout << osm << endl;
    loadOSMDatabaseBIN(osm);
    //initialize the intersection_to_segment array
    intersection_to_segments1 = new std::vector<unsigned>[getNumberOfIntersections()];
    double maxlon, minlon;
    maxlat = getNodeByIndex(0)->coords().lat;
    minlat = getNodeByIndex(0)->coords().lat;
    maxlon = getNodeByIndex(0)->coords().lon;
    minlon = getNodeByIndex(0)->coords().lon;



    for (unsigned long long i = 0; i < getNumberOfWays(); i++) {
        OSMid_to_ways.insert(std::pair<unsigned long long, const OSMWay*>(getWayByIndex(i)->id(), getWayByIndex(i)));
    }
    for (unsigned long long i = 0; i < getNumberOfNodes(); i++) {
        if (getNodeByIndex(i)->coords().lat < minlat)
            minlat = getNodeByIndex(i)->coords().lat;
        if (getNodeByIndex(i)->coords().lat > maxlat)
            maxlat = getNodeByIndex(i)->coords().lat;
        if (getNodeByIndex(i)->coords().lon < minlon)
            minlon = getNodeByIndex(i)->coords().lon;
        if (getNodeByIndex(i)->coords().lon > maxlon)
            maxlon = getNodeByIndex(i)->coords().lon;

    }
    LatLon min(minlat, minlon);
    LatLon max(maxlat, maxlon);
    initial_coords = t_bound_box(lat_to_point(min).x, lat_to_point(min).y, lat_to_point(max).x, lat_to_point(max).y);


    for (unsigned i = 0; i < getNumberOfStreetSegments(); i++) {
        //fulfill the streetid_to_intersection map
        if (streetid_to_intersections.find(getStreetSegmentInfo(i).streetID) != streetid_to_intersections.end()) {
            streetid_to_intersections.find(getStreetSegmentInfo(i).streetID)->second.push_back(getStreetSegmentInfo(i).from);
            streetid_to_intersections.find(getStreetSegmentInfo(i).streetID)->second.push_back(getStreetSegmentInfo(i).to);
        } else {
            unsigned intersections[] = {getStreetSegmentInfo(i).from, getStreetSegmentInfo(i).to};
            std::vector<unsigned> vec(intersections, intersections + 2);
            streetid_to_intersections.insert(std::pair<unsigned, std::vector<unsigned>>(getStreetSegmentInfo(i).streetID, vec));
        }

        //fulfill the intersection_to_segments array
        intersection_to_segments1[getStreetSegmentInfo(i).from].push_back(i);
        intersection_to_segments1[getStreetSegmentInfo(i).to].push_back(i);


        //fulfill the street_id_to_segments map
        if (street_id_to_segments.find(getStreetSegmentInfo(i).streetID) != street_id_to_segments.end()) {
            street_id_to_segments.find(getStreetSegmentInfo(i).streetID)->second.push_back(i);

        } else {
            unsigned streetids[] = {i};
            std::vector<unsigned> vec(streetids, streetids + 1);
            street_id_to_segments.insert(std::pair<unsigned, std::vector<unsigned>>(getStreetSegmentInfo(i).streetID, vec));
        }

        //fulfill the intersection_to_streetname map
        if (intersection_to_streetname.find(getStreetSegmentInfo(i).from) != intersection_to_streetname.end()) {
            intersection_to_streetname.find(getStreetSegmentInfo(i).from)->second.push_back(getStreetName(getStreetSegmentInfo(i).streetID));
        } else {
            std::vector<std::string> vec;
            vec.push_back(getStreetName(getStreetSegmentInfo(i).streetID));
            intersection_to_streetname.insert(std::pair<unsigned, std::vector < std::string >> (getStreetSegmentInfo(i).from, vec));
        }

        if (intersection_to_streetname.find(getStreetSegmentInfo(i).to) != intersection_to_streetname.end()) {
            intersection_to_streetname.find(getStreetSegmentInfo(i).to)->second.push_back(getStreetName(getStreetSegmentInfo(i).streetID));
        } else {
            std::vector<std::string> vec;
            vec.push_back(getStreetName(getStreetSegmentInfo(i).streetID));
            intersection_to_streetname.insert(std::pair<unsigned, std::vector < std::string >> (getStreetSegmentInfo(i).to, vec));
        }

        StreetSegmentInfo segment = getStreetSegmentInfo(i);
        const OSMWay* way = OSMid_to_ways[segment.wayOSMID];
        unsigned tagcount = getTagCount(way);

        for (unsigned j = 0; j < tagcount; j++) {
            pair<std::string, std::string> tagpair = getTagPair(way, j);
            if (tagpair.first == "highway") {
                if (tagpair.second == "motorway") {
                    motorWay.push_back(i);
                    break;
                }
                if (tagpair.second == "motorway_link") {
                    motorway_link.push_back(i);
                    break;
                }
                if (tagpair.second == "trunk") {
                    trunk.push_back(i);
                    break;
                }
                if (tagpair.second == "trunk_link") {
                    trunk_link.push_back(i);
                    break;
                }
                if (tagpair.second == "primary") {
                    primary.push_back(i);
                    break;
                }
                if (tagpair.second == "primary_link") {
                    primary_link.push_back(i);
                    break;
                }
                if (tagpair.second == "secondary" || tagpair.second == "secondary_link") {
                    secondary.push_back(i);
                    break;
                }
                if (tagpair.second == "tertiary" || tagpair.second == "tertiary_link") {
                    tertiary.push_back(i);
                    break;
                }
                if (tagpair.second == "unclassified") {
                    unclassified.push_back(i);
                    break;
                }
                if (tagpair.second == "residential") {
                    unclassified.push_back(i);
                    break;
                }
                if (tagpair.second == "service") {
                    unclassified.push_back(i);
                    break;
                } else {
                    tertiary.push_back(i);

                    break;
                }
            }

        }

    }

    //sort and delete the same elements of each vectors in streetid_to_intersections
    for (auto it = streetid_to_intersections.begin(); it != streetid_to_intersections.end(); it++) {
        sortvector(it->second);
    }

    //fulfill the streetname_to_ids map
    for (unsigned i = 0; i < getNumberOfStreets(); i++) {
        if (streetname_to_ids.find(getStreetName(i)) != streetname_to_ids.end()) {
            streetname_to_ids.find(getStreetName(i))->second.push_back(i);
        } else {
            std::vector<unsigned>vec3;
            vec3.push_back(i);
            streetname_to_ids.insert(std::pair<std::string, std::vector<unsigned>>(getStreetName(i), vec3));
        }
    }
    for (unsigned i = 0; i < getNumberOfIntersections(); i++) {
        allints.push_back(lat_to_point(getIntersectionPosition(i)));
        dropoffints.push_back(0);
        pickupints.push_back(0);
    }
    //    for(unsigned long long i = 0;i<getNumberOfNodes();i++){
    //        OSMid_to_entity.insert(std::pair<unsigned long long,const OSMEntity*>(getNodeByIndex(i)->id(),getNodeByIndex(i)));
    //    }

    //    for(unsigned long long i = 0;i<getNumberOfRelations();i++){
    //        OSMid_to_entity.insert(std::pair<unsigned long long,const OSMEntity*>(getRelationByIndex(i)->id(),getRelationByIndex(i)));
    //    }

    for (unsigned i = 0; i < getNumberOfPointsOfInterest(); i++) {
        interests.push_back(lat_to_point(getPointOfInterestPosition(i)));
        poiname_to_id.insert(pair<string, unsigned>(getPointOfInterestName(i), i));
        poiid_to_position.push_back(getPointOfInterestPosition(i));
    }

    for (unsigned i = 0; i < getNumberOfFeatures(); i++) {

        if (getFeatureType(i) != Shoreline && getFeatureType(i) != River && getFeatureType(i) != Stream) {
            vector<t_point> points;
            for (unsigned k = 0; k < getFeaturePointCount(i); k++) {
                points.push_back(lat_to_point(getFeaturePoint(i, k)));

            }

            area_to_closed_features.insert(pair<double, unsigned>(area(points), i));

        }
    }






    // create any data structures here to speed up your API functions
    return load_success;
}

//close the map
// destroy/clear all data structures created in load_map

void close_map() {
    closeStreetDatabase();
    closeOSMDatabase();
    streetid_to_intersections.clear();
    streetname_to_ids.clear();
    street_id_to_segments.clear();
    for (unsigned i = 1; i < getNumberOfIntersections(); i++) {
        intersection_to_segments1[i].clear();
    }
    delete[] intersection_to_segments1;
    intersection_to_segments1 = NULL;
    intersection_to_streetname.clear();

    OSMid_to_ways.clear();

    motorWay.clear();
    trunk.clear();
    primary.clear();
    secondary.clear();
    tertiary.clear();
    unclassified.clear();
    residential.clear();
    service.clear();
    motorway_link.clear();
    trunk_link.clear();
    primary_link.clear();
    allints.clear();
    interests.clear();
    area_to_closed_features.clear();

}

//function to return street id(s) for a street name
//return a 0-length vector if no street with this name exists.

std::vector<unsigned> find_street_ids_from_name(std::string street_name) {

    return streetname_to_ids[street_name];
}

std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id) {

    return intersection_to_segments1[intersection_id];
}

std::vector<std::string> find_intersection_street_names(unsigned intersection_id) {

    return intersection_to_streetname[intersection_id];
}

//using iterator to check if two intersections are directly connected
//conner cases happened when streets are oneway, at this condition, we need to check the direction of the street

bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2) {
    std::vector<unsigned> streetSG1 = find_intersection_street_segments(intersection_id1);
    std::vector<unsigned> streetSG2 = find_intersection_street_segments(intersection_id2);
    StreetSegmentInfo theStreetInfo;
    //use for loop to check direction of each street segment
    for (std::vector<unsigned>::iterator it1 = streetSG1.begin(); it1 != streetSG1.end(); it1++) {
        for (std::vector<unsigned>::iterator it2 = streetSG2.begin(); it2 != streetSG2.end(); it2++) {
            if (*it1 == *it2) {
                theStreetInfo = getStreetSegmentInfo(*it1);
                if ((theStreetInfo.oneWay)&&(intersection_id1 == theStreetInfo.from))
                    return true;
                else if (!theStreetInfo.oneWay)
                    return true;
            }
        }
    }
    return false;
}

//find adjacent intersections, first use intersection id to find the number of street segment 
//that this intersection has, and check each of the direction(if oneway)

std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id) {
    std::vector<unsigned> adjacent_intersections;
    unsigned segCout;
    unsigned street_seg;
    StreetSegmentInfo streetInfo;
    segCout = getIntersectionStreetSegmentCount(intersection_id);
    //use for loop to check every street segment
    for (unsigned i = 0; i < segCout; i++) {
        street_seg = getIntersectionStreetSegment(intersection_id, i);
        streetInfo = getStreetSegmentInfo(street_seg);
        if (intersection_id == streetInfo.from && streetInfo.oneWay) {
            adjacent_intersections.push_back(streetInfo.to);
        } else if (intersection_id == streetInfo.from && !streetInfo.oneWay) {
            adjacent_intersections.push_back(streetInfo.to);
        } else if (intersection_id == streetInfo.to && !streetInfo.oneWay) {
            adjacent_intersections.push_back(streetInfo.from);
        }
    }
    //after got all elements, sort them and erase the repeats
    sort(adjacent_intersections.begin(), adjacent_intersections.end());
    adjacent_intersections.erase(unique(adjacent_intersections.begin(), adjacent_intersections.end()), adjacent_intersections.end());
    return adjacent_intersections;
}

//use structure created in load map to get the street segment from street id

std::vector<unsigned> find_street_street_segments(unsigned street_id) {

    return street_id_to_segments[street_id];
}

//use structure created in load map to get the street intersection

std::vector<unsigned> find_all_street_intersections(unsigned street_id) {

    return streetid_to_intersections[street_id];

}

//find street names from intersections, use iterator 

std::vector<unsigned> find_intersection_ids_from_street_names(std::string street_name1, std::string street_name2) {
    std::vector<unsigned> intersectionIDs;
    //first get the ids from one name
    std::vector<unsigned>::iterator iter1, iter2;
    std::vector<unsigned> street_id1, street_id2;
    street_id1 = find_street_ids_from_name(street_name1);
    street_id2 = find_street_ids_from_name(street_name2);
    //for loop to check if have same id, the same id defines a street segment
    for (iter1 = street_id1.begin(); iter1 != street_id1.end(); iter1++) {
        for (iter2 = street_id2.begin(); iter2 != street_id2.end(); iter2++) {
            std::vector<unsigned> k1 = find_same_eles_in_vectors(find_all_street_intersections(*iter1), find_all_street_intersections(*iter2));
            intersectionIDs.insert(intersectionIDs.begin(), k1.begin(), k1.end());
        }
    }
    return intersectionIDs;
}

double find_distance_between_two_points(LatLon point1, LatLon point2) {
    double cosine = cos((double) (point1.lat + point2.lat) / 2 * DEG_TO_RAD);
    double d = sqrt(pow((point2.lat - point1.lat) * DEG_TO_RAD, 2) + pow((point2.lon - point1.lon) * DEG_TO_RAD * cosine, 2)) * EARTH_RADIUS_IN_METERS;
    return d;
}

double find_street_segment_length(unsigned street_segment_id) {
    StreetSegmentInfo seg1 = getStreetSegmentInfo(street_segment_id);
    double length = 0;
    //if there is no curve point on this segment
    //then directly calculate the distance between two intersections
    if (seg1.curvePointCount == 0) {
        length = find_distance_between_two_points(getIntersectionPosition(seg1.from), getIntersectionPosition(seg1.to));
        return length;
    }
    //if there exist curve points on this segment
    //add up sectional length of each small segments(from intersection1 to #of curve point, and so on) 
    for (unsigned i = 0; i <= seg1.curvePointCount; i++) {
        if (i == 0) {
            length += find_distance_between_two_points(getIntersectionPosition(seg1.from), getStreetSegmentCurvePoint(street_segment_id, i));
        } else if (i == seg1.curvePointCount) {
            length += find_distance_between_two_points(getIntersectionPosition(seg1.to), getStreetSegmentCurvePoint(street_segment_id, i - 1));
        } else {
            length += find_distance_between_two_points(getStreetSegmentCurvePoint(street_segment_id, i - 1), getStreetSegmentCurvePoint(street_segment_id, i));
        }
    }
    return length;
}

double find_street_length(unsigned street_id) {
    double total = 0;
    std::vector<unsigned> segs = find_street_street_segments(street_id);
    std::vector<unsigned>::iterator it;
    //add up length of all the segments on this street 
    for (it = segs.begin(); it != segs.end(); it++) {
        total += find_street_segment_length(*it);
    }
    return total;
}

double find_street_segment_travel_time(unsigned street_segment_id) {
    //calculating by time = street length / speed
    StreetSegmentInfo seg1 = getStreetSegmentInfo(street_segment_id);
    double length = find_street_segment_length(street_segment_id);
    return length / seg1.speedLimit * 60 / 1000;
}

unsigned find_closest_point_of_interest(LatLon my_position) {
    unsigned point = 0;
    double d = find_distance_between_two_points(my_position, getPointOfInterestPosition(0)) + 1;
    //find the minimum distance between all interest points and my position
    for (unsigned n = 0; n < getNumberOfPointsOfInterest(); n++) {
        LatLon point1 = getPointOfInterestPosition(n);
        double a = find_distance_between_two_points(my_position, point1);
        if (a < d) {
            d = a;
            point = n;
        }
    }
    return point;
}

unsigned find_closest_intersection(LatLon my_position) {
    unsigned intersection = 0;
    LatLon intersection2 = getIntersectionPosition(0);
    double d2 = pow((intersection2.lat - my_position.lat), 2) + pow((intersection2.lon - my_position.lon), 2) + 1;
    double cosine = cos(my_position.lat * DEG_TO_RAD);
    double x2 = my_position.lon * cosine;
    //calculate the distance between every intersection points and the the given point
    //compare all distances and find the minimum one, then return the id of this intersection
    for (unsigned n = 0; n < getNumberOfIntersections(); n++) {
        LatLon intersection1 = getIntersectionPosition(n);
        double x1 = intersection1.lon * cosine;
        double a2 = pow((intersection1.lat - my_position.lat), 2) + pow((x2 - x1), 2);
        if (a2 < d2) {
            d2 = a2;
            intersection = n;
        }
    }
    return intersection;
}
//used to test the output

void print_vector(std::vector<unsigned> a) {
    std::vector<unsigned>::iterator it;
    for (it = a.begin(); it != a.end(); it++) {
        std::cout << *it << ", ";
    }
    std::cout << std::endl;
}
//used to test the output

void print_vector_string(std::vector<std::string> a) {
    std::vector<std::string>::iterator it;
    for (it = a.begin(); it != a.end(); it++) {
        std::cout << *it << ", ";
    }
}
//function that used to delete the duplicated element in a vector

void sortvector(std::vector<unsigned>& a) {
    sort(a.begin(), a.end());
    a.erase(unique(a.begin(), a.end()), a.end());
    return;
}

//function that used to find the same element from two different vector

std::vector<unsigned> find_same_eles_in_vectors(std::vector<unsigned> a, std::vector<unsigned> b) {
    std::vector<unsigned> ret;
    //read each element respectively from two vectors and see whether they are equal
    //if yes, insert the same element into a new vector
    for (std::vector<unsigned>::iterator it1 = a.begin(); it1 != a.end(); it1++) {
        for (std::vector<unsigned>::iterator it2 = b.begin(); it2 != b.end(); it2++) {
            if (*it1 == *it2)
                ret.push_back(*it1);
        }
    }
    return ret;
}

/*std::vector<t_point> convert(std::vector<LatLon> oldpoints) {
    unsigned l = oldpoints.size();
    std::vector<t_point> afterconv;
    for (unsigned i = 0; i < l; i++) {
        t_point newstru;
        double cosine = cos((double) oldpoints[i].lat / 2 * DEG_TO_RAD);
        newstru.x = oldpoints[i].lon * DEG_TO_RAD*cosine;
        newstru.y = oldpoints[i].lat;
        afterconv.push_back(newstru);
    }
    return afterconv;
}*/



t_point lat_to_point(LatLon latlon) {
    t_point ret;
    double cosine = cos((double) (maxlat + minlat) / 2 * DEG_TO_RAD);
    ret.x = latlon.lon * cosine *EARTH_RADIUS_IN_METERS;
    ret.y = latlon.lat*EARTH_RADIUS_IN_METERS;
    return ret;


}

void drawpair(pair<std::string, std::string> kk) {
    cout << kk.first << "     " << kk.second << endl;
    return;

}

double det(double x1, double y1, double x2, double y2) {
    return x1 * y2 - x2*y1;
}

double area(vector<t_point> points) {
    unsigned i;
    double temp = 0;
    for (i = 0; i < points.size() - 1; i++) {
        temp += det(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
    }
    temp += det(points[i].x, points[i].y, points[0].x, points[0].y);
    return fabs(temp / 2);
}
