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
    int actor_index = search_data_array(actorFile, player);
    if (actor_index > 0) {
        int actor_offset = *((int*)actorFile + actor_index);
        get_movies_from_actor(actor_offset, films);
        for(size_t i=0; i<films.size(); ++i) {
            std::cout << films[i].title << "\n";
        }
        return true;
    }

    // memcpy(&first_actor_offset, (char*)actorFile + 4, sizeof(int));
    // std::cout << "offset of first actor is: " << first_actor_offset << "\n";
    // std::cout << "offset of first actor: " << *(int*)((char*)actorFile + 4) << "\n";
    // std::cout << "name of first actor: " << *((char*)actorFile + first_actor_offset) << "\n";
    // char* first_actor = strdup(((char*)actorFile + first_actor_offset));
    // strcpy(first_actor, (char*)((int*)actorFile + first_actor_offset));
    // std::cout << "name of first actor: " << first_actor << "\n";
    // std::string s1(first_actor);
    // std::cout << "Length of name of first actor: " << s1.length() << "\n";
    // memcpy(&first_actor_offset, (char*)actorFile+first_actor_offset+s1.length()+1, sizeof(int));
    // std::cout << "Number of movies " << s1 << " is appeared in is: " << first_actor_offset << "\n";
    // free(first_actor);
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
    std::cout << "The number of movies is: " << num_of_movies << "\n";
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
        std::cout << film_obj.title << " was released in " << film_obj.year << "\n";
    }
}

/* 
 * Method: serach_data_array
 * Take the raw data array blob and run a binary search on it
 * & return -1 if nothing is found as a match or return the offset of
 * the match string.
 *
 * @param: data_file: pointer to the data file to be search.
 * @param: string to search for.
 *
 * @return: -1 if no match is found & offset of the matching
 *         string in the file if the match is found.
 *  */
int imdb::search_data_array(const void* data_file, const string& player) const {
    int start, mid, end; 
    start = 1;
    end = *(int*)data_file;
    int player_is = 0; 
    while(start <= end) {
        mid = (start+end)/2;
        player_is = string_comp(data_file, mid, player);
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
 * @param data_file: The data_file where one of the string to compare is located
 * @param address_offset: This int contains the offset in the data_file where one of the
 *                        strings is located.
 * @param player: String to compare.
 *
 * @return: Returns an int. Returns 0 if the strings are same. -1 if player is smaller and 1
 *          if greater.
 *  */
int imdb::string_comp(const void* data_file, const int address_offset, const string& player) const{
    int actor_offset = *((int*)data_file + address_offset);
    char* actor_name = (char*)data_file + actor_offset;
    if (std::string(actor_name) > player) {
        return -1;
    }
    else if(std::string(actor_name) < player) {
        return 1;
    }
    return 0; 
}

// bool imdb::getCast(const film& movie, vector<string>& players) const {

    // return false;
// }

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