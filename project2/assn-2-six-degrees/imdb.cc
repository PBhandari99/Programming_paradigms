#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 
#include "imdb.h"

using namespace std;

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

bool imdb::getCredits(const string& player, vector<film>& films) const {
    // int first_actor_offset;
    int actor_index = search_data_array(player);
    if (actor_index > 0) {
        int actor_offset = *((int*)actorFile + actor_index);
        get_movies_from_actor(actor_offset, films);
        return true;
    }
    return false; 
}

/* 
 * Method: get_movies_from_actor
 *
 * Given the actor offset the method adds all the moveies the given actor is appeared in
 * into the vecor passed.
 * @param actor_offset: Offset in the actorFile when the actor's name is located.
 * @param films: The vector to which the films should be inserted.
 *  
 *  */

void imdb::get_movies_from_actor(const int actor_offset, std::vector<film>& films) const {
    int length_of_actor_name = std::string((char*)actorFile + actor_offset).length();
    int offset_into_no_of_movies;
    if (length_of_actor_name % 2 != 0) {
        // if the numbers of char in the actor's name is even than the name ends with only one \0,
        // hence +1 in line below.
        offset_into_no_of_movies = actor_offset + length_of_actor_name + 1;
        add_movies_to_vec(offset_into_no_of_movies, films);
    }
    else {
        // if the numbers of char in the actor's name is odd than the name ends with two \0s,
        // hence +2 in line below.
        offset_into_no_of_movies = actor_offset + length_of_actor_name + 2;
        add_movies_to_vec(offset_into_no_of_movies, films);
    }
}

/* 
 * Method: add_movies_to_vec
 *
 * This method will add films to the vector passed based on the offset into the data file.
 * @param offset_into_no_of_movies: This offset is integer that points to a point in actorFile
 *                                  that has the number of movies the adjacent actor has appeared 
 *                                  in. Check the function calls.
 * @param films: The vector to be populated with films.
 *  */
void imdb::add_movies_to_vec(const int offset_into_no_of_movies, std::vector<film>& films) const {
    // This is the actual data that is contained in that offset.
    int num_of_movies = *(short*)((char*)actorFile + offset_into_no_of_movies);
    // This offset contains the offset that leads to the actual movie names in movieFile.
    // The %, modulus is done because if the number of bytes used in the name of the actor and
    // the number of movies he/she is appeared in is not a multiple of four than the extra bytes
    // are filled with "\0"s to make it multiple of four.
    int offset_to_movie_info = offset_into_no_of_movies + (4-(offset_into_no_of_movies%4));
    for (int i=0; i<num_of_movies; ++i) {
        char* movie = (char*)movieFile + (*(int*)((char*)actorFile + offset_to_movie_info + i*4));
        film film_obj;
        film_obj.title = std::string(movie);
        // u_int8_t cast is done since the year of release is only 1 byte.
        film_obj.year = *((u_int8_t*)movie + film_obj.title.length()+1) + 1900;
        films.push_back(film_obj);
    }
}

/* 
 * Method: serach_data_array
 * Take the raw data array blob and run a binary search on it
 * & return -1 if nothing is found as a match or return the offset of
 * the match string.
 *
 * @param: string to search for.
 *
 * @return: -1 if no match is found & offset of the matching
 *         string in the file if the match is found.
 *  */
int imdb::search_data_array(const string& player) const {
    int start, mid, end; 
    start = 1;
    end = *(int*)actorFile;
    int player_is = 0; 
    while(start <= end) {
        mid = (start+end)/2;
        player_is = string_comp(mid, player);
        if (player_is == 0) {
            return mid;
        } 
        else if (player_is < 0) {
            end = mid-1;
        }
        else {
            start = mid + 1;
        }
    }
    return -1;
}

/* 
 * Method: string_comp: Compares to strings and return the result.
 *
 * @param address_offset: This int contains the offset in the data_file where one of the
 *                        strings is located.
 * @param player: String to compare.
 *
 * @return: Returns an int. Returns 0 if the strings are same. -1 if player is smaller and 1
 *          if greater.
 *  */
int imdb::string_comp(const int address_offset, const string& player) const{
    int actor_offset = *((int*)actorFile + address_offset);
    char* actor_name = (char*)actorFile + actor_offset;
    if (std::string(actor_name) > player) {
        return -1;
    }
    else if(std::string(actor_name) < player) {
        return 1;
    }
    return 0; 
}

// binary search the movieFile array of memory for matching movie.
int imdb::search_movie_data_array(const film& movie) const {
    int start, mid, end; 
    start = 1;
    end = *(int*)movieFile;
    int player_is = 0; 
    while(start <= end) {
        mid = (start+end)/2;
        player_is = comp_films(mid, movie);
        if (player_is == 0) {
            return mid;
        } 
        else if (player_is < 0) {
            end = mid-1;
        }
        else {
            start = mid + 1;
        }
    }
    return -1;
}

// compare two movies if there is a match, helper method for the binary search.
int imdb::comp_films(const int address_offset, const film& movie) const {
    film movie_search;
    int movie_offset = *((int*)movieFile+ address_offset);
    char* movie_name = (char*)movieFile + movie_offset;
    movie_search.title = std::string(movie_name);

    // As the year is a single byte and is counted form 1900, hence the addition and u_int8_t cast.
    movie_search.year = *((u_int8_t*)movie_name + movie_search.title.length()+1) + 1900;
    if(movie == movie_search) {
        return 0;
    }
    else if(movie < movie_search) {
        return -1;
    }
    return 1;
}

// Adds the actor in the movie to the vector.
void imdb::add_actors_to_vec(const int no_of_actors, const int movie_offset,
        const int offset_to_actor_offset, vector<string>& players) const {
    int actor_offset;
    for(int i=0; i<no_of_actors; ++i) {
        actor_offset = *(int*)((char*)movieFile + movie_offset + offset_to_actor_offset + i*4);
        char* actor = (char*)actorFile + actor_offset;
        players.push_back(std::string(actor));
    }
}


// Helper method to get all the actors in the given movie.
void imdb::get_actors(const int movie_index, vector<string>& players) const {
    int movie_offset = *((int*)movieFile + movie_index);
    int length_of_movie_title_w_year = std::string(
            (char*)movieFile + movie_offset).length() + 1 + 1; 
    // If the numer of bytes needed for the name of the movie with "/0" and 1 byte year
    // is odd than one extra byte of "/0" is added at the end of year before number of actors.
    if (length_of_movie_title_w_year % 2!=0) {
        length_of_movie_title_w_year += 1;
    }
    int no_of_actors = *(short*)((char*)movieFile + movie_offset + length_of_movie_title_w_year);
    // If the length_of_movie_title_w_year and number of actors is multiple of 4 then no need
    // to pad with extra 2 bytes.
    if((length_of_movie_title_w_year+2)%4==0) {
        int offset_to_actor_offset = length_of_movie_title_w_year + 2;
        add_actors_to_vec(no_of_actors, offset_to_actor_offset, movie_offset, players);
    }
    else {
        int offset_to_actor_offset = length_of_movie_title_w_year + 2 + 2;
        add_actors_to_vec(no_of_actors, offset_to_actor_offset, movie_offset, players);
    }
}

bool imdb::getCast(const film& movie, vector<string>& players) const {
    int movie_index = search_movie_data_array(movie);
    if (movie_index > 0) {
        get_actors(movie_index, players);
        return true;
    }
    return false;
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
