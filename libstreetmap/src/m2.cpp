#include "m2.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <cstdlib>
#include "m3.h"
#include <vector>

//declare external variables here
extern std::vector<t_point> allints;
extern std::unordered_map<unsigned long long, const OSMWay*> OSMid_to_ways;
extern std::vector<unsigned> motorWay;
extern std::vector<unsigned> trunk;
extern std::vector<unsigned> primary;
extern std::vector<unsigned> secondary;
extern std::vector<unsigned> tertiary;
extern std::vector<unsigned> unclassified;
extern std::vector<unsigned> residential;
extern std::vector<unsigned> service;
extern std::vector<unsigned> motorway_link;
extern std::vector<unsigned> trunk_link;
extern std::vector<unsigned> primary_link;
extern std::vector<OSMWay*> subway;
extern std::vector<unsigned> *intersection_to_segments1;
extern std::vector<t_point> interests;
extern std::map<double, unsigned> area_to_closed_features;
std::vector<unsigned> start_and_end_intersection;
extern string bigname;
extern t_bound_box initial_coords;
//declare variables here

float standardRad;
float zoomFactor;
bool if_gas = 0;
vector<unsigned> print_intersections;
bool print_int = 0;
bool if_int = 0;
bool if_interest = 0;
bool if_name = 0;
bool if_arrow = 0;
bool if_parking = 0;
bool if_wash = 0;
bool if_share = 0;
bool if_rental = 0;
bool if_findPath = 0;
bool if_printPathDirection = 0;
//call draw map

//declare street width variables
int motorWayWidth = 0;
int trunkWidth = 0; //also width of trunk link
int primaryWidth = 0; //also width of primary link
int secondaryWidth = 0;
int tertiaryWidth = 0;
int unclassifiedWidth = 0;
int residentialWidth = 0;
int serviceWidth = 0;

void draw_map() {
    string topname = bigname + " Map";
    init_graphics(topname, LIGHTGREY);

    //this "if" deal with the zooming issue of saint island
    if (bigname == "SAINT_HELENA") {
        double xmax = allints[0].x;
        double ymax = allints[0].y;
        double xmin = allints[0].x;
        double ymin = allints[0].y;
        for (unsigned i = 0; i < allints.size(); i++) {
            if (allints[i].x > xmax)
                xmax = allints[i].x;
            if (allints[i].x < xmin)
                xmin = allints[i].x;
            if (allints[i].y > ymax)
                ymax = allints[i].y;
            if (allints[i].y < ymin)
                ymin = allints[i].y;
        }

        initial_coords = t_bound_box(xmin - 400000, ymin - 400000, xmax + 400000, ymax + 400000);
    }
    set_visible_world(initial_coords);

    //create function buttons
    create_button("Window", "Find", act_on_new_button_find);
    destroy_button("Window");
    create_button("Find", "Gas Station", act_on_new_button_gas);
    create_button("Gas Station", "Parking", act_on_new_button_parking);
    create_button("Parking", "Car Washing", act_on_new_button_wash);
    create_button("Car Washing", "Car Sharing", act_on_new_button_sharing);
    create_button("Car Sharing", "Car Rental", act_on_new_button_rental);
    create_button("Car Rental", "Intersection", act_on_new_button_int);
    create_button("Intersection", "Interests", act_on_new_button_interest);
    create_button("Interests", "Names", act_on_new_button_name);
    create_button("Names", "Find path", act_on_new_button_findPath);
    create_button("Find path", "Direction", act_on_new_button_travelDirection);
    create_button("Direction", "Help", act_on_new_button_help);
    update_message("Total view, click proceed to change map.");
    event_loop(act_on_button_press, NULL, NULL, drawscreen);

    close_graphics();
}

//draw streets on the screen there

