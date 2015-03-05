#ifndef DEFINES_H
#define DEFINES_H

#ifdef Q_OS_WIN32
    #define OSINDEX 1
#endif
#ifdef Q_OS_LINUX
    #define OSINDEX 2
#endif
#ifdef Q_OS_MAC
    #define OSINDEX 2
#endif

#ifndef OSINDEX
    #define OSINDEX -1
#endif

// Delimiters from config, and use in engine.
// If these change need to change, make the changes
// in the config/command_map.lqwd too
#define DELIMITER_LIST ","
#define DELIMITER_BLOCK ":"

// References from config
#define REF_COMMENT '#'     // Needs '' for char comparison
#define REF_UNDEFINED "!"
#define REF_PRIOR "*"

#endif // DEFINES_H
