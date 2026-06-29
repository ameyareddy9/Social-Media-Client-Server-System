#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <bits/stdc++.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

class Instagram {
private:
    unordered_map<int, vector<pair<int,int>>> tweets;
    unordered_map<int, unordered_set<int>> following;

    int timer = 0;
    mutex mtx;

public:

    void post(int userId, int tweetId) {
        lock_guard<mutex> lock(mtx);
        tweets[userId].push_back({timer++, tweetId});
    }

    vector<int> getNewsFeed(int userId) {
        // Min-heap to store posts based on timestamp
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;

        // Insert user's own posts
        for (const auto& t : tweets[userId]) {
            pq.push(t);
            // Keep heap size max 10
            if (pq.size() > 10)
                pq.pop();
        }

        // Insert posts from users being followed
        for (int followee : following[userId]) {
            for (const auto& t : tweets[followee]) {
                pq.push(t);
                if (pq.size() > 10)
                    pq.pop();
            }
        }

        // Extract posts from heap in reverse order
        vector<int> res;
        while (!pq.empty()) {
            res.push_back(pq.top().second);
            pq.pop();
        }
        reverse(res.begin(), res.end());
        return res;
    }

    void follow(int followerId, int followeeId) {
        lock_guard<mutex> lock(mtx);

        if(followerId == followeeId)
            return;

        following[followerId].insert(followeeId);
    }

    void unfollow(int followerId, int followeeId) {
        lock_guard<mutex> lock(mtx);

        following[followerId].erase(followeeId);
    }
};

int main() {

    WSADATA wsa;

    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(server_fd == INVALID_SOCKET) {
        cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if(bind(server_fd,
            (sockaddr*)&address,
            sizeof(address)) == SOCKET_ERROR) {

        cerr << "Bind failed\n";
        return 1;
    }

    listen(server_fd, 5);

    cout << "Instagram server running on port 8080...\n";

    Instagram instagram;

    while(true) {

        SOCKET client_socket = accept(server_fd, NULL, NULL);

        if(client_socket == INVALID_SOCKET)
            continue;

        char buffer[2048] = {0};

        int bytesReceived =
            recv(client_socket, buffer, sizeof(buffer), 0);

        if(bytesReceived > 0) {

            string request(buffer);

            stringstream ss(request);

            string cmd;
            ss >> cmd;

            string response;

            if(cmd == "POST") {

                int userId, InstaId;

                ss >> userId >> InstaId;

                instagram.post(userId, InstaId);

                response = "Posted";
            }

            else if(cmd == "FOLLOW") {

                int follower, followee;

                ss >> follower >> followee;

                instagram.follow(follower, followee);

                response = "Follow Success";
            }

            else if(cmd == "UNFOLLOW") {

                int follower, followee;

                ss >> follower >> followee;

                instagram.unfollow(follower, followee);

                response = "Unfollow Success";
            }

            else if(cmd == "FEED") {

                int userId;

                ss >> userId;

                vector<int> feed =
                    instagram.getNewsFeed(userId);

                if(feed.empty()) {

                    response = "Feed Empty";
                }
                else {

                    for(int post : feed) {
                        response += to_string(post);
                        response += " ";
                    }
                }
            }

            else {

                response = "Unknown Command";
            }

            response += "\n";

            send(
                client_socket,
                response.c_str(),
                response.length(),
                0
            );
        }

        closesocket(client_socket);
    }

    WSACleanup();

    return 0;
}