void drawscreen() {
    set_draw_mode(DRAW_NORMAL); // Should set this if your program does any XOR drawing in callbacks.
    clearscreen();
    standardRad = 1;
    t_bound_box now_coords = get_visible_world();
    zoomFactor = initial_coords.get_width() / now_coords.get_width();

    //different maps has different zooming factors, so, we separeting each factor
    if (bigname == "TORONTO") {
        motorWayWidth = floor(0.3 * zoomFactor);
        trunkWidth = floor(0.3 * zoomFactor); //also width of trunk link
        primaryWidth = floor(0.21 * zoomFactor); //also width of primary link
        secondaryWidth = floor(0.21 * zoomFactor);
        tertiaryWidth = floor(0.21 * zoomFactor);
        unclassifiedWidth = floor(0.18 * zoomFactor);
        residentialWidth = floor(0.18 * zoomFactor);
        serviceWidth = floor(0.16 * zoomFactor);
    }
    if (bigname == "NEWYORK") {
        motorWayWidth = floor(0.33 * zoomFactor);
        trunkWidth = floor(0.33 * zoomFactor); //also width of trunk link
        primaryWidth = floor(0.225 * zoomFactor); //also width of primary link
        secondaryWidth = floor(0.225 * zoomFactor);
        tertiaryWidth = floor(0.225 * zoomFactor);
        unclassifiedWidth = floor(0.2 * zoomFactor);
        residentialWidth = floor(0.2 * zoomFactor);
        serviceWidth = floor(0.16 * zoomFactor);
    }
    if (bigname == "MOSCOW") {
        motorWayWidth = floor(0.5 * zoomFactor);
        trunkWidth = floor(0.5 * zoomFactor); //also width of trunk link
        primaryWidth = floor(0.39 * zoomFactor); //also width of primary link
        secondaryWidth = floor(0.39 * zoomFactor);
        tertiaryWidth = floor(0.39 * zoomFactor);
        unclassifiedWidth = floor(0.32 * zoomFactor);
        residentialWidth = floor(0.32 * zoomFactor);
        serviceWidth = floor(0.25 * zoomFactor);
    }
    if (bigname == "HAMILTON_CANADA") {
        motorWayWidth = floor(0.6 * zoomFactor);
        trunkWidth = floor(0.6 * zoomFactor); //also width of trunk link
        primaryWidth = floor(0.42 * zoomFactor); //also width of primary link
        secondaryWidth = floor(0.42 * zoomFactor);
        tertiaryWidth = floor(0.42 * zoomFactor);
        unclassifiedWidth = floor(0.36 * zoomFactor);
        residentialWidth = floor(0.36 * zoomFactor);
        serviceWidth = floor(0.32 * zoomFactor);
    }
    if (bigname == "SAINT_HELENA") {
        motorWayWidth = floor(1.1 * zoomFactor);
        trunkWidth = floor(1.1 * zoomFactor); //also width of trunk link
        primaryWidth = floor(0.7 * zoomFactor); //also width of primary link
        secondaryWidth = floor(0.7 * zoomFactor);
        tertiaryWidth = floor(0.7 * zoomFactor);
        unclassifiedWidth = floor(0.52 * zoomFactor);
        residentialWidth = floor(0.52 * zoomFactor);
        serviceWidth = floor(0.4 * zoomFactor);
    }
    if (bigname == "TORONTO_CANADA") {
        motorWayWidth = floor(0.175 * zoomFactor);
        trunkWidth = floor(0.175 * zoomFactor); //also width of trunk link
        primaryWidth = floor(0.115 * zoomFactor); //also width of primary link
        secondaryWidth = floor(0.115 * zoomFactor);
        tertiaryWidth = floor(0.115 * zoomFactor);
        unclassifiedWidth = floor(0.1 * zoomFactor);
        residentialWidth = floor(0.1 * zoomFactor);
        serviceWidth = floor(0.08 * zoomFactor);
    }
    if (bigname == "CAIRO_EGYPT") {
        motorWayWidth = floor(0.24 * zoomFactor);
        trunkWidth = floor(0.24 * zoomFactor); //also width of trunk link
        primaryWidth = floor(0.17 * zoomFactor); //also width of primary link
        secondaryWidth = floor(0.17 * zoomFactor);
        tertiaryWidth = floor(0.17 * zoomFactor);
        unclassifiedWidth = floor(0.145 * zoomFactor);
        residentialWidth = floor(0.145 * zoomFactor);
        serviceWidth = floor(0.1 * zoomFactor);
    }


    std::map<double, unsigned>::iterator it = area_to_closed_features.end();
    it--;

    //draw all features
    for (it; it != area_to_closed_features.begin(); it--) {
        draw_feature(it->second);
    }
    draw_feature(it->second);


    //draw rivers in the end
    for (unsigned i = 0; i < getNumberOfFeatures(); i++) {

        draw_river(i);
    }

    //then draw names
    if (if_name) {
        for (unsigned i = 0; i < getNumberOfFeatures(); i++) {
            draw_feature_name(i);
        }
    }

    //those for loop used to draw every streets
    setlinewidth(serviceWidth);
    for (unsigned it = 0; it < service.size(); it++) {
        if (zoomFactor > 21) {
            draw_service(service[it], WHITE);
        }
    }

    setlinewidth(residentialWidth);
    for (unsigned it = 0; it < residential.size(); it++) {
        if (zoomFactor > 18 && bigname != "SAINT_HELENA") {
            draw_residential(residential[it], WHITE);

        }
        if (bigname == "SAINT_HELENA") {
            if (zoomFactor >= 0 && zoomFactor < 15) {
                setlinewidth(sqrt(5 * (residentialWidth + 1)));
                draw_residential(residential[it], WHITE);
            }
            if (zoomFactor >= 15) {
                setlinewidth(residentialWidth);
                draw_residential(residential[it], WHITE);
            }
        }
    }

    setlinewidth(unclassifiedWidth);
    for (unsigned it = 0; it < unclassified.size(); it++) {
        if (zoomFactor > 15 && bigname != "SAINT_HELENA") {
            draw_unclassified(unclassified[it], WHITE);
        }
        if (bigname == "SAINT_HELENA") {
            if (zoomFactor >= 0 && zoomFactor < 15) {
                setlinewidth(sqrt(5 * (unclassifiedWidth + 1)));
                draw_unclassified(unclassified[it], WHITE);
            }
            if (zoomFactor >= 15) {
                setlinewidth(unclassifiedWidth);
                draw_unclassified(unclassified[it], WHITE);
            }
        }
    }


    setlinewidth(primaryWidth);
    for (unsigned it = 0; it < primary.size(); it++) {

        if (zoomFactor >= 0 && zoomFactor < 15) {
            setlinewidth(sqrt(5 * (primaryWidth + 1)));
            draw_primary(primary[it], ORANGEYELLOW);
        }
        if (zoomFactor >= 15) {
            setlinewidth(primaryWidth);
            draw_primary(primary[it], ORANGEYELLOW);
        }
    }

    setlinewidth(primaryWidth);
    for (unsigned it = 0; it < primary_link.size(); it++) {

        if (zoomFactor >= 15) {
            draw_primary(primary_link[it], ORANGEYELLOW);
        }
    }

    setlinewidth(secondaryWidth);
    for (unsigned it = 0; it < secondary.size(); it++) {
        if (zoomFactor >= 0 && zoomFactor < 15) {
            setlinewidth(sqrt(5 * (primaryWidth + 1)));
            draw_primary(secondary[it], YELLOWORANGE);
        }
        if (zoomFactor >= 15) {
            setlinewidth(primaryWidth);
            draw_primary(secondary[it], YELLOWORANGE);
        }
    }

    setlinewidth(tertiaryWidth);
    for (unsigned it = 0; it < tertiary.size(); it++) {
        if (zoomFactor > 12 && bigname != "SAINT_HELENA") {
            draw_primary(tertiary[it], WHITE);

        }
        if (bigname == "SAINT_HELENA") {
            if (zoomFactor >= 0 && zoomFactor < 15) {
                setlinewidth(sqrt(5 * (tertiaryWidth + 1)));
                draw_primary(tertiary[it], WHITE);
            }
            if (zoomFactor >= 15) {
                setlinewidth(unclassifiedWidth);
                draw_primary(tertiary[it], WHITE);
            }
        }
    }

    setlinewidth(secondaryWidth);
    for (unsigned it = 0; it < motorway_link.size(); it++) {
        if (zoomFactor >= 15) {
            draw_primary(motorway_link[it], RED);
        }
    }

    for (unsigned it = 0; it < motorWay.size(); it++) {
        if (zoomFactor >= 0 && zoomFactor < 15) {
            setlinewidth(sqrt(20 * (motorWayWidth + 1)));

            draw_motorway(motorWay[it], RED);
        }
        if (zoomFactor >= 15) {
            setlinewidth(motorWayWidth);
            draw_motorway(motorWay[it], RED);
        }
    }


    for (unsigned it = 0; it < trunk.size(); it++) {
        if (zoomFactor >= 0 && zoomFactor < 15) {
            setlinewidth(sqrt(20 * (trunkWidth + 1)));
            draw_motorway(trunk[it], ORANGE);
        }
        if (zoomFactor >= 15) {
            setlinewidth(trunkWidth);
            draw_motorway(trunk[it], ORANGE);
        }
    }

    setlinewidth(trunkWidth);
    for (unsigned it = 0; it < trunk_link.size(); it++) {
        if (zoomFactor > 2) {
            draw_motorway(trunk_link[it], ORANGE);
        }
    }


    //draw names of streets
    if (if_name) {
        for (unsigned it = 0; it < motorWay.size(); it++) {
            if (zoomFactor > 2) {
                if (if_arrow)
                    draw_arrow(motorWay[it]);
                setcolor(BLACK);
                draw_text(motorWay[it]);
            }
        }

        for (unsigned it = 0; it < trunk.size(); it++) {
            if (zoomFactor > 5) {
                if (if_arrow)
                    draw_arrow(trunk[it]);
                setcolor(BLACK);
                draw_text(trunk[it]);
            }
        }

        for (unsigned it = 0; it < primary.size(); it++) {

            if (zoomFactor >= 8) {
                if (if_arrow)
                    draw_arrow(primary[it]);
                setcolor(BLACK);
                draw_text(primary[it]);
            }
        }

        for (unsigned it = 0; it < secondary.size(); it++) {
            if (zoomFactor > 11) {
                if (if_arrow)
                    draw_arrow(secondary[it]);
                setcolor(BLACK);
                draw_text(secondary[it]);
            }
        }

        for (unsigned it = 0; it < tertiary.size(); it++) {
            if (zoomFactor > 14) {
                if (if_arrow)
                    draw_arrow(tertiary[it]);
                setcolor(BLACK);
                draw_text(tertiary[it]);
            }
        }

        setlinewidth(unclassifiedWidth);
        for (unsigned it = 0; it < unclassified.size(); it++) {
            if (zoomFactor > 17) {
                setcolor(BLACK);
                draw_text(unclassified[it]);
            }
        }

        setlinewidth(residentialWidth);
        for (unsigned it = 0; it < residential.size(); it++) {
            if (zoomFactor > 20) {
                if (if_arrow)
                    draw_arrow(residential[it]);
                setcolor(BLACK);
                draw_text(residential[it]);
            }
        }

        setlinewidth(serviceWidth);
        for (unsigned it = 0; it < service.size(); it++) {
            if (zoomFactor > 24) {
                if (zoomFactor > 20) {
                    if (if_arrow)
                        draw_arrow(service[it]);
                    setcolor(BLACK);
                    draw_text(service[it]);
                }
            }
        }


        for (unsigned it = 0; it < motorway_link.size(); it++) {
            if (zoomFactor > 24) {
                if (if_arrow)
                    draw_arrow(motorway_link[it]);
                setcolor(BLACK);
                draw_text(motorway_link[it]);
            }
        }

        for (unsigned it = 0; it < trunk_link.size(); it++) {
            if (zoomFactor > 24) {
                if (if_arrow)
                    draw_arrow(trunk_link[it]);
                setcolor(BLACK);
                draw_text(trunk_link[it]);
            }
        }

        for (unsigned it = 0; it < primary_link.size(); it++) {
            if (zoomFactor > 24) {
                if (if_arrow)
                    draw_arrow(primary_link[it]);
                setcolor(BLACK);
                draw_text(primary_link[it]);
            }
        }
    }
    //
    if (if_int) {
        if (zoomFactor > 33) {
            setcolor(DARKGREY);
            for (unsigned i = 0; i < getNumberOfIntersections(); i++) {
                fillarc(allints[i].x, allints[i].y, 150, 0, 360);
            }
        }
    }
    if (if_interest) {
        if (zoomFactor > 33) {
            for (unsigned i = 0; i < getNumberOfPointsOfInterest(); i++) {
                setcolor(CYAN);
                fillarc(interests[i].x, interests[i].y, 160, 0, 360);

            }
        }
    }


    if (if_gas) {
        draw_gas();
    }

    if (if_parking) {

        draw_parking();
    }

    if (if_wash) {
        draw_washing();
    }

    if (if_share) {
        draw_sharing();
    }

    if (if_rental) {
        draw_rental();
    }

    if (print_intersections.size() != 0 && print_int == 1) {
        for (unsigned i = 0; i < print_intersections.size(); i++) {
            t_point mid = lat_to_point(getIntersectionPosition(print_intersections[i]));
            cout << print_intersections[i] << endl;
            set_visible_world_1(t_bound_box(mid.x - 10000, mid.y - 10000, mid.x + 10000, mid.y + 10000));
            setcolor(BLUE);
            fillrect(mid.x, mid.y, mid.x + zoomFactor * 70, mid.y + zoomFactor * 30);
            setcolor(RED);
            fillarc(mid.x, mid.y, 150, 0, 360);
            settextrotation(0);
            setcolor(BLACK);
            setfontsize(10);
            string name = getIntersectionName(print_intersections[i]);
            stringstream ss;
            ss << intersection_to_segments1[print_intersections[i]].size() << " " << getIntersectionPosition(print_intersections[i]).lat << " " << getIntersectionPosition(print_intersections[i]).lon;
            string num, lat, lon;
            ss >> num >> lat>>lon;
            string numofsegments = "Number of Segments: " + num;
            string position = "Position: latitude: " + lat + " longitude: " + lon;
            drawtext(mid.x + zoomFactor * 35, mid.y + zoomFactor * 25, name, FLT_MAX, FLT_MAX);
            drawtext(mid.x + zoomFactor * 35, mid.y + zoomFactor * 15, numofsegments, FLT_MAX, FLT_MAX);
            drawtext(mid.x + zoomFactor * 35, mid.y + zoomFactor * 5, position, FLT_MAX, FLT_MAX);
        }

    }
    if (zoomFactor < 8)
        print_big_name();

    //draw the path between two intersections
    if (start_and_end_intersection.size() == 2) {
        vector<unsigned> path;
        //path = find_path_between_intersections(17930, 93400);
        path = find_path_between_intersections(start_and_end_intersection[0], start_and_end_intersection[1]);
        //print_vector(path);
        //cout << path.size() << endl;
        
        setlinewidth(10);
        setcolor(BLUE);
        for (unsigned i = 0; i < path.size(); i++) {
            cout << path[i] << endl;
            //drawline(lat_to_point(getIntersectionPosition(getStreetSegmentInfo(path[i]).from)), lat_to_point(getIntersectionPosition(getStreetSegmentInfo(path[i]).to)));
            draw_residential(path[i],BLUE);
        }
        if (if_printPathDirection){
            print_travel_direction(path);
        }
 //       start_and_end_intersection.clear();
    }

    return;
}

