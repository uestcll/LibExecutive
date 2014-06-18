#ifndef DefinitionForConst_H
#define DefinitionForConst_H

#define LOG_FILE_NAME "logger"
#define FILE_PATH_FOR_RECORD_LOCKING "/tmp/"

const int MUTEX_USE_RECORD_LOCK = 0;
const int MUTEX_USE_RECORD_LOCK_AND_PTHREAD = 1;
const int MUTEX_USE_SHARED_PTHREAD = 2;

const int ID_FOR_KEY = 32;
#define FILE_PATH_FOR_SHARED_MEMORY "/tmp/"

#define SHARED_SPACE_FOR_SHARED_MUTEX_ALLOCATOR "shared_space_for_shared_mutex_allocator"
#define SHARED_SPACE_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR "shared_space_for_shared_condition_variable_allocator"
#define SHARED_SPACE_FOR_SHARED_EVENT_ALLOCATOR "shared_space_for_shared_event_allocator"

#define MUTEX_FOR_SHARED_CONDITION_VARIABLE_ALLOCATOR "mutex_for_shared_condition_variable_allocator"
#define MUTEX_FOR_SHARED_MUTEX_ALLOCATOR "mutex_for_shared_mutex_allocator"
#define MUTEX_FOR_SHARED_EVENT_ALLOCATOR "mutex_for_shared_event_allocator"

const int LENGTH_OF_SHARED_OBJECT_NAME = 256;

const int UNINITIALIZED_SHARED_OBJECT = 0;
const int INITIALIZED_SHARED_OBJECT = 1;
const int ALLOCATED_SHARED_OBJECT = 2;

const int NUMBER_OF_SHARED_OBJECT = 1024;
const int MAGIC_NUMBER_FOR_SHARED_OBJECT = 0x12345678;

const int IOVECTOR_DELETE = 0;
const int IOVECTOR_STAIN = 1;
const int IOVECTOR_NON_DELETE = 2;

const int LISTEN_BACKLOG = 128;

const int SOCKET_TYPE_TCP = 1;
const int SOCKET_TYPE_UDP = 2;

const int MESSAGE_ID_BASE = 10000;
const int MESSAGE_ID_BASE_USER_DEFINED = 20000;

const int MESSAGE_ID_FOR_CLIENT_ARRIVED = MESSAGE_ID_BASE + 100;
const int MESSAGE_ID_FOR_CHANNEL_CLOSED = MESSAGE_ID_BASE + 101;

const int MESSAGE_ID_FOR_CHANNEL_ERROR = MESSAGE_ID_BASE + 102;

#endif