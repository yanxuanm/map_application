#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m4.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <cstdlib>

using namespace std;
extern std::map<double, unsigned> area_to_closed_features;

char** map_Name_command_completion(const char* stem_text, int start, int end);
char* map_name_generator(const char* stem_text, int state);
char** street_Name_command_completion(const char* stem_text, int start, int end);
char* street_name_generator(const char* stem_text, int state);

vector<const char*> map_names = {"hamilton_canada", "cairo_egypt", "moscow",
    "newyork", "saint_helena", "toronto", "toronto_canada", "london_england"};

int main() {


    //Use tab for auto completion
    rl_bind_key('\t', rl_complete);
    //Use our function for auto-complete
    rl_attempted_completion_function = map_Name_command_completion;
    //Tell readline to handle double and single quotes for us
    rl_completer_quote_characters = strdup("\"\'");


    vector<string> names;
    names.push_back("hamilton_canada");
    names.push_back("cairo_egypt");
    names.push_back("moscow");
    names.push_back("newyork");
    names.push_back("saint_helena");
    names.push_back("toronto");
    names.push_back("toronto_canada");
    names.push_back("london_england");


    //input map name to open maps
    while (!cin.eof()) {
        cout << "Input the name of map(cairo_egypt;hamilton_canada;moscow;newyork;saint_helena;toronto;toronto_canada;london_england)" << endl;
        cin.clear();
        string name;

        char* buf; //Buffer of line entered by user
        while ((buf = readline("> ")) != NULL) { //Prompt the user for input

            //rl_bind_key ('\t', rl_complete);

            if (strcmp(buf, "") != 0) { //Only save non-empty commands
                add_history(buf);
                name = std::string(buf);
            }

            boost::to_lower(name);

            break;

            //readline generates a new buffer every time, 
            //so we need to free the current one after we've finished
            free(buf);
            //buf = NULL; //Mark it null to show we freed it
        }

        //If the buffer wasn't freed in the main loop we need to free it now
        // Note: if buf is NULL free does nothing
        //string name(buf);
        free(buf);

        while (find(names.begin(), names.end(), name) == names.end()) {
            cout << "You entered wrong name, enter again" << endl;

            while ((buf = readline("> ")) != NULL) { //Prompt the user for input

                rl_bind_key('\t', rl_complete);


                if (strcmp(buf, "") != 0) { //Only save non-empty commands
                    add_history(buf);
                    name = std::string(buf);
                }

                boost::to_lower(name);

                break;

                //readline generates a new buffer every time, 
                //so we need to free the current one after we've finished
                free(buf);
                //buf = NULL; //Mark it null to show we freed it
            }

            //If the buffer wasn't freed in the main loop we need to free it now
            // Note: if buf is NULL free does nothing
            free(buf);
        }

        string map = "/cad2/ece297s/public/maps/" + name + ".streets.bin";
        load_map(map);
        //        for (auto it = area_to_closed_features.begin(); it != area_to_closed_features.end(); it++) {
        //            cout << it->first << "        " << it->second << endl;
        //
        //        }
        //            vector<unsigned> path;
        //    path = find_path_between_intersections(80481, 34196);
        //    print_vector(path);
        //draw_map();
                std::vector<DeliveryInfo> deliveries = {DeliveryInfo(9140, 19741), DeliveryInfo(67133, 63045)};
                vector<unsigned> depots = {24341, 84950};
                vector<unsigned> result_path = traveling_courier(deliveries, depots);
                


        close_map();
    }
    return 0;
}


//-------------functions used for command auto-completion---------------

char* map_name_generator(const char* stem_text, int state) {
    //Static here means a variable's value persists across function invocations
    static int count;

    if (state == 0) {
        //We initialize the count the first time we are called
        //with this stem_text
        count = -1;
    }

    int text_len = strlen(stem_text);

    //Search through intersection_names until we find a match
    while (count < (int) map_names.size() - 1) {
        count++;
        if (strncmp(map_names[count], stem_text, text_len) == 0) {
            //Must return a duplicate, Readline will handle
            //freeing this string itself.       
            return strdup(map_names[count]);

        }
    }

    //No more matches
    return ((char *) NULL);
}
//command completion of map names

char** map_Name_command_completion(const char* stem_text, int start, int end) {
    char ** matches = NULL;

    if (start == 0) {
        //Only generate completions if stem_text' 
        //is not the first thing in the buffer
        matches = rl_completion_matches((char *) stem_text, &map_name_generator);

    }

    return matches;
}