void draw_feature(unsigned featureID) {
    settextrotation(0);
    unsigned numofpts = getFeaturePointCount(featureID);
    t_point points[numofpts];

    string name = getFeatureName(featureID);
    if (name == "<noname>") {
        name = "";
    }
    for (unsigned i = 0; i < numofpts; i++) {
        points[i] = lat_to_point(getFeaturePoint(featureID, i));
    }
    if (getFeatureType(featureID) == Golfcourse || getFeatureType(featureID) == Greenspace) {

        setcolor(GRASSGREEN);
        fillpoly(points, numofpts);

    } else if (getFeatureType(featureID) == Beach) {

        setcolor(YELLOW);
        fillpoly(points, numofpts);

    } else if (getFeatureType(featureID) == Island) {
        setcolor(CREAM);
        fillpoly(points, numofpts);

    } else if (getFeatureType(featureID) == Park) {
        setcolor(GREEN);
        fillpoly(points, numofpts);

    } else if (getFeatureType(featureID) == Lake) {

        setcolor(LIGHTBLUE);
        fillpoly(points, numofpts);

    } else if (getFeatureType(featureID) == Shoreline) {

        setcolor(BLACK);
        setlinewidth(1);
        drawlineofset(points, numofpts);

    } else if (getFeatureType(featureID) == Building && zoomFactor > 21.4) {

        setcolor(DARKGREY);
        fillpoly(points, numofpts);
    }
}

//functions to draw rivers

void draw_river(unsigned featureID) {
    settextrotation(0);
    unsigned numofpts = getFeaturePointCount(featureID);
    t_point points[numofpts];

    string name = getFeatureName(featureID);
    if (name == "<noname>") {
        name = "";
    }
    for (unsigned i = 0; i < numofpts; i++) {
        points[i] = lat_to_point(getFeaturePoint(featureID, i));
    }
    if ((getFeatureType(featureID) == River || getFeatureType(featureID) == Stream) && zoomFactor > 2.4) {

        setcolor(BLUE);
        setlinewidth(3);
        drawlineofset(points, numofpts);
    }
}

void draw_feature_name(unsigned featureID) {
    settextrotation(0);
    setfontsize(10);
    unsigned numofpts = getFeaturePointCount(featureID);
    t_point points[numofpts];

    string name = getFeatureName(featureID);
    if (name == "<noname>") {
        name = "";
    }
    for (unsigned i = 0; i < numofpts; i++) {
        points[i] = lat_to_point(getFeaturePoint(featureID, i));
    }
    if (getFeatureType(featureID) == Park || getFeatureType(featureID) == Golfcourse || getFeatureType(featureID) == Greenspace) {
        t_point mid = find_close_feature_mid_point(points, numofpts);
        setcolor(BLACK);

        drawtext(mid.x, mid.y, name, 4600, 4600);
    } else if (getFeatureType(featureID) == Park) {
        setcolor(GREEN);
        fillpoly(points, numofpts);
        if (zoomFactor > 21) {
            t_point mid = find_close_feature_mid_point(points, numofpts);
            setcolor(BLACK);

            drawtext(mid.x, mid.y, name, 4600, 4600);
        }
    } else if (getFeatureType(featureID) == Beach) {
        t_point mid = find_close_feature_mid_point(points, numofpts);
        setcolor(BLACK);

        drawtext(mid.x, mid.y, name, 30000, 30000);
    } else if (getFeatureType(featureID) == Island) {

        t_point mid = find_close_feature_mid_point(points, numofpts);
        setcolor(BLACK);
        if (zoomFactor > 12) {
            drawtext(mid.x, mid.y, name, 30000, 30000);
        }
    } else if (getFeatureType(featureID) == Lake) {
        t_point mid = find_close_feature_mid_point(points, numofpts);
        setcolor(BLACK);
        drawtext(mid.x, mid.y, name, 30000, 30000);

    } else if (getFeatureType(featureID) == Shoreline) {
        if (zoomFactor > 12) {
            t_point mid = find_open_feature_mid_point(points, numofpts);
            setcolor(BLACK);

            drawtext(mid.x, mid.y, name, 3000, 3000);
        }
    } else if ((getFeatureType(featureID) == River || getFeatureType(featureID) == Stream) && (zoomFactor > 15.4)) {


        t_point mid = find_open_feature_mid_point(points, numofpts);
        setcolor(BLACK);
        if (featureID % 2 == 0) {
            if (zoomFactor > 21) {
                drawtext(mid.x, mid.y, name, 30000, 30000);
            }
        }
    } else if (getFeatureType(featureID) == Building && zoomFactor > 21.4) {


        t_point mid = find_close_feature_mid_point(points, numofpts);
        setcolor(BLACK);

        drawtext(mid.x, mid.y, name, 3000, 3000);
    }
}

