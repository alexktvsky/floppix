#ifndef INCLUDED_RELEASE_H
#define INCLUDED_RELEASE_H

#define SERVER_NAME                  "Server"
#define SERVER_VERSION               "0.0.2"

#define SERVER_VERSION_INFO          (SERVER_NAME " version " SERVER_VERSION)
#define BUILD_TIME                   (__DATE__ " " __TIME__)

#define get_version_info()           (SERVER_VERSION_INFO)
#define get_build_info()             (BUILD_TIME)

#endif /* INCLUDED_RELEASE_H */
