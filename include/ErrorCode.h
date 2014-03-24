#ifndef ErrorCode_H
#define ErrorCode_H

const int NORMAL_ERROR = 10000;

const int RECEIVED_ZERO_BYTE = NORMAL_ERROR + 10;
const int RECEIVED_ERROR = NORMAL_ERROR + 11;
const int RECEIVED_CLOSE = NORMAL_ERROR + 13;

const int MSG_RECEIVED_ZERO = RECEIVED_ZERO_BYTE;
const int MSG_RECEIVED_ERROR = RECEIVED_ERROR;

const int IOVECTOR_ITERATOR_END = NORMAL_ERROR + 20;

const int DATA_POSTER_INITIALIZE_SUCCESS = NORMAL_ERROR + 30;
const int DATA_POSTER_INITIALIZE_PENDING = NORMAL_ERROR + 31;
const int DATA_POSTER_INITIALIZE_ERROR = NORMAL_ERROR + 32;

const int DATA_POSTER_POST_SUCCESS = NORMAL_ERROR + 40;
const int DATA_POSTER_POST_PENDING = NORMAL_ERROR + 41;
const int DATA_POSTER_POST_PARTIAL = NORMAL_ERROR + 42;
const int DATA_POSTER_POST_ERROR = NORMAL_ERROR + 43;

const int MSG_INITIALIZE_SUCCESS = DATA_POSTER_INITIALIZE_SUCCESS;
const int MSG_INITIALIZE_PENDING = DATA_POSTER_INITIALIZE_PENDING;
const int MSG_INITIALIZE_ERROR = DATA_POSTER_INITIALIZE_ERROR;

const int MSG_POST_SUCCESS = DATA_POSTER_POST_SUCCESS;
const int MSG_POST_PARTIAL = DATA_POSTER_POST_PARTIAL;
const int MSG_POST_PENDING = DATA_POSTER_POST_PENDING;
const int MSG_POST_ERROR = DATA_POSTER_POST_ERROR;

const int TRY_CONNECT_END = NORMAL_ERROR + 50;
const int CONNECT_PENDING = NORMAL_ERROR + 51;
const int ACCEPT_PENDING = NORMAL_ERROR+ 52;
const int IO_PENDING = NORMAL_ERROR + 53;

#endif