void drawlineofset(t_point *pts, unsigned number) {
    for (unsigned i = 0; i < number - 1; i++) {
        drawline(pts[i], pts[i + 1]);
    }
}

t_point find_close_feature_mid_point(t_point* pts, unsigned number) {
    double xavg = 0, yavg = 0;
    t_point point;
    for (unsigned i = 0; i < number; i++) {
        xavg += pts[i].x;
        yavg += pts[i].y;
    }
    xavg = xavg / number;
    yavg = yavg / number;
    point.x = xavg;
    point.y = yavg;
    return point;
}

t_point find_open_feature_mid_point(t_point* pts, unsigned number) {
    int midindex = number / 2;
    return pts[midindex];
}

void draw_interest(unsigned interestID) {
    string name = getFeatureName(interestID);
    return;
}
//draw service street segments and set the circle radius

void draw_service(unsigned segmentid, const t_color& color) {
    setcolor(color);
    StreetSegmentInfo segment = getStreetSegmentInfo(segmentid);
    if (segment.curvePointCount == 0) {
        drawline(lat_to_point(getIntersectionPosition(segment.from)), lat_to_point(getIntersectionPosition(segment.to)));
        fillarc(lat_to_point(getIntersectionPosition(segment.to)), 70, 0, 360);
        fillarc(lat_to_point(getIntersectionPosition(segment.from)), 70, 0, 360);

    } else {
        for (unsigned k = 0; k <= segment.curvePointCount; k++) {
            if (k == 0) {
                drawline(lat_to_point(getIntersectionPosition(segment.from)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k)));
                fillarc(lat_to_point(getIntersectionPosition(segment.from)), 70, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k)), 70, 0, 360);
            } else if (k == segment.curvePointCount) {
                drawline(lat_to_point(getIntersectionPosition(segment.to)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)));
                fillarc(lat_to_point(getIntersectionPosition(segment.to)), 70, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), 70, 0, 360);
            } else {
                drawline(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k)));
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), 70, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k)), 70, 0, 360);
            }
        }
    }
}

//draw residential street segments and set the circle radius

void draw_residential(unsigned segmentid, const t_color& color) {
    setcolor(color);
    StreetSegmentInfo segment = getStreetSegmentInfo(segmentid);
    if (segment.curvePointCount == 0) {
        drawline(lat_to_point(getIntersectionPosition(segment.from)), lat_to_point(getIntersectionPosition(segment.to)));
        fillarc(lat_to_point(getIntersectionPosition(segment.to)), 130, 0, 360);
        fillarc(lat_to_point(getIntersectionPosition(segment.from)), 130, 0, 360);

    } else {
        for (unsigned k = 0; k <= segment.curvePointCount; k++) {
            if (k == 0) {
                drawline(lat_to_point(getIntersectionPosition(segment.from)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k)));
                fillarc(lat_to_point(getIntersectionPosition(segment.from)), 130, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k)), 130, 0, 360);
            } else if (k == segment.curvePointCount) {
                drawline(lat_to_point(getIntersectionPosition(segment.to)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)));
                fillarc(lat_to_point(getIntersectionPosition(segment.to)), 130, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), 130, 0, 360);
            } else {
                drawline(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k)));
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), 130, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k)), 130, 0, 360);
            }
        }
    }
}
//draw unclassified street segments and set the circle radius

void draw_unclassified(unsigned segmentid, const t_color& color) {
    setcolor(color);
    StreetSegmentInfo segment = getStreetSegmentInfo(segmentid);
    if (segment.curvePointCount == 0) {
        drawline(lat_to_point(getIntersectionPosition(segment.from)), lat_to_point(getIntersectionPosition(segment.to)));
        fillarc(lat_to_point(getIntersectionPosition(segment.to)), 280, 0, 360);
        fillarc(lat_to_point(getIntersectionPosition(segment.from)), 280, 0, 360);

    } else {
        for (unsigned k = 0; k <= segment.curvePointCount; k++) {
            if (k == 0) {
                drawline(lat_to_point(getIntersectionPosition(segment.from)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k)));
                fillarc(lat_to_point(getIntersectionPosition(segment.from)), 280, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k)), 280, 0, 360);
            } else if (k == segment.curvePointCount) {
                drawline(lat_to_point(getIntersectionPosition(segment.to)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)));
                fillarc(lat_to_point(getIntersectionPosition(segment.to)), 280, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), 280, 0, 360);
            } else {
                drawline(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k)));
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), 280, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k)), 280, 0, 360);
            }
        }
    }
}
//draw motorway street segments and set the circle radius

void draw_motorway(unsigned segmentid, const t_color& color) {
    setcolor(color);
    StreetSegmentInfo segment = getStreetSegmentInfo(segmentid);
    t_point segFrom = lat_to_point(getIntersectionPosition(segment.from));
    t_point segTo = lat_to_point(getIntersectionPosition(segment.to));
    if (segment.curvePointCount == 0) {
        drawline(segFrom, segTo);
        fillarc(segTo, 460, 0, 360);
        fillarc(segFrom, 460, 0, 360);

    } else {
        for (unsigned k = 0; k <= segment.curvePointCount; k++) {
            if (k == 0) {
                drawline(segFrom, lat_to_point(getStreetSegmentCurvePoint(segmentid, k)));
                fillarc(segFrom, 460, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k)), 460, 0, 360);
            } else if (k == segment.curvePointCount) {
                drawline(segTo, lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)));
                fillarc(segTo, 460, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), 460, 0, 360);
            } else {
                drawline(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k)));
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), 460, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k)), 460, 0, 360);
            }
        }
    }
}
//draw primary street segments and set the circle radius

void draw_primary(unsigned segmentid, const t_color& color) {
    setcolor(color);
    StreetSegmentInfo segment = getStreetSegmentInfo(segmentid);
    t_point segFrom = lat_to_point(getIntersectionPosition(segment.from));
    t_point segTo = lat_to_point(getIntersectionPosition(segment.to));
    if (segment.curvePointCount == 0) {
        drawline(segFrom, segTo);
        fillarc(segTo, 320, 0, 360);
        fillarc(segFrom, 320, 0, 360);

    } else {
        for (unsigned k = 0; k <= segment.curvePointCount; k++) {
            if (k == 0) {
                drawline(segFrom, lat_to_point(getStreetSegmentCurvePoint(segmentid, k)));
                fillarc(segFrom, 320, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k)), 320, 0, 360);
            } else if (k == segment.curvePointCount) {
                drawline(segTo, lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)));
                fillarc(segTo, 320, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), 320, 0, 360);
            } else {
                drawline(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), lat_to_point(getStreetSegmentCurvePoint(segmentid, k)));
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1)), 320, 0, 360);
                fillarc(lat_to_point(getStreetSegmentCurvePoint(segmentid, k)), 320, 0, 360);
            }
        }
    }
}

//functions to find the degree of line segments(which have no curve points)

double find_degree(unsigned segmentid) {
    double degree = 0;
    StreetSegmentInfo segment = getStreetSegmentInfo(segmentid);

    t_point p1 = lat_to_point(getIntersectionPosition(segment.from));
    t_point p2 = lat_to_point(getIntersectionPosition(segment.to));

    if (p1.x <= p2.x) {
        degree = atan((p2.y - p1.y) / (p2.x - p1.x)) / DEG_TO_RAD;
    } else if (p1.x > p2.x) {
        degree = (atan((p2.y - p1.y) / (p2.x - p1.x)) / DEG_TO_RAD) + 180;
    }
    return degree;
}

