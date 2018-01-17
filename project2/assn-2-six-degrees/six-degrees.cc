#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <list>
#include <map>
#include <utility>
#include <set>
#include <string>
#include <vector>

#include "imdb.h"
#include "path.h"

using namespace std;

/**
 * Using the specified prompt, requests that the user supply
 * the name of an actor or actress.  The code returns
 * once the user has supplied a name for which some record within
 * the referenced imdb existsif (or if the user just hits return,
 * which is a signal that the empty string should just be returned.)
 *
 * @param prompt the text that should be used for the meaningful
 *               part of the user prompt.
 * @param db a reference to the imdb which can be used to confirm
 *           that a user's response is a legitimate one.
 * @return the name of the user-supplied actor or actress, or the
 *         empty string.
 */

static string promptForActor(const string& prompt, const imdb& db)
{
  string response;
  while (true) {
    cout << prompt << " [or <enter> to quit]: ";
    getline(cin, response);
    if (response == "") return "";
    vector<film> credits;
    if (db.getCredits(response, credits)) return response;
    cout << "We couldn't find \"" << response << "\" in the movie database. "
	 << "Please try again." << endl;
  }
}
/* 
 * This method does a BFS and return the Path to the target from source.
 *
 * @param source first actor to where the search to start.
 * @param target the last actor to search for.
 * @param imdb instance to use.
 *
 * @return zero when exiting the method.
 *  */
int generateShortestPath(string source, string target, const imdb& db) {
    list<path> partialPaths;
    set<string> seenActors;
    set<film> seenFilms;
    map<string, vector<film>> films_by_actor;
    map<film, vector<string>> actors_by_movies;

    path startPlyer(source);
    partialPaths.push_back(startPlyer);
    while(!partialPaths.empty() && partialPaths.front().getLength() <= 5) {
        path initial_path = partialPaths.front();
        partialPaths.pop_front();
        const string last_actor = initial_path.getLastPlayer();
        vector<film> films;
        if (films_by_actor.find(last_actor) == films_by_actor.end()) {
            db.getCredits(last_actor, films);
            std::pair<string, vector<film>> actor_film_pair(last_actor, films);  
            films_by_actor.insert(actor_film_pair);
        }
        else {
            films = films_by_actor[last_actor];
        }
        for (auto movie : films) {
            if (seenFilms.find(movie) == seenFilms.end()) {
                seenFilms.insert(movie);
                vector<string> players;
                if (actors_by_movies.find(movie) == actors_by_movies.end()) {
                    db.getCast(movie, players);
                    std::pair<film, vector<string>> movie_actors_pair(movie, players);  
                    actors_by_movies.insert(movie_actors_pair);
                }
                else {
                    players = actors_by_movies[movie];
                }
                for (auto actor : players) {
                    if (seenActors.find(actor) == seenActors.end()) {
                        seenActors.insert(actor);
                        path clone_path = initial_path;
                        clone_path.addConnection(movie, actor);
                        if (actor == target) {
                            cout << clone_path;
                            return 0;
                        }
                        partialPaths.push_back(clone_path);
                    }
                }
            }
        }
    }
    cout << endl << "No path between " << source << " and " << target << endl << endl;
    return 0;
}


/**
 * Serves as the main entry point for the six-degrees executable.
 * There are no parameters to speak of.
 *
 * @param argc the number of tokens passed to the command line to
 *             invoke this executable.  It's completely ignored
 *             here, because we don't expect any arguments.
 * @param argv the C strings making up the full command line.
 *             We expect argv[0] to be logically equivalent to
 *             "six-degrees" (or whatever absolute path was used to
 *             invoke the program), but otherwise these are ignored
 *             as well.
 * @return 0 if the program ends normally, and undefined otherwise.
 */

int main(int argc, const char *argv[])
{
  imdb db(determinePathToData(argv[1])); // inlined in imdb-utils.h
  if (!db.good()) {
    cout << "Failed to properly initialize the imdb database." << endl;
    cout << "Please check to make sure the source files exist and that you have permission to read them." << endl;
    exit(1);
  }
  
  while (true) {
    string source = promptForActor("Actor or actress", db);
    if (source == "") break;
    string target = promptForActor("Another actor or actress", db);
    if (target == "") break;
    if (source == target) {
      cout << "Good one.  This is only interesting if you specify two different people." << endl;
    } else {
      generateShortestPath(source, target, db);
    }
  }
  cout << "Thanks for playing!" << endl;
  return 0;
}

