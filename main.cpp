//
//  main.cpp
//
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>  //for ostringsream
#include <iomanip>  //for setprecision
#include <fstream>

using namespace std;

string parseFreesoundPageForDownloadLink(string filename)
{
    ifstream fp;
    fp.open(filename.c_str());
    string s = "";
    string downloadLink = "";
    while(getline(fp, s)) {
        int found = s.find("href=\"");
        int found2 = s.find(".wav");
        int found3 = s.find("/sounds/");
        if (found != string::npos && found2 != string::npos && found3 != string::npos) {
            downloadLink = s.substr(found + 6, found2-found-2);
            cout << "Download link: " << downloadLink;
            string cmd = "curl -b cookie2.txt -G http://www.freesound.org" + downloadLink + " > test.wav";
            system(cmd.c_str());
            break;
        }   
    }
    return downloadLink;
}

int main()
{   
    string api_key = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    string query = "rain";
    string username = "username";
    string password = "password";
    
    // get cookie for csrf token
    system("curl -s -c cookie.txt http://www.freesound.org/home/login/  > output2.txt");
    // parse cookie file for csrftoken
    string csfrmiddlewaretoken = "";
    {
        ifstream fp;
        fp.open("cookie.txt");
        string s = "";
        while (getline(fp, s)) {
            int found = s.find("csrftoken");
            if (found != string::npos) {
                csfrmiddlewaretoken = s.substr(found + 10);
                cout << "Found: " << csfrmiddlewaretoken << endl;
            }
        }
    }
    // now get revised cookie using login info and csrf token
    stringstream ss;
    ss << "curl -b cookie.txt -c cookie2.txt -d \"csrfmiddlewaretoken=" 
        << csfrmiddlewaretoken 
        << "&next=/&username=" << username << "&password=" << password << "\" -L http://www.freesound.org/home/login/ > output3.txt";
    system(ss.str().c_str());
    
    
    // get JSON
    string cmd = "curl -G -d \"q=" + query + "\\&api_key=" + api_key + "\" http://www.freesound.org/api/sounds/search/ > output.txt";
    system(cmd.c_str());
    {
        // open up JSON
        ifstream fp;
        fp.open("output.txt");
        string s = "";
        
        // get first two lines - second line has number of results
        getline(fp, s);
        getline(fp, s);
        char buf[256];
        int num_results;
        sscanf(s.c_str(), "%s %d,", buf, &num_results);
        cout << "Found " << num_results << " results for \"" << query << "\"." << endl;
        
        // parse result of query for download links 
        while(getline(fp, s)) {
            
            // add code for stripping sound id number
            
            
            // add code for stripping tags
            
            
            // find the download link and download the WAVE file
            int found = s.find("\"url\": \"");
            int found2 = s.find("sounds/");
            if (found != string::npos && found2 != string::npos) {
                string url = s.substr(found + 7);
                url = url.substr(0, url.length() - 2);
                
                string cmd = "curl -b cookie2.txt " + url + " > url.txt";
                system(cmd.c_str());
                
                string downloadLink = parseFreesoundPageForDownloadLink("url.txt");
                
            }
        }
    }
    
    return 0;
}