void draw_text(unsigned segmentid) {
    StreetSegmentInfo segment = getStreetSegmentInfo(segmentid);
    t_point segFrom = lat_to_point(getIntersectionPosition(segment.from));
    t_point segTo = lat_to_point(getIntersectionPosition(segment.to));
    if (getStreetName(segment.streetID) != "<unknown>") {

        if ((segment.curvePointCount == 0 || segment.curvePointCount < 2)) {
            double degree = find_degree(segmentid);
            if (fabs(degree) > 90) {
                degree -= 180;
            }
            settextrotation(degree);
            setfontsize(8);
            float bound_ = sqrt((segFrom.x - segTo.x)*(segFrom.x - segTo.x)+(segFrom.y - segTo.y)*(segFrom.y - segTo.y));
            if (segmentid % 2 == 0) {
                drawtext((segFrom.x + segTo.x) / 2, (segFrom.y + segTo.y) / 2, getStreetName(segment.streetID), bound_, bound_);
            }
        } else if (segment.curvePointCount >= 2) {
            int index = floor(segment.curvePointCount / 2);
            t_point point = lat_to_point(getStreetSegmentCurvePoint(segmentid, index));
            double degree = find_degree(segmentid);

            if (fabs(degree) > 90) {
                degree -= 180;
            }
            settextrotation(degree);
            setfontsize(8);
            float bound_ = sqrt((segFrom.x - segTo.x)*(segFrom.x - segTo.x)+(segFrom.y - segTo.y)*(segFrom.y - segTo.y));
            if ((zoomFactor > 59) && (segmentid % 2 == 0)) {
                drawtext(point, getStreetName(segment.streetID), bound_, bound_);
            }
        }
    }
}

void act_on_button_press(float x, float y, t_event_buttonPressed event) {
    /* Called whenever event_loop gets a button press in the graphics *
     * area.  Allows the user to do whatever he/she wants with button *
     * clicks.                                                        */
    drawscreen();
    if (if_int) {
        for (unsigned i = 0; i < getNumberOfIntersections(); i++) {
            settextrotation(0);
            if (distance_between_two_t_points(t_point(x, y), lat_to_point(getIntersectionPosition(i))) < 150) {
                setcolor(BLUE);
                if (zoomFactor < 45 && zoomFactor > 30) {

                    fillrect(x, y, x + zoomFactor * 800, y + zoomFactor * 300);
                    setcolor(RED);
                    fillarc(allints[i].x, allints[i].y, 150, 0, 360);
                    setcolor(BLACK);
                    setfontsize(10);
                    string name = getIntersectionName(i);
                    stringstream ss;
                    ss << intersection_to_segments1[i].size() << " " << getIntersectionPosition(i).lat << " " << getIntersectionPosition(i).lon;
                    string num, lat, lon;
                    ss >> num >> lat>>lon;
                    string numofsegments = "Number of Segments: " + num;
                    string position = "Position: latitude: " + lat + " longitude: " + lon;
                    drawtext(x + zoomFactor * 375, y + zoomFactor * 260, name, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 375, y + zoomFactor * 220, numofsegments, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 375, y + zoomFactor * 150, position, FLT_MAX, FLT_MAX);
                } else if (zoomFactor < 80 && zoomFactor > 45) {
                    fillrect(x, y, x + zoomFactor * 300, y + zoomFactor * 120);
                    setcolor(RED);
                    fillarc(allints[i].x, allints[i].y, 150, 0, 360);
                    setcolor(BLACK);
                    setfontsize(10);
                    string name = getIntersectionName(i);
                    stringstream ss;
                    ss << intersection_to_segments1[i].size() << " " << getIntersectionPosition(i).lat << " " << getIntersectionPosition(i).lon;
                    string num, lat, lon;
                    ss >> num >> lat>>lon;
                    string numofsegments = "Number of Segments: " + num;
                    string position = "Position: latitude: " + lat + " longitude: " + lon;
                    drawtext(x + zoomFactor * 150, y + zoomFactor * 100, name, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 150, y + zoomFactor * 80, numofsegments, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 150, y + zoomFactor * 60, position, FLT_MAX, FLT_MAX);
                } else if (zoomFactor < 130 && zoomFactor > 80) {
                    fillrect(x, y, x + zoomFactor * 130, y + zoomFactor * 45);
                    setcolor(RED);
                    fillarc(allints[i].x, allints[i].y, 150, 0, 360);
                    setcolor(BLACK);
                    setfontsize(10);
                    string name = getIntersectionName(i);
                    stringstream ss;
                    ss << intersection_to_segments1[i].size() << " " << getIntersectionPosition(i).lat << " " << getIntersectionPosition(i).lon;
                    string num, lat, lon;
                    ss >> num >> lat>>lon;
                    string numofsegments = "Number of Segments: " + num;
                    string position = "Position: latitude: " + lat + " longitude: " + lon;
                    drawtext(x + zoomFactor * 65, y + zoomFactor * 40, name, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 65, y + zoomFactor * 30, numofsegments, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 65, y + zoomFactor * 20, position, FLT_MAX, FLT_MAX);
                } else if (zoomFactor < 220 && zoomFactor > 130) {
                    fillrect(x, y, x + zoomFactor * 70, y + zoomFactor * 30);
                    setcolor(RED);
                    fillarc(allints[i].x, allints[i].y, 150, 0, 360);
                    setcolor(BLACK);
                    setfontsize(10);
                    string name = getIntersectionName(i);
                    stringstream ss;
                    ss << intersection_to_segments1[i].size() << " " << getIntersectionPosition(i).lat << " " << getIntersectionPosition(i).lon;
                    string num, lat, lon;
                    ss >> num >> lat>>lon;
                    string numofsegments = "Number of Segments: " + num;
                    string position = "Position: latitude: " + lat + " longitude: " + lon;
                    drawtext(x + zoomFactor * 35, y + zoomFactor * 25, name, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 35, y + zoomFactor * 15, numofsegments, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 35, y + zoomFactor * 5, position, FLT_MAX, FLT_MAX);
                } else if (zoomFactor < 280 && zoomFactor > 220) {
                    fillrect(x, y, x + zoomFactor * 30, y + zoomFactor * 10);
                    setcolor(RED);
                    fillarc(allints[i].x, allints[i].y, 150, 0, 360);
                    setcolor(BLACK);
                    setfontsize(10);
                    string name = getIntersectionName(i);
                    stringstream ss;
                    ss << intersection_to_segments1[i].size() << " " << getIntersectionPosition(i).lat << " " << getIntersectionPosition(i).lon;
                    string num, lat, lon;
                    ss >> num >> lat>>lon;
                    string numofsegments = "Number of Segments: " + num;
                    string position = "Position: latitude: " + lat + " longitude: " + lon;
                    drawtext(x + zoomFactor * 15, y + zoomFactor * 9, name, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 15, y + zoomFactor * 6, numofsegments, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 15, y + zoomFactor * 3, position, FLT_MAX, FLT_MAX);
                }
            }

        }

    }
    if (if_findPath && if_int) {
        for (unsigned i = 0; i < getNumberOfIntersections(); i++) {
            if (distance_between_two_t_points(t_point(x, y), lat_to_point(getIntersectionPosition(i))) < 150) {

                setcolor(RED);
                fillarc(allints[i].x, allints[i].y, 150, 0, 360);
                start_and_end_intersection.push_back(i);
                
            }
        }

    }


    if (if_interest) {
        for (unsigned i = 0; i < getNumberOfPointsOfInterest(); i++) {

            settextrotation(0);
            if (distance_between_two_t_points(t_point(x, y), lat_to_point(getPointOfInterestPosition(i))) < 150) {
                setcolor(BLUE);
                if (zoomFactor < 45 && zoomFactor > 30) {

                    fillrect(x, y, x + zoomFactor * 800, y + zoomFactor * 300);
                    setcolor(RED);
                    fillarc(interests[i].x, interests[i].y, 150, 0, 360);
                    setcolor(BLACK);
                    setfontsize(10);
                    string name = getPointOfInterestName(i);
                    string type = getPointOfInterestType(i);
                    stringstream ss;
                    ss << getPointOfInterestPosition(i).lat << " " << getPointOfInterestPosition(i).lon;
                    string lat, lon;
                    ss >> lat>>lon;
                    string typeofinterest = "Type of Interest: " + type;
                    string position = "Position: latitude: " + lat + " longitude: " + lon;
                    drawtext(x + zoomFactor * 375, y + zoomFactor * 260, name, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 375, y + zoomFactor * 220, typeofinterest, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 375, y + zoomFactor * 150, position, FLT_MAX, FLT_MAX);
                } else if (zoomFactor < 80 && zoomFactor > 45) {
                    fillrect(x, y, x + zoomFactor * 300, y + zoomFactor * 120);
                    setcolor(RED);
                    fillarc(interests[i].x, interests[i].y, 150, 0, 360);
                    setcolor(BLACK);
                    setfontsize(10);
                    string name = getPointOfInterestName(i);
                    string type = getPointOfInterestType(i);
                    stringstream ss;
                    ss << getPointOfInterestPosition(i).lat << " " << getPointOfInterestPosition(i).lon;
                    string lat, lon;
                    ss >> lat>>lon;
                    string typeofinterest = "Type of Interest: " + type;
                    string position = "Position: latitude: " + lat + " longitude: " + lon;
                    drawtext(x + zoomFactor * 150, y + zoomFactor * 100, name, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 150, y + zoomFactor * 80, typeofinterest, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 150, y + zoomFactor * 60, position, FLT_MAX, FLT_MAX);
                } else if (zoomFactor < 130 && zoomFactor > 80) {
                    fillrect(x, y, x + zoomFactor * 130, y + zoomFactor * 45);
                    setcolor(RED);
                    fillarc(interests[i].x, interests[i].y, 150, 0, 360);
                    setcolor(BLACK);
                    setfontsize(10);
                    string name = getPointOfInterestName(i);
                    string type = getPointOfInterestType(i);
                    stringstream ss;
                    ss << getPointOfInterestPosition(i).lat << " " << getPointOfInterestPosition(i).lon;
                    string lat, lon;
                    ss >> lat>>lon;
                    string typeofinterest = "Type of Interest: " + type;
                    string position = "Position: latitude: " + lat + " longitude: " + lon;
                    drawtext(x + zoomFactor * 65, y + zoomFactor * 40, name, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 65, y + zoomFactor * 30, typeofinterest, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 65, y + zoomFactor * 20, position, FLT_MAX, FLT_MAX);
                } else if (zoomFactor < 220 && zoomFactor > 130) {
                    fillrect(x, y, x + zoomFactor * 70, y + zoomFactor * 30);
                    setcolor(RED);
                    fillarc(interests[i].x, interests[i].y, 150, 0, 360);
                    setcolor(BLACK);
                    setfontsize(10);
                    string name = getPointOfInterestName(i);
                    string type = getPointOfInterestType(i);
                    stringstream ss;
                    ss << getPointOfInterestPosition(i).lat << " " << getPointOfInterestPosition(i).lon;
                    string lat, lon;
                    ss >> lat>>lon;
                    string typeofinterest = "Type of Interest: " + type;
                    string position = "Position: latitude: " + lat + " longitude: " + lon;
                    drawtext(x + zoomFactor * 35, y + zoomFactor * 25, name, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 35, y + zoomFactor * 15, typeofinterest, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 35, y + zoomFactor * 5, position, FLT_MAX, FLT_MAX);
                } else if (zoomFactor < 280 && zoomFactor > 220) {
                    fillrect(x, y, x + zoomFactor * 30, y + zoomFactor * 10);
                    setcolor(RED);
                    fillarc(interests[i].x, interests[i].y, 150, 0, 360);
                    setcolor(BLACK);
                    setfontsize(10);
                    string name = getPointOfInterestName(i);
                    string type = getPointOfInterestType(i);
                    stringstream ss;
                    ss << getPointOfInterestPosition(i).lat << " " << getPointOfInterestPosition(i).lon;
                    string lat, lon;
                    ss >> lat>>lon;
                    string typeofinterest = "Type of Interest: " + type;
                    string position = "Position: latitude: " + lat + " longitude: " + lon;
                    drawtext(x + zoomFactor * 15, y + zoomFactor * 9, name, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 15, y + zoomFactor * 6, typeofinterest, FLT_MAX, FLT_MAX);
                    drawtext(x + zoomFactor * 15, y + zoomFactor * 3, position, FLT_MAX, FLT_MAX);
                }
            }
        }
    }
}

