#ifndef INCLUDED_RELEASE_H
#define INCLUDED_RELEASE_H

#define HCNSE_NAME                     "HCNSE"
#define HCNSE_VERSION                  "0.0.3"

#define HCNSE_VERSION_INFO             (HCNSE_NAME " version " HCNSE_VERSION)
#define BUILD_TIME                     (__DATE__ " " __TIME__)

#define hcnse_version_info()           (HCNSE_VERSION_INFO)
#define hcnse_build_time()             (BUILD_TIME)

#endif /* INCLUDED_RELEASE_H */