double distance_between_two_t_points(t_point t1, t_point t2) {
    return (sqrt(pow((t1.y - t2.y), 2) + pow((t1.x - t2.x), 2)));
}

void draw_arrow(unsigned segmentid) {
    StreetSegmentInfo segment = getStreetSegmentInfo(segmentid);
    t_point segFrom = lat_to_point(getIntersectionPosition(segment.from));
    t_point segTo = lat_to_point(getIntersectionPosition(segment.to));
    if (segment.oneWay) {
        if (zoomFactor > 21) {
            if (segmentid % 2 != 0) {
                if (segment.curvePointCount == 0) {
                    setcolor(PURPLE);
                    //make the text displayed in different size as zooming in or out
                    if (zoomFactor > 100) {
                        setfontsize(20);
                    } else if (zoomFactor < 100) {
                        setfontsize(12);
                    }
                    settextrotation(find_degree(segmentid));
                    drawtext(segFrom.x + (segTo.x - segFrom.x) / 3, segFrom.y + (segTo.y - segFrom.y) / 3, "->", FLT_MAX, FLT_MAX);
                } else {
                    for (unsigned k = 0; k <= segment.curvePointCount; k++) {
                        //
                        if (k == 0) {
                            t_point kpoint = lat_to_point(getStreetSegmentCurvePoint(segmentid, k));
                            setcolor(PURPLE);
                            if (zoomFactor > 100) {
                                setfontsize(20);
                            } else if (zoomFactor < 100) {
                                setfontsize(12);
                            }
                            settextrotation(find_degree_(segFrom, kpoint));
                            if (zoomFactor > 59) {
                                drawtext(segFrom.x + (kpoint.x - segFrom.x) / 3, segFrom.x + (kpoint.y - segFrom.y) / 3, ">", FLT_MAX, FLT_MAX);
                            }
                            //
                        } else if (k == segment.curvePointCount) {
                            t_point k1point = lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1));
                            setcolor(PURPLE);
                            if (zoomFactor > 100) {
                                setfontsize(20);
                            } else if (zoomFactor < 100) {
                                setfontsize(12);
                            }
                            settextrotation(find_degree_(k1point, segTo));
                            if (zoomFactor > 59) {
                                drawtext(k1point.x + (segTo.x - k1point.x) / 3, k1point.y + (segTo.y - k1point.y) / 3, ">", FLT_MAX, FLT_MAX);
                            }
                        } else {
                            t_point kpoint = lat_to_point(getStreetSegmentCurvePoint(segmentid, k));
                            t_point k1point = lat_to_point(getStreetSegmentCurvePoint(segmentid, k - 1));
                            setcolor(PURPLE);
                            if (zoomFactor > 100) {
                                setfontsize(20);
                            } else if (zoomFactor < 100) {
                                setfontsize(12);
                            }
                            settextrotation(find_degree_(k1point, kpoint));
                            if ((zoomFactor > 59) && (k % 2 != 0)) {
                                drawtext(k1point.x + (kpoint.x - k1point.x) / 3, k1point.y + (kpoint.y - k1point.y) / 3, ">", FLT_MAX, FLT_MAX);
                            }
                        }
                    }
                }
            }
        }
    }
}

//functions to find the degree of line segments(which have curve points)

double find_degree_(t_point t1, t_point t2) {
    double degree = 0;
    if (t1.x <= t2.x) {
        degree = atan((t2.y - t1.y) / (t2.x - t1.x)) / DEG_TO_RAD;
    } else if (t1.x > t2.x) {
        degree = (atan((t2.y - t1.y) / (t2.x - t1.x)) / DEG_TO_RAD) + 180;
    }
    return degree;
}



//make reaction when users click on the "Find" button

void act_on_new_button_find(void (*drawscreen_ptr) (void)) {

    rl_bind_key('\t', rl_complete);
    //Use our function for auto-complete
    rl_attempted_completion_function = street_Name_command_completion;
    //Tell readline to handle double and single quotes for us
    rl_completer_quote_characters = strdup("\"");

    string name1;
    string name2;

    if (print_int == 0) {
        update_message("Please input street names in terminal.");
        std::cout << "Please enter two street\n";

        char* buf; //Buffer of line entered by user

        //========Read the first street name from user=========
        while ((buf = readline("Enter first street: ")) != NULL) { //Prompt the user for input

            if (strcmp(buf, "") != 0) //Only save non-empty commands
                add_history(buf);

            name1 = std::string(buf);

            if (name1.size() != 0) {
                //erase the first quote of input
                name1.erase(name1.begin());
                string::size_type found = name1.find("\"");
                if (found != string::npos)
                    //erase the last quote and spaces behind the quote
                    name1.erase(name1.begin() + found, name1.end());
                break;
            }
            free(buf);
            buf = NULL;
        }
        //If the buffer wasn't freed in the main loop we need to free it now
        free(buf);

        //==========Read the second street name from user===========
        while ((buf = readline("Enter second street: ")) != NULL) { //Prompt the user for input

            if (strcmp(buf, "") != 0) //Only save non-empty commands
                add_history(buf);

            name2 = std::string(buf);
            if (name2.size() != 0) {
                //erase the first quote of input
                name2.erase(name2.begin());
                string::size_type found = name2.find("\"");
                if (found != string::npos)
                    //erase the last quote and spaces behind the quote
                    name2.erase(name2.begin() + found, name2.end());
                break;
            }
            free(buf);
            buf = NULL;
        }
        //If the buffer wasn't freed in the main loop we need to free it now
        free(buf);


        std::vector<unsigned> intersectionids = find_intersection_ids_from_street_names(name1, name2);
        print_intersections = intersectionids;
        cout << "There are " << intersectionids.size() << " " << "intersections between " << name1 << " and " << name2 << " ." << endl;

        if (intersectionids.size() != 0) {
            update_message("Point and information shows on the map, click find again to quit.");
        } else
            //Print out error message
            update_message("There are no intersection between these two streets.");

        print_int = 1;
        drawscreen_ptr();
    } else {
        update_message("Quit to main map");
        print_int = 0;
        drawscreen_ptr();
    }

}

void act_on_new_button_gas(void (*drawscreen_ptr) (void)) {

    if (if_gas == 1) {
        if_gas = 0;
        update_message("Stop showing gas stations");
    } else {
        if_gas = 1;
        update_message("Show gas station points on the map, showing with BIG G.");
    }
    // Re-draw the screen (a few squares are changing colour with time)
    drawscreen_ptr();
}

void draw_gas() {
    for (unsigned i = 0; i < getNumberOfPointsOfInterest(); i++) {

        if (getPointOfInterestType(i) == "fuel") {
            settextrotation(0);

            if (zoomFactor > 35) {
                setfontsize(20);
            } else {
                setfontsize(10);
            }
            setcolor(RED);
            fillarc(interests[i].x, interests[i].y, sqrt(zoomFactor)*50, 0, 360);
            setcolor(BLACK);
            if (zoomFactor >= 1) {
                drawtext(interests[i].x, interests[i].y, "G", FLT_MAX, FLT_MAX);
            }
        }
    }
    return;
}

//make reaction when users click on the "Intersection" button

void act_on_new_button_int(void (*drawscreen_ptr) (void)) {

    if (if_int == 1) {
        if_int = 0;
        update_message("Stop showing intersections");
    } else {
        if_int = 1;
        update_message("Show intersections points on the map, showing with darkgrey color.");
    }
    drawscreen_ptr();
}
//make reaction when users click on the "Interest" button

void act_on_new_button_interest(void (*drawscreen_ptr) (void)) {

    if (if_interest == 1) {
        if_interest = 0;
        update_message("Stop showing interests");
    } else {
        if_interest = 1;
        update_message("Show interests points on the map, showing with cyan color.");
    }
    drawscreen_ptr();
}
//make reaction when users click on the "Name" button

void act_on_new_button_name(void (*drawscreen_ptr) (void)) {

    if (if_name == 1) {
        if_name = 0;
        update_message("Stop showing names");
        if (if_arrow) {
            destroy_button("Arrows");
        }

    } else {
        if_name = 1;
        update_message("Show names on the map.");

        create_button("Names", "Arrows", act_on_new_button_arrow);
    }
    drawscreen_ptr();
}

//make reaction when users click on the "Arrow" button

void act_on_new_button_arrow(void (*drawscreen_ptr) (void)) {

    if (if_arrow == 1) {
        if_arrow = 0;
        update_message("Stop showing arrows");
    } else {
        if_arrow = 1;
        update_message("Show arrows on the map to show the direction for each one-way street.");
    }

    drawscreen_ptr();
}
//make reaction when users click on the "Parking" button

void act_on_new_button_parking(void (*drawscreen_ptr) (void)) {

    if (if_parking == 1) {
        if_parking = 0;
        update_message("Stop showing parkings");
    } else {
        if_parking = 1;
        update_message("Show Parking points on the map, showing with BIG P.");
    }

    drawscreen_ptr();
}
//make reaction when users click on the "Car Wash" button

void act_on_new_button_wash(void (*drawscreen_ptr) (void)) {

    if (if_wash == 1) {
        if_wash = 0;
        update_message("Stop showing Car Washings.");
    } else {
        if_wash = 1;
        update_message("Show Car Washing points on the map, showing with BIG W.");
    }

    drawscreen_ptr();
}
//make reaction when users click on the "Car Sharing" button

void act_on_new_button_sharing(void (*drawscreen_ptr) (void)) {

    if (if_share == 1) {
        if_share = 0;
        update_message("Stop showing Car Sharings.");
    } else {
        if_share = 1;
        update_message("Show Car sharing points on the map, showing with BIG S.");
    }

    drawscreen_ptr();
}

//make reaction when users click on the "Car Renting" button

void act_on_new_button_rental(void (*drawscreen_ptr) (void)) {
    if (if_rental == 1) {
        if_rental = 0;
        update_message("Stop showing Car Rental.");
    } else {
        if_rental = 1;
        update_message("Show Car rental points on the map, showing with BIG R.");
    }
    drawscreen_ptr();
}

void act_on_new_button_findPath(void (*drawscreen_ptr) (void)) {
    if (if_findPath == 1) {
        if_findPath = 0;
        update_message("Stop showing path.");
        start_and_end_intersection.clear();
    } else {
        if_findPath = 1;
        update_message("Enable to show path between two intersections.");
    }
    drawscreen_ptr();

}

//To show the help information for users to understand how to use our map

void act_on_new_button_travelDirection(void (*drawscreen_ptr) (void)) {

    if (if_printPathDirection == 1) {
        if_printPathDirection = 0;
        update_message("Stop showing travel direction.");
    } else {
        if_printPathDirection = 1;
        update_message("Indicate the travel direction along the path.");

    }

    drawscreen_ptr();
}

void act_on_new_button_help(void (*drawscreen_ptr) (void)) {

    cout << "===========Information helps you to use the map============" << endl;
    cout << "Press \"Find\" button to find intersections of two streets." << endl;
    cout << "Press \"Intersection\" button to show all intersections points." << endl;
    cout << "Press \"Interest\" button to show all the interest points." << endl;
    cout << "Press \"Names\" button to show the names of all streets." << endl;
    cout << "Press \"Arrow\" button to show the driving direction of streets." << endl;
    cout << "****The following button helps to find interest points****" << endl;
    cout << "Press \"Gas Station\" button to find gas stations all over the city." << endl;
    cout << "Press \"Parking\" button to find parking places." << endl;
    cout << "Press \"Car Washing/Sharing/Rental\" button to find car service." << endl;
    cout << "Press \"Proceed\" button to display another map." << endl;
    cout << "===========================================================" << endl;
}

//find parking places

void draw_parking() {
    for (unsigned i = 0; i < getNumberOfPointsOfInterest(); i++) {

        if (getPointOfInterestType(i) == "parking") {
            settextrotation(0);
            if (zoomFactor > 35) {
                setfontsize(20);
            } else {
                setfontsize(10);
            }
            setcolor(WHITE);
            fillarc(interests[i].x, interests[i].y, sqrt(zoomFactor)*50, 0, 360);
            setcolor(DARKBLUE);
            drawtext(interests[i].x, interests[i].y, "P", FLT_MAX, FLT_MAX);
        }
    }
    return;
}


//functions to draw the car_washing interest points

void draw_washing() {
    for (unsigned i = 0; i < getNumberOfPointsOfInterest(); i++) {

        if (getPointOfInterestType(i) == "car_wash") {
            settextrotation(0);
            if (zoomFactor > 35) {
                setfontsize(20);
            } else {
                setfontsize(10);
            }
            setcolor(WHITE);
            fillarc(interests[i].x, interests[i].y, sqrt(zoomFactor)*50, 0, 360);
            setcolor(LIGHTBLUE);
            drawtext(interests[i].x, interests[i].y, "W", FLT_MAX, FLT_MAX);
        }
    }
    return;
}

//show the map name on the centor of the screen

void print_big_name() {
    settextrotation(0);
    setcolor(DARKDARKGRAY);
    setfontsize(80);
    drawtext((get_visible_world().left() + get_visible_world().right()) / 2, (get_visible_world().bottom() + get_visible_world().top()) / 2, bigname, FLT_MAX, FLT_MAX);
}

//functions to draw the car_sharing interest points

void draw_sharing() {
    for (unsigned i = 0; i < getNumberOfPointsOfInterest(); i++) {
        if (getPointOfInterestType(i) == "car_sharing") {
            settextrotation(0);
            if (zoomFactor > 35) {
                setfontsize(20);
            } else {
                setfontsize(10);
            }
            setcolor(RED);
            fillarc(interests[i].x, interests[i].y, sqrt(zoomFactor)*50, 0, 360);
            setcolor(PLUM);
            drawtext(interests[i].x, interests[i].y, "S", FLT_MAX, FLT_MAX);
        }
    }
    return;
}
//functions to find places to rent cars

void draw_rental() {
    for (unsigned i = 0; i < getNumberOfPointsOfInterest(); i++) {
        if (getPointOfInterestType(i) == "car_rental") {
            settextrotation(0);
            if (zoomFactor > 35) {
                setfontsize(20);
            } else {
                setfontsize(10);
            }
            setcolor(RED);
            fillarc(interests[i].x, interests[i].y, sqrt(zoomFactor)*50, 0, 360);
            setcolor(CORAL);
            drawtext(interests[i].x, interests[i].y, "R", FLT_MAX, FLT_MAX);
        }
    }
    return;
}

void print_travel_direction(std::vector<unsigned> path) {
    cout << "==================================================================================" << endl;
    cout << "Path has been found, and here are instrction help users to reach the destination: " << endl; 
    //
    for (unsigned i = 0; i < path.size(); i++) {
        //        StreetSegmentInfo segment = getStreetSegmentInfo(path[i]);

        double travelDistance = 0;
        //for the start point of the path
        if (i == 0) {
            travelDistance = find_street_segment_length(path[i]);
            cout << " Go straight " << travelDistance << " meters."  << endl;

        } else {
            //if the street segment does not have curve point
            if (getStreetSegmentInfo(path[i]).curvePointCount == 0) {

                //make the text displayed in different size as zooming in or out

                travelDistance = find_street_segment_length(path[i]);

                if ((find_degree(path[i]) + 90 - find_degree(path[i - 1])) > 0) {
                    cout << " Turn left, ";
                } else if ((find_degree(path[i]) + 90 - find_degree(path[i - 1])) < 0) {
                    cout << " Turn right, ";
                } else;
                
                cout << " Go straight " << travelDistance << " meters." << endl;
            }
                //segments have curve points
            else {
                for (unsigned k = 0; k <= getStreetSegmentInfo(path[i]).curvePointCount; k++) {
                    //first curve point
                    if (k == 0) {
                        StreetSegmentInfo current_seg = getStreetSegmentInfo(path[i]);
                        t_point kpoint = lat_to_point(getStreetSegmentCurvePoint(path[i], k));
                        t_point t_intersection = lat_to_point(getIntersectionPosition(current_seg.from));

                        if ((find_degree_(t_intersection, kpoint) + 90 - find_degree(path[i]-1)) > 0) {
                            cout << " Turn left, ";
                        } else if ((find_degree_(t_intersection, kpoint) + 90 - find_degree(path[i-1])) < 0) {
                            cout << " Turn right, ";
                        }
                        else;
                        travelDistance = find_distance_between_two_points(getIntersectionPosition(current_seg.from), getStreetSegmentCurvePoint(path[i], 0));
                        cout << " Go straight " << travelDistance << " meters."  << endl;
                        
                        //find_degree_(getStreetSegmentInfo(path[i]).from, kpoint)

                    }//curve points in the middle
                    else if (k == getStreetSegmentInfo(path[i]).curvePointCount) {
                        t_point pre_point = lat_to_point(getStreetSegmentCurvePoint(path[i], k - 1));
                        t_point k_point = lat_to_point(getStreetSegmentCurvePoint(path[i], k));
                        StreetSegmentInfo current_seg = getStreetSegmentInfo(path[i]);
                        t_point t_intersection = lat_to_point(getIntersectionPosition(current_seg.to));
                        
                        if ((find_degree_(pre_point, t_intersection) + 90 - find_degree_(pre_point, k_point)) > 0) {
                            cout << " Turn left, ";
                        } else if ((find_degree_(pre_point, t_intersection) + 90 - find_degree_(pre_point, k_point)) < 0) {
                            cout << " Turn right, ";
                        }
                        else;
                        travelDistance = find_distance_between_two_points(getStreetSegmentCurvePoint(path[i], k-1), getStreetSegmentCurvePoint(path[i], k));
                        cout << " Go straight " << travelDistance << " meters."  << endl;
                        //find_degree_(k1point, getStreetSegmentInfo(path[i]).to)

                    } else {

                        t_point pre_point = lat_to_point(getStreetSegmentCurvePoint(path[i], k));
                        t_point k_point = lat_to_point(getStreetSegmentCurvePoint(path[i], k - 1));
                        t_point pre2_point = lat_to_point(getStreetSegmentCurvePoint(path[i], k - 2));

                        //find_degree_(k1point, kpoint)
                        if ((find_degree_(pre_point, k_point) + 90 - find_degree_(pre2_point, pre_point)) > 0) {
                            cout << " Turn left, ";
                        } else if ((find_degree_(pre_point, k_point) + 90 - find_degree_(pre2_point, pre_point)) < 0) {
                            cout << " Turn right, ";
                        }
                        else;
                        travelDistance = find_distance_between_two_points(getStreetSegmentCurvePoint(path[i], k-1), getStreetSegmentCurvePoint(path[i], k));
                        cout << " Go straight " << travelDistance << " meters."  << endl;

                    }
                }
            }
        }
    }
    return;
}

/*===========================
            GNU input
 ============================*/
char* street_name_generator(const char* stem_text, int state) {
    //Static here means a variable's value persists across function invocations
    static int count;
    std::vector<string> streetName;
    std::vector<const char*> charStrings;

    if (state == 0) {
        //We initialize the count the first time we are called
        //with this stem_text
        count = -1;
    }

    int text_len = strlen(stem_text);

    for (unsigned i = 0; i < getNumberOfStreets(); i++) {
        streetName.push_back(getStreetName(i));
        charStrings.push_back(const_cast<char*> (streetName[i].c_str()));
    }

    //Search through intersection_names until we find a match
    while (count < (int) charStrings.size() - 1) {
        count++;

        if (strncmp(charStrings[count], stem_text, text_len) == 0) {
            //Must return a duplicate, Readline will handle
            //freeing this string itself.

            return strdup(charStrings[count]);
        }

    }

    //No more matches
    return NULL;
}

//command completion of street names

char** street_Name_command_completion(const char* stem_text, int start, int end) {
    char ** matches = NULL;

    if (start != 0) {
        //Only generate completions if stem_text'
        //is not the first thing in the buffer
        matches = rl_completion_matches((char *) stem_text, &street_name_generator);
    }

    return matches;
}


